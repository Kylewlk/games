//
// Created by Yun on 2022/10/1.
//
#include "GeometryScene.h"
#include "common/EventSystem.h"
#include "common/Shader.h"
#include "camera/Camera2D.h"

namespace Game101_HW4
{

GeometryScene::GeometryScene()
        : BaseScene(ID, 0, 0)
{
    this->init();
    GeometryScene::reset();
}

GeometryScene::~GeometryScene()
{
    if (this->vao != 0)
    {
        glDeleteVertexArrays(1, &vao);
    }
    if (this->vbo != 0)
    {
        glDeleteBuffers(1, &vbo);
    }

    if (this->ctrlVao != 0)
    {
        glDeleteVertexArrays(1, &ctrlVao);
    }
    if (this->ctrlVbo != 0)
    {
        glDeleteBuffers(1, &ctrlVbo);
    }
}

SceneRef GeometryScene::create()
{
    struct enable_make_shared : public GeometryScene
    {
        enable_make_shared() = default;
    };
    return std::make_shared<enable_make_shared>();
}

bool GeometryScene::init()
{
    this->shader = Shader::createByPath("asset/shader/solid.vert", "asset/shader/solid.frag");

    glGenVertexArrays(1, &vao);
    glGenVertexArrays(1, &ctrlVao);

    return true;
}

void GeometryScene::reset()
{
    points[0] = math::Vec2{-200, -200};
    points[1] = math::Vec2{-200, 200};
    points[2] = math::Vec2{200, 200};
    points[3] = math::Vec2{200, -200};
    this->activeIndex = -1;
    this->step = 0.02;
    this->drawType = 0;

    bezier(drawType == 0);
    updatePoints();

    BaseScene::reset();
}

void GeometryScene::draw()
{
    this->camera->setViewSize((float)this->width, (float)this->height);
    this->camera->update();

    auto mat = camera->getViewProj() * math::rotateDegree(rotation, math::Z_Axis);

    glEnable(GL_MULTISAMPLE);
    glPointSize(8);

    this->shader->use();
    this->shader->setUniform("mvp", mat);

    this->shader->setUniform("color", math::Vec4{0.4, 0.8, 0.6, 1.0});
    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_STRIP, 0, this->vertexCount);

    this->shader->setUniform("color", math::Vec4{0.4, 0.3, 0.9, 1.0});
    glBindVertexArray(ctrlVao);
    glDrawArrays(GL_POINTS, 0, 4);

    if (this->activeIndex >= 0)
    {
        this->shader->setUniform("color", math::Vec4{0.4, 0.9, 0.2, 1.0});
        glDrawArrays(GL_POINTS, activeIndex, 1);
    }

    glBindVertexArray(0);
}

void GeometryScene::onMouseEvent(const MouseEvent* e)
{
    if (e->mouseButton == MouseEvent::kButtonLeft)
    {
        if (e->mouseEventType == MouseEvent::kMousePress)
        {
            this->activeIndex = -1;
            auto pos = this->camera->screenToWorld(e->point);
            auto dist = 10 * this->camera->getViewScale();
            for (int i = 0; i < 4; ++i)
            {
                if (glm::distance(pos, points[i]) < dist)
                {
                    this->activeIndex = i;
                }
            }
        }
        else if (e->mouseEventType == MouseEvent::kMouseRelease)
        {
            this->activeIndex = -1;
        }
    }
    else if (e->mouseEventType == MouseEvent::kMouseMove)
    {
        if (this->activeIndex >= 0)
        {
            auto pos = this->camera->screenToWorld(e->point);
            auto delta = pos - lastMousePos;
            this->points[activeIndex] += delta;
            this->bezier(this->drawType == 0);
            this->updatePoints();
        }
    }

    this->lastMousePos = this->camera->screenToWorld(e->point);
    if (this->activeIndex == -1)
    {
        BaseScene::onMouseEvent(e);
    }
}

void GeometryScene::drawSettings()
{
    bool needUpdate{false};

    needUpdate = ImGui::SliderFloat("Step", &this->step, 0.001f, 0.2f, "%.3f",  ImGuiSliderFlags_AlwaysClamp);

    ImGui::Separator();
    needUpdate = ImGui::RadioButton("Native", &this->drawType, 0) || needUpdate;
    needUpdate =  ImGui::RadioButton("de Casteljau", &this->drawType, 1) || needUpdate;

    if (needUpdate)
    {
        this->bezier(this->drawType == 0);
    }

}

math::Vec2 GeometryScene::naive_bezier(float t)
{
    auto& p_0 = points[0];
    auto& p_1 = points[1];
    auto& p_2 = points[2];
    auto& p_3 = points[3];

    auto point = std::pow(1 - t, 3.0f) * p_0 + 3 * t * std::pow(1 - t, 2.0f) * p_1 +
                 3 * std::pow(t, 2.0f) * (1 - t) * p_2 + std::pow(t, 3.0f) * p_3;

    return point;
}

math::Vec2 GeometryScene::recursive_bezier(float t)
{
    // TODO: Implement de Casteljau's algorithm

    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    auto p11 = glm::mix(p_0, p_1, t);
    auto p12 = glm::mix(p_1, p_2, t);
    auto p13 = glm::mix(p_2, p_3, t);

    auto p21 = glm::mix(p11, p12, t);
    auto p22 = glm::mix(p12, p13, t);

    auto p3 = glm::mix(p21, p22, t);

    return p3;
}

void GeometryScene::bezier(bool native)
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's

    auto calVertex = &GeometryScene::recursive_bezier;
    if (native)
    {
        calVertex = &GeometryScene::naive_bezier;
    }

    std::vector<math::Vec2> vertices;
    vertices.reserve(int(1.0f/this->step) + 1);

    for (float t = 0.0f; t <= 1.0f; t += this->step)
    {
        vertices.push_back((this->*calVertex)(t));
    }
    vertices.push_back((this->*calVertex)(1.0));

    glBindVertexArray(vao);

    if (this->vbo != 0)
    {
        glDeleteBuffers(1, &vbo);
    }
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(math::Vec2), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*) nullptr);
    glEnableVertexAttribArray(0);

    this->vertexCount = (int)vertices.size();
}

void GeometryScene::updatePoints()
{

    glBindVertexArray(ctrlVao);

    if (this->ctrlVbo != 0)
    {
        glDeleteBuffers(1, &ctrlVbo);
    }
    glGenBuffers(1, &ctrlVbo);
    glBindBuffer(GL_ARRAY_BUFFER, ctrlVbo);
    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(math::Vec2), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*) nullptr);
    glEnableVertexAttribArray(0);
}

}
