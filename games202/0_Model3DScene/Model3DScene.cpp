//
// Created by DELL on 2022/3/3.
//

#include "Model3DScene.h"
#include "common/Model.h"
#include "common/Texture.h"
#include "common/Shader.h"
#include "common/TimeSys.h"

Model3DScene::Model3DScene()
    : BaseScene(ID, 0, 0)
{
    this->init();
    Model3DScene::reset();
}

Model3DScene::~Model3DScene()
= default;

SceneRef Model3DScene::create()
{
    struct enable_make_shared : public Model3DScene
    {
        enable_make_shared() = default;
    };
    return std::make_shared<enable_make_shared>();
}

bool Model3DScene::init()
{
    this->model = Model::create("asset/model/mary/Marry.obj");
    this->texture = Texture::create("asset/model/mary/MC003_Kozakura_Mari.png", true, false);
    this->shaderUnlit = Shader::createByPath("asset/shader/model.vert", "asset/shader/modelUnlit.frag");
    this->shaderLight = Shader::createByPath("asset/shader/model.vert", "asset/shader/modelPointLight.frag");


    return true;
}

void Model3DScene::reset()
{
    camera->lookAt({0.0, 2.0, 8.0}, {0.0f, 0.0f, 0.0f});

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

void Model3DScene::draw()
{
    BaseScene::draw();
    //    glEnable(GL_CULL_FACE);
    auto mat = math::translate({0, -1.0f, 0});
    auto normalMat = glm::transpose(glm::inverse(math::Mat3{mat}));

    if (showType == 0)
    {
        shaderUnlit->use();
        shaderUnlit->setUniform("viewProj", camera->getViewProj());
        shaderUnlit->setUniform("model", mat);
        shaderUnlit->setUniform("normalMatrix", normalMat);

        shaderUnlit->setUniform("color", color);
        shaderUnlit->setUniform("useTexture", useTexture);
        shaderUnlit->bindTexture("tex", this->texture);

        this->model->draw();
    }
    else
    {
        if(this->lightMove)
        {
            static auto timer = 0.0f;
            timer += static_cast<float>(TimeSys::getDelta() * 0.2);
            timer = std::fmod(timer, math::pi_2);
            lightPosition.x = std::sin(timer*6.0f) * 1.5f;
            lightPosition.y = std::sin(timer*4.0f) * 2.0f + 2.0f;
            lightPosition.z = std::sin(timer*2.0f) * 1.5f + 2.0f;
        }

        shaderLight->use();
        shaderLight->setUniform("viewProj", camera->getViewProj());
        shaderLight->setUniform("model", mat);
        shaderLight->setUniform("normalMatrix", normalMat);

        shaderLight->bindTexture("tex", this->texture);
        shaderLight->setUniform("useTexture", useTexture);
        shaderLight->setUniform("color", color);
        shaderLight->setUniform("kd", diffuse);
        shaderLight->setUniform("ks", specular);
        shaderLight->setUniform("lightPosition", lightPosition);
        shaderLight->setUniform("lightColor", lightColor);
        shaderLight->setUniform("lightIntensity", lightIntensity);
        shaderLight->setUniform("halfLambert", halfLambert);
        shaderLight->setUniform("cameraPosition", camera->getViewPosition());

        this->model->draw();

        drawPoints(&lightPosition, 1, camera->getViewProj(), {lightColor, 1.0}, 5);
    }
}

void Model3DScene::drawSettings()
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

