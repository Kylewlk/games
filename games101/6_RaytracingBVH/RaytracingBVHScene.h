//
// Created by Yun on 2022/10/1.
//
#pragma once
#include "Define.h"
#include "Renderer.hpp"
#include "camera/Camera2D.h"
#include "BaseScene.h"

namespace Game101_HW6
{

class RaytracingBVHScene;
using RaytracingBVHSceneRef = std::shared_ptr<RaytracingBVHScene>;

class MeshTriangle;

class RaytracingBVHScene : public BaseScene
{
public:
    static constexpr const char* ID = "05 Raytracing BVH";

    static SceneRef create();

    ~RaytracingBVHScene() override;
    RaytracingBVHScene(const RaytracingBVHScene&) = delete;
    RaytracingBVHScene& operator=(const RaytracingBVHScene&) = delete;

private:

    const int BufferWidth = 1280;
    const int BufferHeight = 960;

    RaytracingBVHScene();
    bool init();
    void startRender();
    void stopRender();

    void draw() override;
    void drawSettings() override;

    Game101_HW6::Scene scene;
    MeshTriangle* bunny{};
    Game101_HW6::Renderer renderer;
    std::vector<Vector3f> frameBuffer;

    std::thread* thread{};
    std::atomic_bool isRendering{false};
    std::atomic_int renderLine{0};
    std::atomic_bool needUpdateTex{false};
};

}