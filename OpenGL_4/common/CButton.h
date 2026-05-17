// CButton.h
#pragma once

#include "CSprite2D.h"
#include <glm/glm.hpp>

class CButton : public CSprite2D {
public:
    // width, height：按鈕尺寸（像素）
    // colOff, colOn：非按下／按下時的顏色
    CButton(float width = 50.0f,
        float height = 50.0f,
        glm::vec4 colOff = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
        glm::vec4 colOn = glm::vec4(0.3f, 0.6f, 1.0f, 1.0f));
    virtual ~CButton();
    // 必須在建立好 GL context、shader program 後呼叫
    // screenW, screenH：視窗寬高，用於 Y 座標翻轉
    void init(GLuint shaderProg, float screenW, float screenH);
    // 以「螢幕座標」設定中心位置 (0,0 為左上，Y 向下為正)
    void setScreenPos(float sx, float sy);
    // 滑鼠是否在按鈕範圍內
    bool isMouseOver(float mouseX, float mouseY) const;
    // 處理點擊：若點擊在按鈕上，切換顏色並回傳 true
    bool handleClick(float mouseX, float mouseY);
    virtual void draw() override;
    virtual void drawRaw() override;

private:
    void generateQuad();

    float     _width, _height;    // 按鈕寬高
    float     _screenW, _screenH; // 螢幕寬高
    glm::vec4 _colorOff, _colorOn;    // 兩種狀態顏色
    bool      _bActive;            // 目前是否按下
    glm::vec2 _minXY, _maxXY;     // 範圍，用於滑鼠偵測
};

