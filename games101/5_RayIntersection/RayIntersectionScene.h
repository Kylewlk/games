//
// Created by Yun on 2022/10/1.
//
#pragma once
#include "Define.h"
#include "BaseScene.h"
#include "Scene.hpp"
#include "Renderer.hpp"

namespace Game101_HW5
{

class RayIntersectionScene : public BaseScene
{
public:
    static constexpr const char* ID = "05 Ray Intersection";

    static SceneRef create();

    ~RayIntersectionScene() override;
    RayIntersectionScene(const RayIntersectionScene&) = delete;
    RayIntersectionScene& operator=(const RayIntersectionScene&) = delete;

private:

    const int BufferWidth = 1280;
    const int BufferHeight = 960;

    RayIntersectionScene();
    bool init();
    void startRender();
    void stopRender();

    void draw() override;
    void reset() override;
    void drawSettings() override;

    Game101_HW5::Scene scene;
    Game101_HW5::Renderer renderer;
    std::vector<Vector3f> frameBuffer;

    std::thread* thread{};
    std::atomic_bool isRendering{false};
    std::atomic_int renderLine{0};
    std::atomic_bool needUpdateTex{false};
};

}