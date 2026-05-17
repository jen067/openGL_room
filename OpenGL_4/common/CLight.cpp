// CLight.cpp

#define CHECK_UNIFORM(loc, name) \
    if ((loc) == -1) { std::cerr << "[警告] Uniform 無效: " << (name) << std::endl; }
#include "CLight.h"
#include <glm/gtc/type_ptr.hpp>
#include "typedefs.h"
#include <iostream>

CLight::CLight(
    glm::vec3 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
    float constant, float linear, float quadratic)
    : _lightObj(5.0f, 15, 15, 2) 
{
    _position = position;
    _ambient = ambient;  _diffuse = diffuse;  _specular = specular;
    _ambient.w = 1.0f; _diffuse.w = 1.0f; _specular.w = 1.0f;
    _constant = constant; _linear = linear; _quadratic = quadratic;
    _intensity = 1.0f;
    _type = LightType::POINT; _posStart = position;
    _direction = glm::vec3(1.0f); _target = glm::vec3(1.0f);
    _innerCutOff = 0.0f; _outerCutOff = 0.0f; _exponent = 1.0f;
    _displayOn = true; _motionOn = false; _clock = 0.0f;
    _lighingOn = true;

    _lightObj.setPos(position);  
    // 四個頂點，繞正方形（XZ 平面上）
    _path.push_back(glm::vec3(8.0f, _position.y, -8.0f)); // 0 右下角
    _path.push_back(glm::vec3(8.0f, _position.y, 8.0f)); // 1 右上角
    _path.push_back(glm::vec3(-8.0f, _position.y, 8.0f)); // 2 左上角
    _path.push_back(glm::vec3(-8.0f, _position.y, -8.0f)); // 3 左下角

    _position = _path[0];    // 從右下角出發
    _dir = glm::normalize(_path[1] - _path[0]); // 朝右上角
    _lightObj.setRotate(0.0f, glm::vec3(0, 1, 0)); // 初始朝向（+Z）
}

CLight::CLight(
    glm::vec3 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
    float constant, float linear, float quadratic, bool withMotion)
    : _lightObj(2.0f, 4, 4, 2)
{
    _position = position;
    _ambient = ambient;  _diffuse = diffuse;  _specular = specular;
    _ambient.w = 1.0f; _diffuse.w = 1.0f; _specular.w = 1.0f;
    _constant = constant; _linear = linear; _quadratic = quadratic;
    _intensity = 1.0f;
    _type = LightType::POINT;
    _posStart = position;
    _direction = glm::vec3(1.0f);
    _target = glm::vec3(1.0f);
    _innerCutOff = 0.0f; _outerCutOff = 0.0f; _exponent = 1.0f;
    _displayOn = true; _motionOn = withMotion; _clock = 0.0f;
    _lighingOn = true;

    _lightObj.setPos(position);

    if (withMotion) {
        _path.push_back(glm::vec3(8.0f, _position.y, -8.0f));
        _path.push_back(glm::vec3(8.0f, _position.y, 8.0f));
        _path.push_back(glm::vec3(-8.0f, _position.y, 8.0f));
        _path.push_back(glm::vec3(-8.0f, _position.y, -8.0f));
        _position = _path[0];
        _dir = glm::normalize(_path[1] - _path[0]);
        _lightObj.setRotate(0.0f, glm::vec3(0, 1, 0));
    }
}

// 目標式聚光燈建構元
CLight::CLight(glm::vec3 position, glm::vec3 target, float innerCutOffDeg, float outerCutOffDeg, float exponent,
    glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float constant, float linear, float quadratic) {
    _position = position; _target = target; _direction = glm::normalize(target - position);
    _intensity = 1.0f;  _exponent = exponent;
    _innerCutOff = glm::cos(glm::radians(innerCutOffDeg)); 
    _outerCutOff = glm::cos(glm::radians(outerCutOffDeg));
    _ambient = ambient;  _diffuse = diffuse;  _specular = specular;
    _ambient.w = 1.0f; _diffuse.w = 1.0f; _specular.w = 1.0f;
    _constant = constant; _linear = linear; _quadratic = quadratic;
    _type = LightType::SPOT; _posStart = position;
    _displayOn = true; _motionOn = false; _clock = 0.0f;
    _lighingOn = true; _lightObj.setPos(position);
}

CLight::~CLight() = default; // 讓編譯器產生預設的解構元

// 初始位置由建構元來設定，更新光源位置則是呼叫 setPos
void CLight::setPos(glm::vec3 pos) { 
    _position = pos; 
    // 如果是 spot light 則更新 direction
    if ( _type == LightType::SPOT ) {
        _direction = glm::normalize(_target - _position);
    }
}
glm::vec3 CLight::getPos()  { return _position; }

