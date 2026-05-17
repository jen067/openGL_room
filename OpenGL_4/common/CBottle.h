#pragma once
#include "CShape.h"

class CBottle : public CShape {
public:
    // radialSegs: 環向分段數
    // heightSegs: 高度方向分段數
    // pattern: 預設 1，提供 1~4 種顏色選擇
    CBottle(unsigned int radialSegs = 36, unsigned int heightSegs = 8, GLuint pattern = 1);
    virtual ~CBottle();

    virtual void draw() override;
    virtual void drawRaw() override;
    virtual void update(float dt) override;

protected:
    // 產生瓶身與底面，模型預設方向為 Z 軸朝上
    void generateBottle(unsigned int radialSegs, unsigned int heightSegs, GLuint pattern);
};