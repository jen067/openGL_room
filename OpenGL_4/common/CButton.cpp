// CButton.cpp
#include "CButton.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

CButton::CButton(float width, float height, glm::vec4 colOff, glm::vec4 colOn)
{
	_width = width;  // 按鈕寬度
	_height = height;  // 按鈕高度
	_colorOff = colOff;
	_colorOn = colOn;
	_bActive = false;  // 按鈕初始狀態為未按下
    _screenW = 0;  _screenH = 0;
    // CSprite2D 預設 uniform color
}

CButton::~CButton() {}

void CButton::init(GLuint shaderProg, float screenW, float screenH) {
    _screenW = screenW;
    _screenH = screenH;
    setShaderID(shaderProg);
    generateQuad();
    setColor(_colorOff);  // uniform color
}

void CButton::generateQuad() {
    // pos(3) + color(3 dummy) + uv(2) = 8 floats
    _vtxAttrCount = 8;
    _vtxCount = 4;
    _idxCount = 6;

    _points = new GLfloat[_vtxCount * _vtxAttrCount];
    _idx = new GLuint[_idxCount];

    float hw = _width * 0.5f, hh = _height * 0.5f;
    // 以中心為原點的四頂點：x,y,z,   r,g,b (dummy),   u,v
    GLfloat verts[4][8] = {
        {-hw, -hh, 0.0f,   0,0,0,   0,0},
        { hw, -hh, 0.0f,   0,0,0,   1,0},
        { hw,  hh, 0.0f,   0,0,0,   1,1},
        {-hw,  hh, 0.0f,   0,0,0,   0,1},
    };
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < _vtxAttrCount; ++j)
            _points[i * _vtxAttrCount + j] = verts[i][j];

    GLuint idxs[6] = { 0,1,2,  2,3,0 };
    for (int i = 0; i < 6; ++i) _idx[i] = idxs[i];

    // 呼用基底的 setupVertexAttributes() 建立 VAO/VBO/EBO
    setupVertexAttributes();  // CSprite2D 提供
}

void CButton::setScreenPos(float sx, float sy) {
    // 直接對應到 bottom‑left 原點的正交投影
    setPos(glm::vec3(sx, sy, 0.0f));
    // 更新滑鼠偵測範圍
    _minXY = glm::vec2(sx - _width * 0.5f, sy - _height * 0.5f);
    _maxXY = glm::vec2(sx + _width * 0.5f, sy + _height * 0.5f);
}

bool CButton::isMouseOver(float mx, float my) const {
    return mx >= _minXY.x && mx <= _maxXY.x
        && my >= _minXY.y && my <= _maxXY.y;
}

bool CButton::handleClick(float mx, float my) {
    if (!isMouseOver(mx, my)) return false;
    _bActive = !_bActive;
    setColor(_bActive ? _colorOn : _colorOff);
    return true;
}

void CButton::draw()
{
    glUseProgram(_shaderProg);
    glBindVertexArray(_vao);
    updateMatrix();
    glUniform1i(_coloringModeLoc, _coloringMode);
    if (_bObjColor) glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
    glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void CButton::drawRaw()
{
    glBindVertexArray(_vao);
    updateMatrix();
    glUniform1i(_coloringModeLoc, _coloringMode);
    if (_bObjColor) glUniform4fv(_colorLoc, 1, glm::value_ptr(_color));
    glDrawElements(GL_TRIANGLES, _idxCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}