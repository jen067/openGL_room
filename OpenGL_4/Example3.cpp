//#define GLM_ENABLE_EXPERIMENTAL 1

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>

#include <glew/include/GL/glew.h>
#include <glfw/include/GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common/initshader.h"
#include "common/arcball.h"
#include "common/wmhandler.h"
#include "common/CCamera.h"
#include "common/CShaderPool.h"
#include "models/CQuad.h"
#include "models/CBottle.h"
#include "models/CTeapot.h"
#include "models/CTorusKnot.h"
#include "models/CBox.h"
#include "models/CSphere.h"
#include "models/CPlane.h"

#include "common/CLight.h"
#include "common/CMaterial.h"
#include "models/CObjModel.h"
#include "common/TextureLoader.h"
#include "common/CButton.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 800 
#define ROW_NUM 24

CBox g_house;
CPlane g_plane;


glm::vec3 g_eyeloc(8.0f, 8.0f, 8.0f); // 鏡頭位置
CSphere g_centerloc;
CQuad g_floor[ROW_NUM][ROW_NUM]; 
CQuad g_wallF[ROW_NUM][ROW_NUM];

GLuint g_shadingProg;

// 全域光源
CLight g_light(
    glm::vec3(8.0f, 8.0f, -8.0f),
    glm::vec4(0.25f, 0.20f, 0.15f, 1.0f),
    glm::vec4(0.8f, 0.6f, 0.4f, 1.0f),
    glm::vec4(0.5f, 0.4f, 0.3f, 1.0f),
    1.0f, 0.03f, 0.04f
);

// 左側聚光燈
CLight g_spotLightL(
    glm::vec3(-12.0f, 16.0f, 12.0f),
    glm::vec3(-10.0f, 0.0f, 5.0f),       
    10.0f, 60.0f, 10.0f,
    glm::vec4(0.8f, 0.6f, 0.3f, 1.0f),     
    glm::vec4(1.0f, 0.75f, 0.25f, 1.0f),
    glm::vec4(1.0f, 0.9f, 0.6f, 1.0f),
    1.0f, 0.0097f, 0.01f
);

// 背面聚光燈
CLight g_spotLightB(
    glm::vec3(0.0f, 16.0f, -12.0f),
    glm::vec3(0.0f, 0.0f, -12.0f),
    10.0f, 55.0f, 10.0f,
    glm::vec4(0.8f, 0.6f, 0.3f, 1.0f),
    glm::vec4(1.0f, 0.75f, 0.25f, 1.0f),
    glm::vec4(1.0f, 0.9f, 0.6f, 1.0f),
    1.0f, 0.0097f, 0.01f
);

// 後側聚光燈
CLight g_spotLightR(
    glm::vec3(12.0f, 16.0f, 12.0f), 
    glm::vec3(10.0f, 0.0f, 6.0f), 
    10.0f, 55.0f, 10.0f,
    glm::vec4(0.8f, 0.6f, 0.3f, 1.0f),
    glm::vec4(1.0f, 0.75f, 0.25f, 1.0f),
    glm::vec4(1.0f, 0.9f, 0.6f, 1.0f),
    1.0f, 0.0097f, 0.01f
);

