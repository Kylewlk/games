//
// Created by Yun on 2022/10/1.
//
#include "RaytracingBVHScene.h"
#include "common/Texture.h"
#include "Triangle.hpp"

namespace Game101_HW6
{

RaytracingBVHScene::RaytracingBVHScene()
    :BaseScene(ID, 0, 0), scene(BufferWidth, BufferHeight)
{
    this->init();
}

RaytracingBVHScene::~RaytracingBVHScene()
{
    stopRender();
    delete bunny;
}

SceneRef RaytracingBVHScene::create()
{
    struct enable_make_shared : public RaytracingBVHScene
    {
        enable_make_shared() = default;
    };
    return std::make_shared<enable_make_shared>();
}

bool RaytracingBVHScene::init()
{
//    Scene scene(1280, 960);

//    MeshTriangle bunny("src/games/101graphic/models/bunny/bunny.obj");
//    scene.Add(&bunny);
    bunny = new MeshTriangle("asset/model/bunny/bunny.obj");
    scene.Add(bunny);

    scene.Add(std::make_unique<Light>(Vector3f(-20, 70, 20), 1.0f));
    scene.Add(std::make_unique<Light>(Vector3f(20, 70, 20), 1.0f));
    scene.buildBVH();

    this->texture = Texture::create(GL_RGB32F, BufferWidth, BufferHeight);

    this->startRender();

    return true;
}

void RaytracingBVHScene::startRender()
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

void RaytracingBVHScene::stopRender()
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

void RaytracingBVHScene::draw()
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

void RaytracingBVHScene::drawSettings()
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
