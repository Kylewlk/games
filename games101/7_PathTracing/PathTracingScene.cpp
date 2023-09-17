//
// Created by Yun on 2022/10/1.
//
#include "PathTracingScene.h"
#include "common/Texture.h"
#include "Triangle.hpp"

namespace Game101_HW7
{

PathTracingScene::PathTracingScene()
    :BaseScene(ID, 0, 0), scene(BufferWidth, BufferHeight)
{
    this->init();
}

PathTracingScene::~PathTracingScene()
{
    stopRender();

    delete red;
    delete green;
    delete white;
    delete light;

    delete floor;
    delete shortbox;
    delete tallbox;
    delete left;
    delete right;
    delete light_;
}

SceneRef PathTracingScene::create()
{
    struct enable_make_shared : public PathTracingScene
    {
        enable_make_shared() = default;
    };
    return std::make_shared<enable_make_shared>();
}

bool PathTracingScene::init()
{
    // Change the definition here to change resolution
//    Scene scene(784, 784);

    red = new Material(DIFFUSE, Vector3f(0.0f));
    red->Kd = Vector3f(0.63f, 0.065f, 0.05f);
    green = new Material(DIFFUSE, Vector3f(0.0f));
    green->Kd = Vector3f(0.14f, 0.45f, 0.091f);
    white = new Material(DIFFUSE, Vector3f(0.0f));
    white->Kd = Vector3f(0.725f, 0.71f, 0.68f);
    light = new Material(DIFFUSE, (8.0f * Vector3f(0.747f+0.058f, 0.747f+0.258f, 0.747f) + 15.6f * Vector3f(0.740f+0.287f,0.740f+0.160f,0.740f) + 18.4f *Vector3f(0.737f+0.642f,0.737f+0.159f,0.737f)));
    light->Kd = Vector3f(0.65f);

    floor = new MeshTriangle("asset/model/cornellbox/floor.obj", white);
    shortbox = new MeshTriangle("asset/model/cornellbox/shortbox.obj", white);
    tallbox = new MeshTriangle("asset/model/cornellbox/tallbox.obj", white);
    left = new MeshTriangle("asset/model/cornellbox/left.obj", red);
    right = new MeshTriangle("asset/model/cornellbox/right.obj", green);
    light_ = new MeshTriangle("asset/model/cornellbox/light.obj", light);

    scene.Add(floor);
    scene.Add(shortbox);
    scene.Add(tallbox);
    scene.Add(left);
    scene.Add(right);
    scene.Add(light_);

    scene.buildBVH();

    texture = Texture::create(GL_RGB32F, BufferWidth, BufferHeight);

    this->startRender();

    return true;
}

void PathTracingScene::startRender()
{
    if (!this->isRendering)
    {
        frameBuffer.clear();
        frameBuffer.resize(BufferWidth*BufferHeight, {});
        texture->update(0, 0, BufferWidth, BufferHeight, GL_RGB, GL_FLOAT, frameBuffer.data());

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

void PathTracingScene::stopRender()
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

void PathTracingScene::draw()
{
    if (this->isRendering && !frameBuffer.empty())
    {
        texture->update(0, 0, BufferWidth, this->renderLine, GL_RGB, GL_FLOAT, frameBuffer.data());
    }
    else if (this->needUpdateTex)
    {
        texture->update(0, 0, BufferWidth, BufferHeight, GL_RGB, GL_FLOAT, frameBuffer.data());
        this->needUpdateTex = false;
    }

    BaseScene::draw();
}

void PathTracingScene::drawSettings()
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
