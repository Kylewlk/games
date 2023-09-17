//
// Created by goksu on 2/25/20.
//
#pragma once
#include "Scene.hpp"


namespace Game101_HW7
{

struct RenderTask
{
    int startLine{0};
    int lineCount{0};
    std::atomic_bool isRendering{false};
};

struct hit_payload
{
    float tNear;
    uint32_t index;
    Vector2f uv;
    Object* hit_obj;
};

class Renderer
{
public:
    void Render(const Scene& scene);

    void Render(const Scene& scene, std::vector<Vector3f>& framebuffer, RenderTask& task);

private:
};

}