// 檯燈光源
CLight g_Lamp(
    glm::vec3(0.9f, 7.5f, -1.45f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    10.0f, 45.0f, 1.0f,
    glm::vec4(0.6f, 0.5f, 0.2f, 1.0f),
    glm::vec4(1.0f, 0.65f, 0.1f, 1.0f),
    glm::vec4(1.0f, 0.75f, 0.3f, 1.0f),
    1.0f, 0.0095f, 0.01f
);

CLight g_spotLightWall(
    glm::vec3(-3.15f, 9.3f, 11.0f),
    glm::vec4(0.15f, 0.10f, 0.05f, 1.0f), // 偏冷白
    glm::vec4(0.9f, 0.6f, 0.3f, 1.0f), // 溫暖日光色
    glm::vec4(0.5f, 0.4f, 0.3f, 1.0f),   // 柔和高光
    1.0f, 0.03f, 0.04f,
    false
);

// 物件
CObjModel* g_wallLamp = nullptr;
CObjModel* g_case = nullptr;
CObjModel* g_chairU = nullptr;
CObjModel* g_chairB = nullptr;
CObjModel* g_table = nullptr;
CObjModel* g_lamp = nullptr;
CObjModel* g_drawer = nullptr;
CObjModel* g_lampH = nullptr;
CObjModel* g_lampO = nullptr;
CObjModel* g_latter = nullptr;
CObjModel* g_vase = nullptr;
CObjModel* g_grass = nullptr;
CObjModel* g_wShelf = nullptr;
CObjModel* g_door = nullptr;
CObjModel* g_doorFrame = nullptr;
CObjModel* g_book = nullptr;
CObjModel* g_bookSet = nullptr;
CObjModel* g_bookSet1 = nullptr;
CObjModel* g_cdPlayer = nullptr;
CObjModel* g_cd = nullptr;
CObjModel* g_leaf = nullptr;
CObjModel* g_leafVase = nullptr;
CObjModel* g_ghost = nullptr;
CObjModel* g_ghostEye = nullptr;


// 材質球
CMaterial g_matOliveWood; // 橄欖綠
CMaterial g_matLatte; // 奶茶
CMaterial g_matCaramelRed; // 珊瑚紅
CMaterial g_matAlmond; // 杏色
CMaterial g_matKhakiGold; // 金色
CMaterial g_matGrassOlive; // 草
CMaterial g_matWarmWalnut; // 胡桃
CMaterial g_matLampCoverWhite; // 白
CMaterial g_matDarkMetal; // 金屬黑
CMaterial g_matOliveWoodDeep; // 深綠
CMaterial g_matSteelBlue; // 深藍
CMaterial g_brownMaterial; // 咖啡
CMaterial g_ghostWhite; // 白

// 貼圖材質
TextureLoader textureF;
TextureLoader textureW;

GLuint g_uiShader;
GLint g_2dViewLoc, g_2dProjLoc;

// 光源切換按鈕
std::array<CButton, 5> g_button = {
    CButton(50.0f, 50.0f, glm::vec4(1.0f, 0.4f, 0.72f, 1.0f), glm::vec4(0.72f, 0.2f, 0.38f, 1.0f)),
    CButton(50.0f, 50.0f, glm::vec4(0.71f, 0.9f, 0.99f, 1.0f), glm::vec4(0.45f, 0.52f, 0.95f, 0.67f)),
    CButton(50.0f, 50.0f, glm::vec4(0.71f, 0.9f, 0.99f, 1.0f), glm::vec4(0.45f, 0.52f, 0.95f, 0.67f)),
    CButton(50.0f, 50.0f, glm::vec4(0.71f, 0.9f, 0.99f, 1.0f), glm::vec4(0.45f, 0.52f, 0.95f, 0.67f)),
    CButton(50.0f, 50.0f, glm::vec4(0.25f, 0.7f, 0.7f, 1.0f), glm::vec4(0.25f, 0.75f, 0.95f, 0.67f))
};


glm::mat4 g_2dmxView = glm::mat4(1.0f);
glm::mat4 g_2dmxProj = glm::mat4(1.0f);

glm::vec4 floorColor1(0.88f, 0.84f, 0.76f, 1.0f);
glm::vec4 floorColor2(0.75f, 0.78f, 0.80f, 1.0f);
glm::vec4 floorColor3(0.82f, 0.75f, 0.65f, 1.0f);

//----------------------------------------------------------------------------
void genMaterial();

void loadScene(void)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    genMaterial();  // 建立材質
    g_shadingProg = CShaderPool::getInstance().getShader("v_phong.glsl", "f_phong.glsl");
    g_uiShader = CShaderPool::getInstance().getShader("ui_vtxshader.glsl", "ui_fragshader.glsl");

    // 設定光源
    g_light.setShaderID(g_shadingProg, "uLight",true);
    g_spotLightWall.setShaderID(g_shadingProg, "uLight1", true);
    g_spotLightL.setShaderID(g_shadingProg, "spotLights[0]", false);
    g_spotLightB.setShaderID(g_shadingProg, "spotLights[1]", false);
    g_spotLightR.setShaderID(g_shadingProg, "spotLights[2]", false);
    g_Lamp.setShaderID(g_shadingProg, "spotLights[3]", true);
    
    // 設定地板與貼圖
    int k = 0;
    GLuint texFloor = textureF.loadTexture("common/texture/wood.png");
    for (int i = 0; i < ROW_NUM; i++)
    {
        k++;
        for (int j = 0; j < ROW_NUM; j++) {
            g_floor[i][j].setupVertexAttributes();
            g_floor[i][j].setShaderID(g_shadingProg, 5);
            
            g_floor[i][j].setTextureID(texFloor);
            g_floor[i][j].setPos(glm::vec3((ROW_NUM / 2) - 0.5f - (float)i, 1.12f, (float)j - (ROW_NUM / 2) + 0.5f));
            g_floor[i][j].setRotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            g_floor[i][j].setScale(glm::vec3(1.0f,1.0f,4.0f));
            k++;
        }
    }

    
    // 建立房間
    g_house.setupVertexAttributes();
    g_house.setShaderID(g_shadingProg, 5);
    GLuint texWood = textureW.loadTexture("common/texture/flower.png");
    g_house.setTextureID(texWood);
    g_house.setScale(glm::vec3(30.0f, 12.0f, 30.0f));
    g_house.setPos(glm::vec3(0.0f, 8.3f, 0.0f));

    // 建立天花板
    g_plane.setupVertexAttributes();
    g_plane.setShaderID(g_shadingProg, 5);
    GLuint texblanket = textureF.loadTexture("common/texture/blanket.png");
    g_plane.setTextureID(texblanket);
    g_plane.setScale(glm::vec3(10.0f));
    g_plane.setPos(glm::vec3(0.0f, 7.2f, 0.0f));
    
    //物件相關貼圖與材質設定
    g_case = new CObjModel("asset/case.obj"); 
    g_case->setShaderID(g_shadingProg, 3);
    g_case->setMaterial(g_matOliveWood);
    g_case->setPos(glm::vec3(22.7f, 3.6f, -25.0f));
    g_case->setRotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    g_case->setScale(glm::vec3(4.5f));

    g_chairB = new CObjModel("asset/chairB.obj"); 
    g_chairB->setShaderID(g_shadingProg, 3);
    g_chairB->setMaterial(g_matSteelBlue); 
    g_chairB->setPos(glm::vec3(18.0f, 1.1f, -9.6f));
    g_chairB->setScale(glm::vec3(3.5));

    g_chairU = new CObjModel("asset/chairU.obj");
    g_chairU->setShaderID(g_shadingProg, 3);
    g_chairU->setMaterial(g_matOliveWood);
    g_chairU->setPos(glm::vec3(18.0f, 1.1f, -9.6f)); 
    g_chairU->setScale(glm::vec3(3.5f));

    g_table = new CObjModel("asset/table.obj"); 
    g_table->setShaderID(g_shadingProg, 3);
    g_table->setMaterial(g_matLampCoverWhite); 
    g_table->setPos(glm::vec3(0.8f, 1.12f, -1.6f)); 
    g_table->setScale(glm::vec3(3.5f));

    g_lamp = new CObjModel("asset/lamp.obj");
    g_lamp->setShaderID(g_shadingProg, 3);
    g_lamp->setMaterial(g_matSteelBlue); 
    g_lamp->setPos(glm::vec3(-3.2f, 1.15f, -2.8f)); 
    g_lamp->setScale(glm::vec3(3.5f));
    
    g_drawer = new CObjModel("asset/drawer.obj"); 
    g_drawer->setShaderID(g_shadingProg, 3);
    g_drawer->setMaterial(g_matWarmWalnut);
    g_drawer->setPos(glm::vec3(25.8f, 1.05f, 1.6f)); 
    g_drawer->setScale(glm::vec3(4.0f));

    g_lampH = new CObjModel("asset/lampH.obj");
    g_lampH->setShaderID(g_shadingProg, 3);
    g_lampH->setMaterial(g_matKhakiGold);
    g_lampH->setPos(glm::vec3(1.0f, -0.82f, -1.2f)); 
    g_lampH->setScale(glm::vec3(5.5f));

    g_lampO = new CObjModel("asset/lampO.obj");
    g_lampO->setShaderID(g_shadingProg, 3);
    g_lampO->setMaterial(g_matLampCoverWhite);
    g_lampO->setPos(glm::vec3(1.0f, -0.82f, -1.2f));
    g_lampO->setScale(glm::vec3(5.5f));

    g_wallLamp = new CObjModel("asset/wallLight.obj");
    g_wallLamp->setShaderID(g_shadingProg, 3);
    g_wallLamp->setMaterial(g_matSteelBlue);
    g_wallLamp->setPos(glm::vec3(-3.0f, 8.5f, 11.2f));
    g_wallLamp->setScale(glm::vec3(4.0f));

    g_latter = new CObjModel("asset/multiCabinet.obj"); 
    g_latter->setShaderID(g_shadingProg, 3);
    g_latter->setMaterial(g_matLampCoverWhite);
    g_latter->setPos(glm::vec3(22.0f, 1.05f, -5.0f)); 
    g_latter->setScale(glm::vec3(4.0f));

    g_vase = new CObjModel("asset/vase.obj"); 
    g_vase->setShaderID(g_shadingProg, 3);
    g_vase->setMaterial(g_matCaramelRed);
    g_vase->setPos(glm::vec3(-3.5f, 1.0f, -5.5f)); 
    g_vase->setScale(glm::vec3(4.0f));

    g_grass = new CObjModel("asset/grass.obj"); 
    g_grass->setShaderID(g_shadingProg, 3);
    g_grass->setMaterial(g_matGrassOlive);
    g_grass->setPos(glm::vec3(-3.5f, 1.0f, -5.5f)); 
    g_grass->setScale(glm::vec3(4.0f));

    g_wShelf = new CObjModel("asset/wallShelf.obj"); 
    g_wShelf->setShaderID(g_shadingProg, 3);
    g_wShelf->setMaterial(g_matAlmond);
    g_wShelf->setPos(glm::vec3(4.6f, 1.26f, 0.0f)); 
    g_wShelf->setScale(glm::vec3(4.0f));
    
    g_door = new CObjModel("asset/door.obj"); 
    g_door->setShaderID(g_shadingProg, 3);
    g_door->setMaterial(g_matWarmWalnut);
    g_door->setPos(glm::vec3(2.0f, 1.1f, 12.0f)); 
    g_door->setScale(glm::vec3(4.0f));

    g_doorFrame = new CObjModel("asset/doorFrame.obj"); 
    g_doorFrame->setShaderID(g_shadingProg, 3);
    g_doorFrame->setMaterial(g_matWarmWalnut);
    g_doorFrame->setPos(glm::vec3(2.0f, 1.1f, 12.0f)); 
    g_doorFrame->setScale(glm::vec3(4.0f));

    g_book = new CObjModel("asset/book.obj"); 
    g_book->setShaderID(g_shadingProg, 3);
    g_book->setMaterial(g_matKhakiGold);
    g_book->setPos(glm::vec3(2.2f, -0.06f, -2.2f)); 
    g_book->setScale(glm::vec3(4.0f));

    g_bookSet = new CObjModel("asset/bookset.obj"); 
    g_bookSet->setShaderID(g_shadingProg, 3);
    g_bookSet->setMaterial(g_matAlmond);
    g_bookSet->setPos(glm::vec3(23.5f, 1.05f, -4.5f)); 
    g_bookSet->setScale(glm::vec3(4.3f));
    
    g_bookSet1 = new CObjModel("asset/bookset.obj"); 
    g_bookSet1->setShaderID(g_shadingProg, 3);
    g_bookSet1->setMaterial(g_matCaramelRed);
    g_bookSet1->setPos(glm::vec3(22.0f, 7.4f, -4.5f)); 
    g_bookSet1->setScale(glm::vec3(4.3f));

    g_cdPlayer = new CObjModel("asset/cdplayer.obj"); 
    g_cdPlayer->setShaderID(g_shadingProg, 3);
    g_cdPlayer->setMaterial(g_brownMaterial);
    g_cdPlayer->setPos(glm::vec3(-5.0f, -1.16f, -0.5f)); 
    g_cdPlayer->setScale(glm::vec3(3.5f));
    
    g_cd = new CObjModel("asset/cd.obj"); 
    g_cd->setShaderID(g_shadingProg, 3);
    g_cd->setMaterial(g_matDarkMetal);
    g_cd->setPos(glm::vec3(-5.0f, -1.16f, -0.5f)); 
    g_cd->setScale(glm::vec3(3.5f));
    
    g_leaf = new CObjModel("asset/leaf.obj"); 
    g_leaf->setShaderID(g_shadingProg, 3);
    g_leaf->setMaterial(g_matOliveWoodDeep);
    g_leaf->setPos(glm::vec3(-9.8f, 2.3f, 2.0f)); 
    g_leaf->setScale(glm::vec3(4.3f));
    
    g_leafVase = new CObjModel("asset/leafVase.obj"); 
    g_leafVase->setShaderID(g_shadingProg, 3);
    g_leafVase->setMaterial(g_matKhakiGold);
    g_leafVase->setPos(glm::vec3(-10.0f, 2.0f, 2.0f)); 
    g_leafVase->setScale(glm::vec3(4.3f));

    g_ghost = new CObjModel("asset/ghost.obj"); 
    g_ghost->setShaderID(g_shadingProg, 3);
    g_ghost->setMaterial(g_ghostWhite);
    g_ghost->setPos(glm::vec3(g_light.getPos() + glm::vec3(0.0f, 0.7f, -0.7f))); 
    g_ghost->setScale(glm::vec3(4.5f));    

    g_ghostEye = new CObjModel("asset/ghostEye.obj"); 
    g_ghostEye->setShaderID(g_shadingProg, 3);
    g_ghostEye->setMaterial(g_matSteelBlue);
    g_ghost->setPos(glm::vec3(g_light.getPos() + glm::vec3(0.0f, 0.7f, -0.7f))); 
    g_ghostEye->setScale(glm::vec3(4.5f));    


    
	CCamera::getInstance().updateView(g_eyeloc); // 設定 eye 位置
    CCamera::getInstance().updateCenter(glm::vec3(0,4,0));
	CCamera::getInstance().updatePerspective(45.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 100.0f);
    glm::mat4 mxView = CCamera::getInstance().getViewMatrix();
	glm::mat4 mxProj = CCamera::getInstance().getProjectionMatrix();


    GLint viewLoc = glGetUniformLocation(g_shadingProg, "mxView"); 	// 取得 view matrix 變數的位置
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mxView));

    GLint projLoc = glGetUniformLocation(g_shadingProg, "mxProj"); 	// 取得投影矩陣變數的位置
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(mxProj));

    // 產生  UI 所需的相關資源
    g_button[0].init(g_uiShader, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
    g_button[0].setScreenPos(450.0f, 80.0f);

    g_button[1].init(g_uiShader, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
    g_button[1].setScreenPos(550.0f, 80.0f);

    g_button[2].init(g_uiShader, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
    g_button[2].setScreenPos(650.0f, 80.0f);

    g_button[3].init(g_uiShader, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
    g_button[3].setScreenPos(750.0f, 80.0f);

    g_button[4].init(g_uiShader, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
    g_button[4].setScreenPos(750.0f, 160.0f);



    g_2dViewLoc = glGetUniformLocation(g_uiShader, "mxView"); 	// 取得 MVP 變數的位置
    glUniformMatrix4fv(g_2dViewLoc, 1, GL_FALSE, glm::value_ptr(g_2dmxView));

    g_2dProjLoc = glGetUniformLocation(g_uiShader, "mxProj"); 	// 取得 MVP 變數的位置
    g_2dmxProj = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT, -1.0f, 1.0f);
    glUniformMatrix4fv(g_2dProjLoc, 1, GL_FALSE, glm::value_ptr(g_2dmxProj));

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 設定清除 back buffer 背景的顏色
    glEnable(GL_DEPTH_TEST); // 啟動深度測試
    
}
//----------------------------------------------------------------------------


void render(void)
{
    // 元
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 設定 back buffer 的背景顏色
    glUseProgram(g_shadingProg);
    glUniform3fv(glGetUniformLocation(g_shadingProg, "viewPos"), 1, glm::value_ptr(g_eyeloc));


    // 家
    glm::mat4 mxView = CCamera::getInstance().getViewMatrix();
    glm::mat4 mxProj = CCamera::getInstance().getProjectionMatrix();


    GLint viewLoc = glGetUniformLocation(g_shadingProg, "mxView"); 	// 取得 mxView 變數的位置
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(mxView));
    GLint projLoc = glGetUniformLocation(g_shadingProg, "mxProj"); 	// 取得 mxProj 變數的位置
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(mxProj));

       for (int i = 0; i < ROW_NUM; i++)
        for (int j = 0; j < ROW_NUM; j++) {
            g_floor[i][j].uploadMaterial();
            g_floor[i][j].drawRaw();
        }
    
    g_light.updateToShader();
    g_spotLightWall.updateToShader();
    g_spotLightL.updateToShader();
    g_spotLightB.updateToShader();
    g_spotLightR.updateToShader();
    g_Lamp.updateToShader();

    g_light.drawRaw();
    g_spotLightWall.drawRaw();
    g_spotLightL.drawRaw();
    g_spotLightB.drawRaw();
    g_spotLightR.drawRaw();
    g_Lamp.drawRaw();
    

    g_house.uploadMaterial();
    g_house.drawRaw();

    g_plane.uploadMaterial();
    g_plane.drawRaw();

    if (g_case) {
        g_case->uploadMaterial();
        g_case->drawRaw();
    }
    if (g_chairB) {
        g_chairB->uploadMaterial();
        g_chairB->drawRaw();
    }
    if (g_chairU) {
        g_chairU->uploadMaterial();
        g_chairU->drawRaw();
    }

    if (g_table) {
        g_table->uploadMaterial();
        g_table->drawRaw();
    }

    if (g_lamp) {
        g_lamp->uploadMaterial();
        g_lamp->drawRaw();
    }

    if (g_drawer) {
        g_drawer->uploadMaterial();
        g_drawer->drawRaw();
    }

    if (g_lampH) {
        g_lampH->uploadMaterial();
        g_lampH->drawRaw();
    }

    if (g_lampO) {
        g_lampO->uploadMaterial();
        g_lampO->drawRaw();
    }

    if (g_wallLamp) {
        g_wallLamp->uploadMaterial();
        g_wallLamp->drawRaw();
    }
    


    if (g_latter) {
        g_latter->uploadMaterial();
        g_latter->drawRaw();
    }

    if (g_vase) {
        g_vase->uploadMaterial();
        g_vase->drawRaw();
    }

    if (g_grass) {
        g_grass->uploadMaterial();
        g_grass->drawRaw();
    }

    if (g_wShelf) {
        g_wShelf->uploadMaterial();
        g_wShelf->drawRaw();
    }

    if (g_door) {
        g_door->uploadMaterial();
        g_door->drawRaw();
    }

    if (g_doorFrame) {
        g_doorFrame->uploadMaterial();
        g_doorFrame->drawRaw();
    }

    if (g_book) {
        g_book->uploadMaterial();
        g_book->drawRaw();
    }

    if (g_bookSet) {
        g_bookSet->uploadMaterial();
        g_bookSet->drawRaw();
    }
    
    if (g_bookSet1) {
        g_bookSet1->uploadMaterial();
        g_bookSet1->drawRaw();
    }
    
    if (g_cdPlayer) {
        g_cdPlayer->uploadMaterial();
        g_cdPlayer->drawRaw();
    }
    
    if (g_cd) {
        g_cd->uploadMaterial();
        g_cd->drawRaw();
    }

    if (g_leaf) {
        g_leaf->uploadMaterial();
        g_leaf->drawRaw();
    }

    if (g_leafVase) {
        g_leafVase->uploadMaterial();
        g_leafVase->drawRaw();
    }
    
    if (g_ghost) {
        g_ghost->uploadMaterial();
        g_ghost->drawRaw();
    }

    if (g_ghostEye) {
        g_ghostEye->uploadMaterial();
        g_ghostEye->drawRaw();
    }

    

    glUseProgram(g_uiShader); // 使用 shader program
    glUniformMatrix4fv(g_2dViewLoc, 1, GL_FALSE, glm::value_ptr(g_2dmxView));
    glUniformMatrix4fv(g_2dProjLoc, 1, GL_FALSE, glm::value_ptr(g_2dmxProj));
    g_button[0].draw();
    g_button[1].draw();
    g_button[2].draw();
    g_button[3].draw();
    g_button[4].draw();

}
//----------------------------------------------------------------------------

void update(float dt)
{
    g_light.update(dt);

    if (g_light.getPrevTarget() == 1) {
        g_ghostEye->setPos(glm::vec3(g_light.getPos() + glm::vec3(0.7f, 0.7f, 0.0f)));
        g_ghost->setPos(glm::vec3(g_light.getPos() + glm::vec3(0.7f, 0.7f, 0.0f)));
        g_ghost->setTargetRotation(-90);
        g_ghostEye->setTargetRotation(-90);
    }
    else if (g_light.getPrevTarget() == 2) {
        g_ghostEye->setPos(glm::vec3(g_light.getPos() + glm::vec3(0.0f, 0.7f, 0.7f)));
        g_ghost->setPos(glm::vec3(g_light.getPos() + glm::vec3(0.0f, 0.7f, 0.7f)));

        g_ghost->setTargetRotation(-180);
        g_ghostEye->setTargetRotation(-180);
    }
    else if (g_light.getPrevTarget() == 3) {
        g_ghostEye->setPos(glm::vec3(g_light.getPos() + glm::vec3(-0.7f, 0.7f, 0.0f)));
        g_ghost->setPos(glm::vec3(g_light.getPos() + glm::vec3(-0.7f, 0.7f, 0.0f)));
        g_ghost->setRotate(-270, glm::vec3(0.0f, 1.0f, 0.0f));
        g_ghostEye->setRotate(-270, glm::vec3(0.0f, 1.0f, 0.0f));

        g_ghost->setTargetRotation(-270);
        g_ghostEye->setTargetRotation(-270);

    }
    else {
        g_ghostEye->setPos(glm::vec3(g_light.getPos() + glm::vec3(0.0f, 0.7f, -0.7f)));
        g_ghost->setPos(glm::vec3(g_light.getPos() + glm::vec3(0.0f, 0.7f, -0.7f)));
        g_ghost->setRotate(-360, glm::vec3(0.0f, 1.0f, 0.0f));
        
        g_ghost->setTargetRotation(-360); // = 0度
        g_ghostEye->setTargetRotation(-360);
    }
    g_spotLightWall.update(dt);

    g_door->update(dt);

    g_ghost->updateRotation();
    g_ghostEye->updateRotation();

    g_spotLightL.update(dt);
    g_spotLightB.update(dt);
    g_spotLightR.update(dt);
    g_Lamp.update(dt);
}

void releaseAll()
{

}

int main() {
    // ------- 檢查與建立視窗  ---------------  
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL_4 Example 3 Per Pixel Lighting", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    // ---------------------------------------


    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);// 視窗大小
    glfwSetKeyCallback(window, keyCallback);                        // 鍵盤按鍵按下
    glfwSetMouseButtonCallback(window, mouseButtonCallback);        // 滑鼠按下
    glfwSetCursorPosCallback(window, cursorPosCallback);            // 滑鼠移動
	
    loadScene();

    float lastTime = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastTime; // 計算前一個 frame 到目前為止經過的時間
        lastTime = currentTime;
        update(deltaTime);
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    releaseAll(); // 釋放資源
    glfwTerminate();
    return 0;
}

