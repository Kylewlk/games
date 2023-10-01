//
// Created by DELL on 2022/3/3.
//

#include "ShadowScene.h"
#include "common/Model.h"
#include "common/Texture.h"
#include "common/Shader.h"
#include "common/TimeSys.h"

ShadowScene::ShadowScene()
    : BaseScene(ID, 0, 0)
{
    this->init();
    ShadowScene::reset();
}

ShadowScene::~ShadowScene()
= default;

SceneRef ShadowScene::create()
{
    struct enable_make_shared : public ShadowScene
    {
        enable_make_shared() = default;
    };
    return std::make_shared<enable_make_shared>();
}

bool ShadowScene::init()
{
    this->model = Model::create("asset/model/mary/Marry.obj");
    this->texture = Texture::create("asset/model/mary/MC003_Kozakura_Mari.png", true, false);
    this->modelPlane = Model::create("asset/model/plane.obj");
    this->shaderLight = Shader::createByPath("asset/shader/model.vert", "asset/shader/modelPointLight.frag");

    return true;
}

void ShadowScene::reset()
{
    camera->lookAt({3.0, 8.0, 15.0}, {0.0f, 0.0f, 0.0f});

    this->lightMove = true;

    this->color = {0.6f, 0.6f, 0.6f};
    this->diffuse = {0.8f, 0.8f, 0.8f};
    this->specular = {0.1f, 0.1f, 0.1f};

    this->lightPosition = {0.0f, 5.0f, 3.0f};
    this->lightColor = {1.0, 1.0, 1.0};
    this->lightIntensity = 1.0f;
    this->lightMove = true;
    this->useTexture = true;
    this->halfLambert = false;
}

void ShadowScene::draw()
{
    BaseScene::draw();
    //    glEnable(GL_CULL_FACE);
    auto mat = math::translate({0, 0.0f, 0});
    auto normalMat = glm::transpose(glm::inverse(math::Mat3{mat}));

    if(this->lightMove)
    {
        static auto timer = 0.0f;
        timer += static_cast<float>(TimeSys::getDelta() * 0.2);
        timer = std::fmod(timer, math::pi_2);
        lightPosition.x = std::sin(timer*6.0f) * 1.5f;
        lightPosition.y = std::sin(timer*4.0f) * 1.0f + 5.0f;
        lightPosition.z = std::sin(timer*2.0f) * 1.5f + 3.f;
    }

    shaderLight->use();
    shaderLight->setUniform("viewProj", camera->getViewProj());
    shaderLight->setUniform("normalMatrix", normalMat);

    shaderLight->setUniform("color", color);
    shaderLight->setUniform("kd", diffuse);
    shaderLight->setUniform("ks", specular);
    shaderLight->setUniform("lightPosition", lightPosition);
    shaderLight->setUniform("lightColor", lightColor);
    shaderLight->setUniform("lightIntensity", lightIntensity);
    shaderLight->setUniform("halfLambert", halfLambert);
    shaderLight->setUniform("cameraPosition", camera->getViewPosition());

    shaderLight->setUniform("model", mat);
    shaderLight->bindTexture("tex", this->texture);
    shaderLight->setUniform("useTexture", useTexture);
    this->model->draw();

    mat = math::scale({100, 100, 100});
    shaderLight->setUniform("model", mat);
    shaderLight->setUniform("useTexture", false);
    this->modelPlane->draw();


    drawAxis3D(camera->getViewProj(), 1, 2);
    drawPoints(&lightPosition, 1, camera->getViewProj(), {lightColor, 1.0}, 5);
}

void ShadowScene::drawSettings()
{
    ImGui::RadioButton("Unlit", &showType, 0);
    ImGui::SameLine(0, 5);
    ImGui::RadioButton("Point Light", &showType, 1);
    ImGui::Separator();

    if (showType == 0)
    {
        ImGui::Checkbox("Use Texture", &useTexture);
        ImGui::ColorEdit3("Color", (float*)&color, ImGuiColorEditFlags_Float);
    }
    else
    {
        ImGui::Checkbox("Use Texture", &useTexture);
        ImGui::ColorEdit3("Color", (float*)&color, ImGuiColorEditFlags_Float);
        ImGui::ColorEdit3("Diffuse", (float*)&diffuse);
        ImGui::ColorEdit3("Specular", (float*)&specular);

        ImGui::Checkbox("Move Light", &this->lightMove);
        ImGui::ColorEdit3("Light Color", (float*)&lightColor, ImGuiColorEditFlags_Float);
        ImGui::DragFloat3("Light Position", (float*)&lightPosition, 0.01f);
        ImGui::SliderFloat("Light Intensity", (float*)&lightIntensity, 0.1f, 4.0f);
        ImGui::Checkbox("Half Lambert", &halfLambert);
    }
}

