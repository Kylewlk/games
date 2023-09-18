//
// Created by Yun on 2022/10/1.
//
#include "PathTracingScene.h"
#include "common/Texture.h"
#include "Triangle.hpp"
#include <imgui/imgui_internal.h>

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

        renderLine = 0;
        isRendering = true;
        currentSPP = spp;
        process = 0.0f;
        this->startRenderTime = std::chrono::steady_clock::now();

        for (int i = 0; i < maxTaskCount; ++i)
        {
            tasks[i].spp = this->spp;
            tasks[i].startLine = renderLine;
            tasks[i].lineCount = taskLineCount;
            tasks[i].isRendering = true;
            results[i] = std::async([this, i](){
                this->renderer.Render(scene, frameBuffer, this->tasks[i]);
                return &this->tasks[i];
            });

            renderLine += taskLineCount;
        }
    }
}

void PathTracingScene::stopRender()
{
    this->isRendering = false;
    for (auto & t : tasks)
    {
        t.isRendering = false;
    }
    for (auto& result : results)
    {
        if (result.valid())
        {
            result.wait();
        }
    }
}

void PathTracingScene::draw()
{
    if (this->isRendering)
    {
        bool allTaskFinish = true;
        for (auto& result : results)
        {
            if (result.valid())
            {
                auto status = result.wait_for(std::chrono::microseconds(0));
                if (status == std::future_status::ready)
                {
                    auto t = result.get();
                    texture->update(0, t->startLine, BufferWidth, t->lineCount, GL_RGB, GL_FLOAT,
                                    frameBuffer.data() + (t->startLine * BufferWidth));
                    if (this->renderLine < BufferHeight)
                    {
                        t->startLine = renderLine;
                        t->lineCount = std::min(BufferHeight - renderLine, taskLineCount);
                        t->isRendering = true;
                        result = std::async([this, t](){
                            this->renderer.Render(scene, frameBuffer, *t);
                            return t;
                        });
                        this->renderLine += t->lineCount;

                        allTaskFinish = false;
                    }
                    this->process += float(t->lineCount) / float(BufferHeight);
                }
                else
                {
                    allTaskFinish = false;
                }
            }
        }
        auto now = std::chrono::steady_clock::now();
        this->renderTime = (float)std::chrono::duration_cast<std::chrono::milliseconds>(now - startRenderTime).count() / 1000.0f;
        if (allTaskFinish)
        {
            this->isRendering = false;
            auto finishTime = std::chrono::steady_clock::now();
            std::cout << "Render complete," << "Time taken: "  << renderTime << " seconds\n";
        }
   }

    BaseScene::draw();
}

void PathTracingScene::drawSettings()
{
    ImGui::Text("Current Render SPP: %d", this->spp);
    if ( this->isRendering)
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }

    ImGui::SliderInt("SPP(Sample Per Pixel)", &spp, 1, 256);

    if ( this->isRendering)
    {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }

    if (isRendering)
    {
        if (ImGui::Button("Stop"))
        {
            this->stopRender();
        }
    }
    else
    {
        if (ImGui::Button("Restart"))
        {
            this->startRender();
        }
    }

    ImGui::ProgressBar(process);
    ImGui::Text("Time: %.2fs", renderTime);

}

}