void CLight::setAmbient( glm::vec4 amb) { _ambient = amb; }
glm::vec4 CLight::getAmbient()  { return _ambient; }

void CLight::setDiffuse( glm::vec4 diff,int pattern) { 
    _diffuse = diff;
    glm::vec4 topColor, bottomColor;
    switch (pattern) {
    case 1: // 藍
        _lightObj.generateSphere(5.0, 15, 15, 1);
        _lightObj.setupVertexAttributes();
        _lightObj.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
        _lightObj.setPos(_position);
        updateToShader();
        draw();
        break;
    case 2: // 橘
        _lightObj.generateSphere(5.0f, 15, 15, 2);
        _lightObj.setupVertexAttributes();
        _lightObj.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
        _lightObj.setPos(_position);
        updateToShader();
        draw();
        break;
    case 3:
        break;
    case 4:
        _lightObj.generateSphere(5.0f, 15, 15, 4);
        _lightObj.setupVertexAttributes();
        _lightObj.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
        _lightObj.setPos(_position);
        updateToShader();
        draw();
        break;
    case 5:
        break;
    case 6:
        _lightObj.generateSphere(5.0f, 15, 15, 6);
        _lightObj.setupVertexAttributes();
        _lightObj.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
        _lightObj.setPos(_position);
        updateToShader();
        draw();
        break;
    }
}
glm::vec4 CLight::getDiffuse()  { return _diffuse; }

void CLight::setSpecular( glm::vec4 spec) { _specular = spec; }
glm::vec4 CLight::getSpecular()  { return _specular; }

void CLight::setIntensity(float intensity) { 
    _intensity = intensity;
    _ambient  = _ambient  * _intensity;
    _diffuse  = _diffuse  * _intensity;
    _specular = _specular * _intensity;
    _ambient.w = 1.0f; _diffuse.w = 1.0f; _specular.w = 1.0f;
}

void CLight::setAttenuation(float c, float l, float q) {
    _constant = c; _linear = l;  _quadratic = q;
}
void CLight::getAttenuation(float& c, float& l, float& q) {
    c = _constant; l = _linear;  q = _quadratic; 
}

void CLight::setLightOn(bool enable) { _lighingOn = enable; }
bool CLight::isLightOn(){ return _lighingOn; }

void CLight::setMotionEnabled() { _motionOn = !_motionOn; }

// 設定 shaderID 時，預設光源物件顯示為 true
void CLight::setShaderID(GLuint shaderProg, std::string name, bool displayon)
{
    _shaderID = shaderProg; _lightname = name;
    // 如果沒有啟用，就把強度設為零
    glm::vec4 amb = _lighingOn ? _ambient : glm::vec4(0.0f);
    glm::vec4 diff = _lighingOn ? _diffuse : glm::vec4(0.0f);
    glm::vec4 spec = _lighingOn ? _specular : glm::vec4(0.0f);

    GLint loc = glGetUniformLocation(_shaderID, (_lightname + ".position").c_str());
    glUniform3fv(loc, 1, glm::value_ptr(_position));

    loc = glGetUniformLocation(_shaderID, (_lightname + ".ambient").c_str());
    glUniform4fv(loc, 1, glm::value_ptr(amb));

    loc = glGetUniformLocation(_shaderID, (_lightname + ".diffuse").c_str());
    glUniform4fv(loc, 1, glm::value_ptr(diff));

    loc = glGetUniformLocation(_shaderID, (_lightname + ".specular").c_str());
    glUniform4fv(loc, 1, glm::value_ptr(spec));

    loc = glGetUniformLocation(_shaderID, (_lightname + ".constant").c_str());
    glUniform1f(loc, _constant);

    loc = glGetUniformLocation(_shaderID, (_lightname + ".linear").c_str());
    glUniform1f(loc, _linear);

    loc = glGetUniformLocation(_shaderID, (_lightname + ".quadratic").c_str());
    glUniform1f(loc, _quadratic);

    loc = glGetUniformLocation(_shaderID, "lightType");
    glUniform1i(loc, _type);

    if ( _type == LightType::SPOT ) {
        loc = glGetUniformLocation(_shaderID, (_lightname + ".direction").c_str());
        glUniform3fv(loc, 1, glm::value_ptr(_direction));

        loc = glGetUniformLocation(_shaderID, (_lightname + ".cutOff").c_str());
        glUniform1f(loc, _innerCutOff);

        loc = glGetUniformLocation(_shaderID, (_lightname + ".outerCutOff").c_str());
        glUniform1f(loc, _outerCutOff);
    }
    _displayOn = displayon;
    if ( _displayOn ) { // 設定代表光源物件的顯示
        _lightObj.setupVertexAttributes();
        _lightObj.setShaderID(_shaderID);
        _lightObj.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
        _lightObj.setPos(_position);
    }
}