void genMaterial()
{
    g_matOliveWood.setAmbient(glm::vec4(0.180f, 0.205f, 0.170f, 1.0f));
    g_matOliveWood.setDiffuse(glm::vec4(0.48f, 0.62f, 0.49f, 1.0f));
    g_matOliveWood.setSpecular(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    g_matOliveWood.setShininess(16.0f);

    g_matLatte.setAmbient(glm::vec4(0.250f, 0.210f, 0.140f, 1.0f));
    g_matLatte.setDiffuse(glm::vec4(0.93f, 0.80f, 0.55f, 1.0f));
    g_matLatte.setSpecular(glm::vec4(0.35f, 0.35f, 0.30f, 1.0f));
    g_matLatte.setShininess(20.0f);

    g_matCaramelRed.setAmbient(glm::vec4(0.220f, 0.130f, 0.110f, 1.0f));
    g_matCaramelRed.setDiffuse(glm::vec4(0.80f, 0.45f, 0.35f, 1.0f));
    g_matCaramelRed.setSpecular(glm::vec4(0.30f, 0.25f, 0.25f, 1.0f));
    g_matCaramelRed.setShininess(18.0f);

    g_matAlmond.setAmbient(glm::vec4(0.270f, 0.230f, 0.160f, 1.0f));
    g_matAlmond.setDiffuse(glm::vec4(0.96f, 0.87f, 0.68f, 1.0f));
    g_matAlmond.setSpecular(glm::vec4(0.30f, 0.28f, 0.25f, 1.0f));
    g_matAlmond.setShininess(12.0f);

    g_matKhakiGold.setAmbient(glm::vec4(0.220f, 0.200f, 0.130f, 1.0f));
    g_matKhakiGold.setDiffuse(glm::vec4(0.92f, 0.80f, 0.48f, 1.0f));
    g_matKhakiGold.setSpecular(glm::vec4(0.38f, 0.35f, 0.30f, 1.0f));
    g_matKhakiGold.setShininess(28.0f);

    g_matGrassOlive.setAmbient(glm::vec4(0.160f, 0.180f, 0.130f, 1.0f));
    g_matGrassOlive.setDiffuse(glm::vec4(0.54f, 0.64f, 0.42f, 1.0f));
    g_matGrassOlive.setSpecular(glm::vec4(0.20f, 0.22f, 0.18f, 1.0f));
    g_matGrassOlive.setShininess(10.0f);

    g_matWarmWalnut.setAmbient(glm::vec4(0.170f, 0.110f, 0.070f, 1.0f));
    g_matWarmWalnut.setDiffuse(glm::vec4(0.58f, 0.36f, 0.24f, 1.0f));
    g_matWarmWalnut.setSpecular(glm::vec4(0.25f, 0.22f, 0.20f, 1.0f));
    g_matWarmWalnut.setShininess(22.0f);

    g_matLampCoverWhite.setAmbient(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    g_matLampCoverWhite.setDiffuse(glm::vec4(0.92f, 0.92f, 0.90f, 1.0f));
    g_matLampCoverWhite.setSpecular(glm::vec4(0.30f, 0.30f, 0.30f, 1.0f));
    g_matLampCoverWhite.setShininess(18.0f);

    g_ghostWhite.setAmbient(glm::vec4(0.25f, 0.25f, 0.25f, 0.3f));
    g_ghostWhite.setDiffuse(glm::vec4(0.92f, 0.92f, 0.90f, 0.95f));
    g_ghostWhite.setSpecular(glm::vec4(0.3f, 0.3f, 0.3f, 0.3f));
    g_ghostWhite.setShininess(18.0f);
    

    g_matDarkMetal.setAmbient(glm::vec4(0.08f, 0.08f, 0.08f, 1.0f));
    g_matDarkMetal.setDiffuse(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
    g_matDarkMetal.setSpecular(glm::vec4(0.50f, 0.48f, 0.42f, 1.0f));
    g_matDarkMetal.setShininess(64.0f);

    g_matOliveWoodDeep.setAmbient(glm::vec4(0.12f, 0.15f, 0.11f, 1.0f));
    g_matOliveWoodDeep.setDiffuse(glm::vec4(0.38f, 0.48f, 0.39f, 1.0f));
    g_matOliveWoodDeep.setSpecular(glm::vec4(0.22f, 0.22f, 0.22f, 1.0f));
    g_matOliveWoodDeep.setShininess(14.0f);

    g_matSteelBlue.setAmbient(glm::vec4(0.10f, 0.12f, 0.15f, 1.0f));
    g_matSteelBlue.setDiffuse(glm::vec4(0.30f, 0.38f, 0.45f, 1.0f));
    g_matSteelBlue.setSpecular(glm::vec4(0.45f, 0.47f, 0.50f, 1.0f));
    g_matSteelBlue.setShininess(40.0f);

    g_brownMaterial.setAmbient(glm::vec4(0.10f, 0.05f, 0.02f,1.0f));
    g_brownMaterial.setDiffuse(glm::vec4(0.40f, 0.25f, 0.10f, 1.0f));
    g_brownMaterial.setSpecular(glm::vec4(0.15f, 0.10f, 0.05f, 1.0f));
    g_brownMaterial.setShininess(32.0f);
    
}