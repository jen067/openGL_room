#include <iostream>
#include <glew/include/GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <array>

#include "CCamera.h"
#include "wmhandler.h"
#include "arcball.h"

#include "../common/CMaterial.h"
#include "../models/CQuad.h"
#include "../models/CCube.h"
#include "../models/CSphere.h"
#include "../models/CTeapot.h"
#include "../common/CLight.h"
#include "../models/CObjModel.h"
#include "CButton.h"

//#define SPOT_TARGET  // Example 2 解開

extern Arcball g_arcball;

bool  g_bCamRoting = false;
bool  g_bfirstMouse = true; 
float g_lastX = 400, g_lastY = 400;
float g_mouseSens = 0.005f; 
bool g_dKeyReleased = true;

extern CCube g_centerloc;
extern GLuint g_shadingProg;
extern glm::vec3 g_eyeloc;

extern CLight g_light;
extern CLight g_spotLightL;
extern CLight g_spotLightR;
extern CLight g_spotLightB;
extern CLight g_Lamp;
extern CLight g_spotLightWall;

extern CObjModel g_wallLamp;
extern CObjModel* g_door;

extern CMaterial g_matOliveWood; // 橄欖綠
extern CMaterial g_matLatte; // 奶茶
extern CMaterial g_matCaramelRed; // 珊瑚紅
extern CMaterial g_matAlmond; // 杏色
extern CMaterial g_matKhakiGold; // 金色
extern CMaterial g_matGrassOlive; // 草
extern CMaterial g_matWarmWalnut; // 胡桃
extern CMaterial g_matLampCoverWhite; // 白
extern CMaterial g_matDarkMetal; // 金屬黑
extern CMaterial g_matOliveWoodDeep; // 深綠
extern CMaterial g_matSteelBlue; // 深藍
extern CMaterial g_brownMaterial; // 咖啡
extern CMaterial g_ghostWhite;

extern std::array<CButton, 5> g_button;

extern CSphere  g_sphere; 

struct RoomBounds {
    float minX = -10.0f;  // -0.8/2
    float maxX = 10.0f;   // 0.8/2
    float minZ = -10.0f;  // -0.8/2
    float maxZ = 10.0f;   // 0.8/2
    float minY = 2.0f;   // 地板高度
    float maxY = 16.0f;   // 天花板
};

RoomBounds g_roomBounds;

#ifdef SPOT_TARGET
extern CCube g_spotTarget;
#endif


Arcball g_arcball;

glm::vec3 eye = CCamera::getInstance().getViewLocation();
glm::vec3 center = CCamera::getInstance().getCenterLocation(); 
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

// 鏡頭前向量
glm::vec3 front = glm::normalize(center - eye);
// 前方向（XZ 平面）
glm::vec3 forward = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
// 右方向
glm::vec3 right = glm::normalize(glm::cross(forward, up));

float speed = 0.1f;
int lampPress = 0;

// 邊界檢查函數
glm::vec3 clampToRoom(const glm::vec3& position, const RoomBounds& bounds) {
    return glm::vec3(
        glm::clamp(position.x, bounds.minX, bounds.maxX),
        glm::clamp(position.y, bounds.minY, bounds.maxY),
        glm::clamp(position.z, bounds.minZ, bounds.maxZ)
    );
}

// 檢查新位置是否在房間內
bool isInsideRoom(const glm::vec3& position, const RoomBounds& bounds) {
    return (position.x >= bounds.minX && position.x <= bounds.maxX &&
        position.y >= bounds.minY && position.y <= bounds.maxY &&
        position.z >= bounds.minZ && position.z <= bounds.maxZ);
}


