//
// Created by wlk12 on 2023/8/6.
//
#pragma once

#include "Scene.hpp"

class ModelScene : public Scene
{
public:
    static constexpr const char* ID = "Model Scene";

    static SceneRef create();

    ~ModelScene() override = default;
    ModelScene(const ModelScene&) = delete;
    ModelScene& operator=(ModelScene&) = delete;

private:
    ModelScene(int width, int height);

    void onMouseEvent(const MouseEvent* e) override;

    void draw() override;
    void drawProperty() override;
    void reset();

    Camera3DRef camera;
    ShaderRef shader;

    ModelRef sphere;
    ModelRef cube;
    ModelRef plane;

    math::Vec3 lightColor{};
    math::Vec3 lightDir{};

    math::Vec3 sphereColor{};
    math::Vec3 cubeColor{};
    math::Vec3 planeColor{};

    math::Vec2 lastMousePos{};
    bool holdLeftButton{false};
    bool holdMidButton{false};
};

