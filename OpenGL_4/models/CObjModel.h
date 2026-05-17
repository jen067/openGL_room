#pragma once
#include "CShape.h"
#include <string>
#include <vector>

class CObjModel : public CShape {
public:
    CObjModel(const std::string& filepath);
    ~CObjModel();

    void draw();
    void drawRaw();
    void update(float dt) override;
    void setMotionEnabled();
    void updateMotion(float dt);

private:
    void loadObj(const std::string& filepath);
    float _clock = 0.0f;    // 動態控制的 clock
    bool  _motionOn;
    bool _isOpen = false;       // 門目前是否開啟
    bool _isAnimating = false;  // 是否正在動畫中
};
