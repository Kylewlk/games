//
// Created by wlk12 on 2023/8/6.
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

    Camera3DRef camera;

    math::Vec2 lastMousePos{};
    bool holdLeftButton{false};
    bool holdMidButton{false};
};

