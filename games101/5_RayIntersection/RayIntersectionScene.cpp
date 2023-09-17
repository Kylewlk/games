//
// Created by Yun on 2022/10/1.
//
#include "RayIntersectionScene.h"
#include "common/Texture.h"
#include "Sphere.hpp"
#include "Triangle.hpp"
#include "Light.hpp"

namespace Game101_HW5
{

RayIntersectionScene::RayIntersectionScene()
    :BaseScene(ID, 0, 0), scene(BufferWidth, BufferHeight)
{
    this->init();
    RayIntersectionScene::reset();
}

RayIntersectionScene::~RayIntersectionScene()
{
    stopRender();
}

SceneRef RayIntersectionScene::create()
{
    struct enable_make_shared : public RayIntersectionScene
    {
        enable_make_shared() = default;
    };
    return std::make_shared<enable_make_shared>();
}

bool RayIntersectionScene::init()
{
//    Scene scene(1280, 960);

//    auto sph1 = std::make_unique<Sphere>(Vector3f(-1, 0, -12), 2.0f);
//    sph1->ior = 2.8;
//    sph1->materialType = REFLECTION;
//    sph1->diffuseColor = Vector3f(0.6, 0.7, 0.8);
//
//    auto sph2 = std::make_unique<Sphere>(Vector3f(2.5, -0.5, -8), 1.5f);
//    sph2->ior = 1.5;
//    sph2->diffuseColor = Vector3f(0.9, 0.0, 0.1);
//    sph2->materialType = REFLECTION_AND_REFRACTION;

    auto sph1 = std::make_unique<Sphere>(Vector3f(-1, 0, -12), 2.0f);
    sph1->materialType = DIFFUSE_AND_GLOSSY;
    sph1->diffuseColor = Vector3f(0.6, 0.7, 0.8);

    auto sph2 = std::make_unique<Sphere>(Vector3f(0.5, -0.5, -8), 1.5f);
    sph2->ior = 1.5;
    sph2->materialType = REFLECTION_AND_REFRACTION;

    scene.Add(std::move(sph1));
    scene.Add(std::move(sph2));

    Vector3f verts[4] = {{-5,-3,-6}, {5,-3,-6}, {5,-3,-16}, {-5,-3,-16}};
    uint32_t vertIndex[6] = {0, 1, 3, 1, 2, 3};
    Vector2f st[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    auto mesh = std::make_unique<MeshTriangle>(verts, vertIndex, 2, st);
    mesh->materialType = DIFFUSE_AND_GLOSSY;

    scene.Add(std::move(mesh));
    scene.Add(std::make_unique<Light>(Vector3f(-20, 70, 20), 0.5f));
    scene.Add(std::make_unique<Light>(Vector3f(30, 50, -12), 0.5f));

    this->texture = Texture::create(GL_RGB32F, BufferWidth, BufferHeight);

    this->startRender();

    return true;
}

void RayIntersectionScene::reset()
{
    BaseScene::reset();
}

void RayIntersectionScene::startRender()
{
    if (!this->isRendering)
    {
        frameBuffer.clear();
        frameBuffer.resize(BufferWidth*BufferHeight, {});
        this->texture->update(0, 0, BufferWidth, BufferHeight, GL_RGB, GL_FLOAT, frameBuffer.data());

        if (this->thread != nullptr)
        {
            if (this->thread->joinable())
            {
                this->thread->join();
            }
            delete this->thread;
            this->thread = nullptr;
        }

        this->thread = new std::thread([this](){
            this->isRendering = true;

            auto start = std::chrono::system_clock::now();
            this->renderer.Render(scene, frameBuffer, isRendering, renderLine);
            auto stop = std::chrono::system_clock::now();
            if (this->isRendering)
            {
                this->isRendering = false;
                this->needUpdateTex = true;
                std::cout << "Render complete: \n";
            }
            std::cout << "Time taken: "  << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() / 1000.0f << " seconds\n";
        });
    }
}

void RayIntersectionScene::stopRender()
{
    if (this->thread != nullptr)
    {
        this->isRendering = false;
        if (this->thread->joinable())
        {
            this->thread->join();
        }
        delete this->thread;
        this->thread = nullptr;
    }
}

void RayIntersectionScene::draw()
{
    if (this->isRendering && !frameBuffer.empty())
    {
        this->texture->update(0, 0, BufferWidth, this->renderLine, GL_RGB, GL_FLOAT, frameBuffer.data());
    }
    else if (this->needUpdateTex)
    {
        this->texture->update(0, 0, BufferWidth, BufferHeight, GL_RGB, GL_FLOAT, frameBuffer.data());
        this->needUpdateTex = false;
    }

    BaseScene::draw();
}

void RayIntersectionScene::drawSettings()
{

    if (ImGui::Button("Stop"))
    {
        this->stopRender();
    }

    if (ImGui::Button("Restart"))
    {
        this->startRender();
    }

}

}
