//
// Created by wlk12 on 2023/8/6.
//

#include "ModelScene.h"
#include "common/Texture.h"
#include "common/Shader.h"
#include "common/FrameBuffer.h"
#include "common/EventSystem.h"
#include "camera/Camera3D.h"
#include "common/Logger.h"
#include "common/Model.h"

ModelScene::ModelScene(int width, int height)
    : Scene(ID, width, height)
{
    this->camera = Camera3D::create();
    this->camera->setLockUp(true);

    this->shader = Shader::createByPath("asset/shader/model.vert", "asset/shader/model.frag");

    this->sphere = Model::create("asset/model/sphere.obj");
    this->cube = Model::create("asset/model/cube.obj");
    this->plane = Model::create("asset/model/plane.obj");

    ModelScene::reset();
}

SceneRef ModelScene::create()
{
    struct enable_make_shared : public ModelScene
    {
        enable_make_shared() : ModelScene(0, 0) {}
    };
    return std::make_shared<enable_make_shared>();
}

void ModelScene::reset()
{
    this->camera->resetView();
    this->camera->round(-20, 0);
    this->camera->round(0, -60);

    this->lightColor = {0.9, 0.9, 0.9};
    this->lightDir = {1, 1, 0.3};

    this->sphereColor = math::Vec3{0.3, 0.7, 0.5};
    this->cubeColor = math::Vec3{0.2, 0.6, 0.8};
    this->planeColor = math::Vec3{0.4, 0.4, 0.4};
}

void ModelScene::draw()
{
    this->camera->setViewSize((float)this->width, (float)this->height);
    this->camera->update();

    shader->use();
    shader->setUniform("viewProj", camera->getViewProj());

    shader->setUniform("lightColor", lightColor);
    shader->setUniform("lightDir", glm::normalize(lightDir));
    shader->setUniform("cameraPos", camera->getViewPosition());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
//    glDisable(GL_MULTISAMPLE);


    auto mat = math::translate({120, 1, 0}) * math::scale({100, 100, 100});
    auto normalMat = glm::transpose(glm::inverse(math::Mat3{mat}));
    shader->setUniform("albedo", cubeColor);
    shader->setUniform("model", mat);
    shader->setUniform("normalMatrix", normalMat);
    this->cube->draw();


    mat = math::translate({-120, 50, 0}) * math::scale({100, 100, 100});
    normalMat = glm::transpose(glm::inverse(math::Mat3{mat}));
    shader->setUniform("albedo", sphereColor);
    shader->setUniform("model", mat);
    shader->setUniform("normalMatrix", normalMat);
    this->sphere->draw();


    mat = math::translate({0, -50, 0}) * math::scale({500, 500, 500});
    normalMat = glm::transpose(glm::inverse(math::Mat3{mat}));
    shader->setUniform("albedo", planeColor);
    shader->setUniform("model", mat);
    shader->setUniform("normalMatrix", normalMat);
    this->plane->draw();
}

void ModelScene::drawProperty()
{
    if (!showPropertyWindow)
    {
        return;
    }

    if(ImGui::Begin(Scene::PropertyWindow, &showPropertyWindow, 0))
    {
        ImGui::SetWindowSize({300, 400}, ImGuiCond_FirstUseEver);
        if (ImGui::Button("Reset", {100.0f, 0}))
        {
            this->reset();
        }
        ImGui::SameLine(0, 20);
        if (ImGui::Button("Save Picture"))
        {
            const auto& pixels = this->fbResolved->readPixel();
            stbi_flip_vertically_on_write(true);
            stbi_write_png(".data/picture-scene.png", width, height, 4, pixels.data(), width * 4);

            auto workingDir = std::filesystem::current_path().u8string();
            LOGI("Save to picture: {}/{}", (const char*)workingDir.data(), ".data/picture-scene.png");
        }

        ImGui::Separator();
        ImGui::ColorEdit3("Light Color", (float*)&lightColor, ImGuiColorEditFlags_Float);
        ImGui::DragFloat3("Light Direction", (float*)&lightDir);

        ImGui::Separator();
        ImGui::ColorEdit3("Sphere Color", (float*)&sphereColor, ImGuiColorEditFlags_Float);
        ImGui::ColorEdit3("Cube Color", (float*)&cubeColor, ImGuiColorEditFlags_Float);
        ImGui::ColorEdit3("Plane Color", (float*)&planeColor, ImGuiColorEditFlags_Float);
    }
    ImGui::End();
}

void ModelScene::onMouseEvent(const MouseEvent* e)
{
    if (e->mouseButton == MouseEvent::kButtonLeft)
    {
        if (e->mouseEventType == MouseEvent::kMousePress)
        {
            this->holdLeftButton = true;
        }
        else if (e->mouseEventType == MouseEvent::kMouseRelease)
        {
            this->holdLeftButton = false;
        }
    }

    if (e->mouseButton == MouseEvent::kButtonMiddle)
    {
        if (e->mouseEventType == MouseEvent::kMousePress)
        {
            this->holdMidButton = true;
        }
        else if (e->mouseEventType == MouseEvent::kMouseRelease)
        {
            this->holdMidButton = false;
        }
    }

    if (e->mouseEventType == MouseEvent::kMouseScroll)
    {
        this->camera->forward((float)e->scrollY*20.0f);
    }
    else if (e->mouseEventType == MouseEvent::kMouseMove)
    {
        auto delta = e->posDelta;
        if (this->holdLeftButton)
        {
            this->camera->round(delta.x, -delta.y);
        }
        else if(this->holdMidButton)
        {
            this->camera->move({delta.x, -delta.y, 0});
        }
    }
}
