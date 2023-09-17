//
// Created by Yun on 2022/10/1.
//
#pragma once
#include "Define.h"
#include "Renderer.hpp"
#include "camera/Camera2D.h"
#include "BaseScene.h"
#include <future>
#include <atomic>
#include <chrono>

namespace Game101_HW7
{

class MeshTriangle;
class Material;

class PathTracingScene : public BaseScene
{
public:
    static constexpr const char* ID = "07 Path Tracing";

    static SceneRef create();

    ~PathTracingScene() override;
    PathTracingScene(const PathTracingScene&) = delete;
    PathTracingScene& operator=(const PathTracingScene&) = delete;

private:

    static constexpr int BufferWidth = 784;
    static constexpr int BufferHeight = 784;
    static constexpr int maxTaskCount = 16;
    static constexpr int taskLineCount = 16;

    PathTracingScene();
    bool init();
    void startRender();
    void stopRender();

    void draw() override;
    void drawSettings() override;

    Material* red{};
    Material* green{};
    Material* white{};
    Material* light{};

    MeshTriangle* floor{};
    MeshTriangle* shortbox{};
    MeshTriangle* tallbox{};
    MeshTriangle* left{};
    MeshTriangle* right{};
    MeshTriangle* light_{};

    Game101_HW7::Scene scene;
    Game101_HW7::Renderer renderer;
    std::vector<Vector3f> frameBuffer;

    int spp{16};
    int currentSPP{0};
    float process{0.0f};
    std::chrono::steady_clock::time_point startRenderTime;

    int renderLine{0};
    bool isRendering{false};

    RenderTask tasks[maxTaskCount];
    std::future<RenderTask*> results[maxTaskCount];
};

}