#pragma once
// CLight.h
#include "../models/CSphere.h"
#include <glm/glm.hpp>
#include <glew/include/GL/glew.h>
#include <string>
#include <vector>


class CLight {
public:
    enum LightType { POINT = 0, SPOT = 1 };
    // 建構子：建立點光源：預設位置 (0,0,0)、環境/漫射/鏡面顏色，以及常用衰減值
    CLight(glm::vec3 position, glm::vec4 ambient , glm::vec4 diffuse,
           glm::vec4 specular, float constant, float linear, float quadratic
    );  // constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f

    CLight(
        glm::vec3 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
        float constant, float linear, float quadratic, bool withMotion
    );

    // 目標式聚光燈建構元
    CLight(glm::vec3 position, glm::vec3 target, float innerCutOffDeg, float outerCutOffDeg, float exponent,
           glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
           float constant, float linear, float quadratic
    );
    ~CLight();

    // Setter / Getter
    void setPos(glm::vec3 pos);
    glm::vec3 getPos();

    void setAmbient(glm::vec4 amb);
    glm::vec4 getAmbient();

    void setDiffuse(glm::vec4 diff,int pattern);
    glm::vec4 getDiffuse();

    void setSpecular(glm::vec4 spec);
    glm::vec4 getSpecular();

    void setAttenuation(float c, float l, float q);
    void getAttenuation(float &c, float &l, float &q);

    void setIntensity(float intensity);

    void setLightOn(bool enable);
    bool isLightOn();

    void setTarget(const glm::vec3& target);
    void setCutOffDeg(float innerDeg, float outerDeg, float exponent = 1.0f);
    glm::vec3 getTarget(); 
    glm::vec3 getDirection();

    // 將所有參數上傳到指定 shader program 的 uniform struct
    // uniformName: GLSL 中對應的 PointLight 變數名稱，例如 "pointLight"  
    void setShaderID(GLuint shaderProg, std::string uniformName, bool displayon=true);
    void updateToShader();
    void update(float dt);
    void setMotionEnabled(); // 設定為開關機制，
    void updateMotion(float dt);

    // 描繪代表 light 的模型
    void draw();
    void drawRaw();

    bool _enabled = true; // 是否啟用開關
    int getPrevTarget() { return _prevTarget; }

private:
    std::string _lightname;
    GLuint _shaderID;
    glm::vec3 _position;
    glm::vec3 _posStart;
    glm::vec4 _ambient;
    glm::vec4 _diffuse;
    glm::vec4 _specular;
    float     _constant, _linear, _quadratic;
    float     _intensity;

    // Spot Light
    glm::vec3 _direction;
    glm::vec3 _target;
    float     _innerCutOff;   // cos(innerAngle)
    float     _outerCutOff;   // cos(outerAngle)
    float     _exponent;      // cos 的指數
    LightType _type;
    bool      _lighingOn;

    // 代表 light 的模型，可自行更換
    bool  _displayOn;  // 是否顯示代表光源的模型


    // 動態控制相關變數
    float _clock;    // 動態控制的 clock
    bool  _motionOn; // 光源動態的控制

    CSphere _lightObj;
    glm::vec3 _dir;
    std::vector<glm::vec3> _path;
    int _prevTarget = 0;
    int _currTarget = 1;
    float _moveSpeed = 8.0f;
    
};