void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    // Arcball* arcball = static_cast<Arcball*>(glfwGetWindowUserPointer(window));
    g_arcball.onMouseButton(button, action, xpos, ypos);
    //std::cout << "button = " << button << "action = " << action << "mods = " << mods << std::endl;
    if (button == GLFW_MOUSE_BUTTON_LEFT )
    {
		if (action == GLFW_PRESS)
		{
            g_bCamRoting = true;

            if (g_button[0].isMouseOver((float)xpos, height - (float)ypos)) { // upperarm
                g_button[0].handleClick((float)xpos, height - (float)ypos);
                g_light.setMotionEnabled();
            }
            if (g_button[1].isMouseOver((float)xpos, height - (float)ypos)) { // forearm
                g_button[1].handleClick((float)xpos, height - (float)ypos);
                g_spotLightL._enabled = !g_spotLightL._enabled;
                g_spotLightL.updateToShader();
            }
            if (g_button[2].isMouseOver((float)xpos, height - (float)ypos)) { // forearm
                g_button[2].handleClick((float)xpos, height - (float)ypos);
                g_spotLightB._enabled = !g_spotLightB._enabled;
                g_spotLightB.updateToShader();
            }
            if (g_button[3].isMouseOver((float)xpos, height - (float)ypos)) { // forearm
                g_button[3].handleClick((float)xpos, height - (float)ypos);
                g_spotLightR._enabled = !g_spotLightR._enabled;
                g_spotLightR.updateToShader();
            }
            if (g_button[4].isMouseOver((float)xpos, height - (float)ypos)) { // forearm
                g_button[4].handleClick((float)xpos, height - (float)ypos);
                lampPress %= 3;
                lampPress += 1;
                //std::cout << lampPress << std::endl;
                switch (lampPress)
                {
                case 3:
                    g_Lamp.setAmbient(glm::vec4(0.6f, 0.5f, 0.2f, 1.0f));
                    g_Lamp.setSpecular(glm::vec4(1.0f, 0.75f, 0.3f, 1.0f));
                    g_Lamp.setAttenuation(1.0f, 0.0095f, 0.01f);
                    g_Lamp.updateToShader();
                    break;
                case 1:
                    g_Lamp.setAmbient(glm::vec4(0.5f, 0.6f, 0.3f, 1.0f));
                    g_Lamp.setSpecular(glm::vec4(1.0f, 0.8f, 0.5f, 1.0f));
                    g_Lamp.setAttenuation(1.0f, 0.0095f, 0.01f);
                    g_Lamp.updateToShader();
                    break;
                case 2:
                    g_Lamp.setAmbient(glm::vec4(0.1f, 0.12f, 0.05f, 1.0f));
                    g_Lamp.setSpecular(glm::vec4(0.3f, 0.25f, 0.2f, 1.0f));
                    g_Lamp.setAttenuation(1.0f, 0.045f, 0.075f);
                    g_Lamp.updateToShader();
                    break;
                default:
                    break;
                }
                
            }
		}
        else if (action == GLFW_RELEASE)
        {
            g_bCamRoting = false; 
			g_bfirstMouse = true; // 滑鼠釋放
        }      
    }
}
// ---------------------------------------------------------------------------------------

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    g_arcball.onCursorMove(xpos, ypos, width, height);
    // Arcball* arcball = static_cast<Arcball*>(glfwGetWindowUserPointer(window));
    //std::cout << "x = " << xpos << "y = " << ypos << std::endl;
    if ( g_bCamRoting )
    {
        if (g_bfirstMouse) {
            g_lastX = (float)xpos;
            g_lastY = (float)ypos;
            g_bfirstMouse = false;
            return;

        }
        float deltaX = ((float)xpos - g_lastX);
        float deltaY = ((float)ypos - g_lastY);
        g_lastX = (float)xpos;
        g_lastY = (float)ypos;

        glm::vec3 originalEye = CCamera::getInstance().getViewLocation();
        glm::vec3 originalCenter = CCamera::getInstance().getCenterLocation();

        CCamera::getInstance().processMouseMovement(deltaX, deltaY, g_mouseSens);

        // 檢查新位置是否在房間內
        glm::vec3 newEye = CCamera::getInstance().getViewLocation();
        glm::vec3 newCenter = CCamera::getInstance().getCenterLocation();

        if (!isInsideRoom(newEye, g_roomBounds)) {
            // 如果新位置超出邊界，限制在房間內
            glm::vec3 clampedEye = clampToRoom(newEye, g_roomBounds);
            glm::vec3 offset = clampedEye - newEye;
            glm::vec3 clampedCenter = newCenter + offset;
            CCamera::getInstance().updateViewCenter(clampedEye, clampedCenter);
        }

        // 更新 view matrix
        glm::mat4 mxView = CCamera::getInstance().getViewMatrix();
        GLint viewLoc = glGetUniformLocation(g_shadingProg, "mxView");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mxView));
        g_eyeloc = CCamera::getInstance().getViewLocation();
    }
}
// ---------------------------------------------------------------------------------------
// 當視窗大小改變的 callback function
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// ---------------------------------------------------------------------------------------
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

    CCamera::getInstance().updateRadius((float)yoffset * -0.2f);
    glm::mat4 mxView = CCamera::getInstance().getViewMatrix();
    GLint viewLoc = glGetUniformLocation(g_shadingProg, "mxView"); 	// 取得 mxView 變數的位置
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mxView));
    g_eyeloc = CCamera::getInstance().getViewLocation();

}
     
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    glm::vec3 vPos;
    glm::mat4 mxView, mxProj;
    GLint viewLoc, projLoc;
    float shin;

    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }
            break;
        case GLFW_KEY_SPACE:
            break;

        case 262:   // 向右鍵 (右移)
        case 263:   // 向左鍵 (左移)
        case 264:   // 向下鍵 (後退)
        case 265:   // 向上鍵 (前進)
        {
            // 取得目前的鏡頭位置與朝向
            glm::vec3 eye = CCamera::getInstance().getViewLocation();
            glm::vec3 center = CCamera::getInstance().getCenterLocation(); // 你需要新增這個 getter
            glm::vec3 up(0.0f, 1.0f, 0.0f); // 世界的上方向（Y 軸）

            // 鏡頭方向向量與 XZ 平面方向
            glm::vec3 front = glm::normalize(center - eye);
            glm::vec3 forward = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
            glm::vec3 right = glm::normalize(glm::cross(forward, up));

            float speed = 0.1f;
            glm::vec3 newEye = eye;
            glm::vec3 newCenter = center;

            // 根據鍵盤輸入移動鏡頭
            if (key == 265) {        // Up: 向前
                newEye += forward * speed;
                newCenter += forward * speed;
            }
            else if (key == 264) {   // Down: 向後
                newEye -= forward * speed;
                newCenter -= forward * speed;
            }
            else if (key == 263) {   // Left: 向左
                newEye -= right * speed;
                newCenter -= right * speed;
            }
            else if (key == 262) {   // Right: 向右
                newEye += right * speed;
                newCenter += right * speed;
            }

            if (isInsideRoom(newEye, g_roomBounds)) {
                // 新位置在房間內，可以移動
                CCamera::getInstance().updateViewCenter(newEye, newCenter);
            }
            else {
                // 新位置超出邊界，限制在房間內
                glm::vec3 clampedEye = clampToRoom(newEye, g_roomBounds);
                glm::vec3 offset = clampedEye - newEye;
                glm::vec3 clampedCenter = newCenter + offset;
                CCamera::getInstance().updateViewCenter(clampedEye, clampedCenter);
            }

            glm::mat4 mxView = CCamera::getInstance().getViewMatrix();
            GLint viewLoc = glGetUniformLocation(g_shadingProg, "mxView");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mxView));
            break;
        }



        default: // 針對英文字母大小寫進行處理
            if (action == GLFW_RELEASE) {
                // 檢查 Shift 鍵(左右兩邊各一個)是否被按下
                bool isShiftPressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ||
                                      (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
                // 判斷字母鍵的大小寫，預設使用鍵盤是沒有按下 CAPS 鍵
                if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
                    char letter = (isShiftPressed) ? ('A' + (key - GLFW_KEY_A)) : ('a' + (key - GLFW_KEY_A));
                    //std::cout << "key = " << letter << std::endl;
                    switch (letter) {
                        case 'R':
                        case 'r':
                            g_light.setDiffuse(glm::vec4(1.0f, 0.25f, 0.1f, 1.0f),6);
                            break;
                        case 'G':
                        case 'g':
                            g_light.setDiffuse(glm::vec4(0.1f, 0.75f, 0.75f, 1.0f),4);
                            break;
                        case 'B':
                        case 'b':
                            g_light.setDiffuse(glm::vec4(0.1f, 0.25f, 1.0f, 1.0f),1);
                            break;
                        case 'N':
                        case 'n':
                            g_light.setDiffuse(glm::vec4(0.8f, 0.6f, 0.4f, 1.0f), 2);
                            break;

                        case 'L':
                        case 'l':
                            g_light.setMotionEnabled();
                            break;
                        case 'D':
                        case 'd':
                            g_door->setMotionEnabled();
                            break;
                    }
                }   
            }
        
    }


}