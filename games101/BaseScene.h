//
// Created by wlk12 on 2023/9/14.
//
#pragma once

#include "scene/Scene.hpp"

class BaseScene : public Scene
{
public:
    
    ~BaseScene() override = default;
    BaseScene(const BaseScene&) = delete;
    BaseScene& operator=(BaseScene&) = delete;

protected:
    BaseScene(const char* id, int width, int height);
    
    void onMouseEvent(const MouseEvent* e) override;
    void draw() override;
    void drawProperty() override;

    virtual void reset();
    virtual void drawSettings();

    TextureRef texture;
    ShaderRef shader;
    Camera2DRef camera;

    math::Vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    float rotation = 0.0f;

    bool holdLeftButton{false};
};