void CLight::updateToShader()
{
    if (_shaderID == 0) return;

    GLint loc;

    auto setUniform3fv = [&](const std::string& name, const glm::vec3& val) {
        loc = glGetUniformLocation(_shaderID, name.c_str());
        if (loc != -1) glUniform3fv(loc, 1, glm::value_ptr(val));
    };

    auto setUniform4fv = [&](const std::string& name, const glm::vec4& val) {
        loc = glGetUniformLocation(_shaderID, name.c_str());
        if (loc != -1) glUniform4fv(loc, 1, glm::value_ptr(val));
    };

    auto setUniform1f = [&](const std::string& name, float val) {
        loc = glGetUniformLocation(_shaderID, name.c_str());
        if (loc != -1) glUniform1f(loc, val);
    };

    auto setUniform1i = [&](const std::string& name, int val) {
        loc = glGetUniformLocation(_shaderID, name.c_str());
        if (loc != -1) glUniform1i(loc, val);
    };

    setUniform1i(_lightname + ".enabled", _enabled ? 1 : 0);
    setUniform3fv(_lightname + ".position", _position);
    setUniform4fv(_lightname + ".ambient", _ambient);
    setUniform4fv(_lightname + ".diffuse", _diffuse);
    setUniform4fv(_lightname + ".specular", _specular);
    setUniform1f(_lightname + ".constant", _constant);
    setUniform1f(_lightname + ".linear", _linear);
    setUniform1f(_lightname + ".quadratic", _quadratic);
    setUniform1i(_lightname + ".lightType", _type); // 建議使用 per-light 成員

    if (_type == LightType::SPOT) {
        setUniform3fv(_lightname + ".direction", _direction);
        setUniform1f(_lightname + ".cutOff", _innerCutOff);
        setUniform1f(_lightname + ".outerCutOff", _outerCutOff);
        setUniform1f(_lightname + ".exponent", _exponent);
    }
}

glm::vec3 CLight::getTarget() { return _target; }
glm::vec3 CLight::getDirection() { return _direction; }

void CLight::setTarget(const glm::vec3& target) {
    _target = target;
    _direction = glm::normalize(_target - _position);  // 計算 spot light 的照明方向
    _type = LightType::SPOT; // 設定成 spot light
}

void CLight::setCutOffDeg(float innerDeg, float outerDeg, float exponent) {
    _innerCutOff = glm::cos(glm::radians(innerDeg));
    _outerCutOff = glm::cos(glm::radians(outerDeg));
    _exponent = exponent;
    _type = LightType::SPOT;
}

void CLight::update(float dt)
{
    //std::cout << _motionOn << std::endl;
    if (_motionOn) updateMotion(dt);
}

void CLight::updateMotion(float dt) {
    if (!_motionOn) return;

    glm::vec3 target = _path[_currTarget];  // 移除不必要的指標
    glm::vec3 toTarget = target - _position;
    float distance = glm::length(toTarget);

    if (distance < 0.1f) {
        _prevTarget = _currTarget;
        _currTarget = (_currTarget + 1) % _path.size();
        glm::vec3 next = _path[_currTarget];
        _dir = glm::normalize(next - _position);

        // 改用更穩定的方向判斷
        float angle = 0.0f;
        const float threshold = 0.99f; // 用於浮點數比較的閾值

        if (glm::dot(_dir, glm::vec3(0, 0, 1)) > threshold)       angle = 0.0f;
        else if (glm::dot(_dir, glm::vec3(1, 0, 0)) > threshold)  angle = 90.0f;
        else if (glm::dot(_dir, glm::vec3(0, 0, -1)) > threshold) angle = 180.0f;
        else if (glm::dot(_dir, glm::vec3(-1, 0, 0)) > threshold) angle = 270.0f;

        _lightObj.setRotate(angle, glm::vec3(0, 1, 0));
    }

    _position += _dir * _moveSpeed * dt;
    setPos(_position);
    if (_displayOn)
        _lightObj.setPos(_position);
}

void CLight::draw()
{
    if( _displayOn ) _lightObj.draw(); // 顯示代表光源的模型
}

void CLight::drawRaw()
{
    //std::cout << "LightPos: " << _position.x << _position.y << _position.z << std::endl;
    //std::cout << "Direction: " << _direction.x << _direction.y << _direction.z << std::endl;
    if (_displayOn) _lightObj.draw(); // 顯示代表光源的模型
}