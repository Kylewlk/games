//
// Created by wlk12 on 2023/9/14.
//

#include "BaseScene.h"
#include "common/Texture.h"
#include "common/Shader.h"
#include "common/FrameBuffer.h"
#include "common/EventSystem.h"
#include "camera/Camera2D.h"
#include "common/Logger.h"

BaseScene::BaseScene(const char* id, int width, int height)
    : Scene(id, width, height)
{
    this->shader = Shader::createByPath("asset/shader/picture.vert", "asset/shader/picture.frag");
    this->camera = Camera2D::create();

    BaseScene::reset();
}

void BaseScene::reset()
{
    this->color = { 1.0f, 1.0f, 1.0f, 1.0f};
    this->rotation = 0.0f;
    this->camera->resetView();
}

void BaseScene::draw()
{
    if (this->texture == nullptr)
    {
        return;
    }

    this->camera->setViewSize((float)this->width, (float)this->height);
    this->camera->update();

    auto mat = camera->getViewProj() * math::rotateDegree(rotation, math::Z_Axis);

    // Y 轴乘以 -1 是为了让 纹理坐标的原点在左上角，Y 轴向下
    mat = mat * math::scale({(float)texture->getWidth() * 0.5f, (float)texture->getHeight() * -0.5f, 1.0f});

    shader->use();
    glUniformMatrix4fv(1, 1, false, (float*)&mat);
    glUniform4fv(2, 1, (float*)&color);
    shader->bindTexture(3, this->texture);
    drawQuad();
}

void BaseScene::drawSettings()
{
    ImGui::ColorEdit4("Color", (float*)&color);
    ImGui::DragFloat("Rotate", &rotation);
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
        if (ImGui::Button("Reset"))
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

    if (e->mouseEventType == MouseEvent::kMouseScroll)
    {
        float scale = this->camera->getViewScale();
        scale = (e->scrollY > 0) ? scale * 0.8f : scale * 1.25f;
        this->camera->setViewScale(scale);
    }
    else if (e->mouseEventType == MouseEvent::kMouseMove)
    {
        if (this->holdLeftButton)
        {
            auto delta = e->posDelta;
            delta *= this->camera->getViewScale();
            this->camera->move({delta.x, -delta.y, 0});
        }
    }
}
