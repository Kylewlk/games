//
// Created by Yun on 2022/10/1.
//
#pragma once
#include "Define.h"
#include "Renderer.hpp"
#include "camera/Camera2D.h"
#include "BaseScene.h"

namespace Game101_HW7
{

class PathTracingScene;
using PathTracingSceneRef = std::shared_ptr<PathTracingScene>;

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

    const int BufferWidth = 784;
    const int BufferHeight = 784;

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

    std::thread* thread{};
    std::atomic_bool isRendering{false};
    std::atomic_int renderLine{0};
    std::atomic_bool needUpdateTex{false};
};

}