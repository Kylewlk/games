//
// Created by goksu on 2/25/20.
//
#pragma once

#include "Scene.hpp"

namespace Game101_HW6
{

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

    void Render(const Scene& scene, std::vector<Vector3f>& framebuffer,
                std::atomic_bool& isContinue, std::atomic_int& renderedLine);

private:
};

}
