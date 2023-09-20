//
// Created by wlk12 on 2023/8/6.
//

#include "BaseScene.h"
#include "common/Texture.h"
#include "common/Shader.h"
#include "common/FrameBuffer.h"
#include "common/EventSystem.h"
#include "camera/Camera3D.h"
#include "common/Logger.h"
#include "common/Model.h"

BaseScene::BaseScene(const char* id,int width, int height)
    : Scene(id, width, height)
{
    this->camera = Camera3D::create();
    this->camera->setLockUp(true);

    BaseScene::reset();
}

void BaseScene::reset()
{
    this->camera->resetView();
}

void BaseScene::draw()
{
    this->camera->setViewSize((float)this->width, (float)this->height);
    this->camera->update();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
}

void BaseScene::drawSettings()
{
}

void BaseScene::drawProperty()
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
        if (ImGui::Button("Save"))
        {
            const auto& pixels = this->fbResolved->readPixel();
            std::string path = ".data/";
            path += this->getName();
            path += ".png";
            stbi_flip_vertically_on_write(true);
            stbi_write_png(path.c_str(), width, height, 4, pixels.data(), width * 4);

            auto workingDir = std::filesystem::current_path().u8string();
            LOGI("Save to picture: {}/{}", (const char*)workingDir.data(), path);
        }

        ImGui::Separator();
        this->drawSettings();
    }
    ImGui::End();
}

void BaseScene::onMouseEvent(const MouseEvent* e)
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
