//
// Created by DELL on 2022/3/3.
//

#include "ShadowScene.h"
#include "common/Model.h"
#include "common/Texture.h"
#include "common/Shader.h"
#include "common/TimeSys.h"
#include "common/FrameBuffer.h"
#include "camera/Camera2D.h"

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
    this->cameraLight = Camera3D::create();
    this->camera2d = Camera2D::create();

    this->model = Model::create("asset/model/mary/Marry.obj");
    this->texture = Texture::create("asset/model/mary/MC003_Kozakura_Mari.png", true, false);
    this->modelPlane = Model::create("asset/model/plane.obj");
    this->shaderLight = Shader::createByPath("asset/shader/model.vert", "asset/shader/modelPointLight.frag");
    this->shader2d = Shader::createByPath("asset/shader/picture.vert", "asset/shader/picture.frag");
    this->shaderShadowMap = Shader::createByPath("games202/1_Shadow/shadow_map.vert", "games202/1_Shadow/shadow_map.frag");
    this->shaderShowShadowMap = Shader::createByPath("asset/shader/picture.vert", "games202/1_Shadow/show_shadow_map.frag");
    this->shaderShadow = Shader::createByPath("games202/1_Shadow/shadow.vert", "games202/1_Shadow/shadow.frag");

    this->shadowMap = FrameBuffer::create(shadowMapResolution, shadowMapResolution, RenderTarget::kTexColor, RenderTarget::kTexDepth);

    return true;
}

void ShadowScene::reset()
{
    camera->lookAt({3.0, 8.0, 15.0}, {0.0f, 0.0f, 0.0f});
    camera2d->resetView();

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

    this->shadowType = 3;
    this->depthBias = 0.1f;
    this->sampleDistribution = 0;
    this->samplerFilterSize = 15;
}

void ShadowScene::genShadowMap()
{
    shadowMap->bind();
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    shaderShadowMap->use();
    shaderShadowMap->setUniform("viewProj", cameraLight->getViewProj());

    if (this->showType == 4) // draw Depth Z
    {
        shaderShadowMap->setUniform("uUseDistance", false);
    }
    else
    {
        shaderShadowMap->setUniform("uUseDistance", true);
        shaderShadowMap->setUniform("uLightPos", lightPosition);
        shaderShadowMap->setUniform("uMaxDistance", maxLightDistance);
    }

    shaderShadowMap->setUniform("model", modelMat);
    this->model->draw();

    shaderShadowMap->setUniform("model", planeMat);
    this->modelPlane->draw();

    shadowMap->unbind();
}

void ShadowScene::drawScene()
{
    genShadowMap();

    shaderShadow->use();
    shaderShadow->setUniform("viewProj", camera->getViewProj());
    shaderShadow->setUniform("lightViewProj", cameraLight->getViewProj());

    shaderShadow->setUniform("uKd", diffuse);
    shaderShadow->setUniform("uKs", specular);
    shaderShadow->setUniform("uLightPos", lightPosition);
    shaderShadow->setUniform("uLightColor", lightColor);
    shaderShadow->setUniform("uLightIntensity", lightIntensity);
    shaderShadow->setUniform("uCameraPos", camera->getViewPosition());
    shaderShadow->setUniform("uHalfLambert", halfLambert);

    shaderShadow->bindTexture("uShadowMap", shadowMap->getColor());
    shaderShadow->setUniform("uShadowMapInvSize", math::Vec2{1.0f/shadowMapResolution});
    shaderShadow->setUniform("uMaxLightDistance", maxLightDistance);
    shaderShadow->setUniform("uBias", depthBias);
    shaderShadow->setUniform("uShadowType", shadowType);
    shaderShadow->setUniform("uSampleDistribution", sampleDistribution);
    shaderShadow->setUniform("uSamplerFilterSize", samplerFilterSize);

    auto normalMat = glm::transpose(glm::inverse(math::Mat3{modelMat}));
    shaderShadow->setUniform("model", modelMat);
    shaderShadow->setUniform("normalMatrix", normalMat);
    shaderShadow->bindTexture("uSampler", this->texture);
    shaderShadow->setUniform("uUseTexture", useTexture);
    this->model->draw();

    normalMat = glm::transpose(glm::inverse(math::Mat3{planeMat}));
    shaderShadow->setUniform("model", planeMat);
    shaderShadow->setUniform("normalMatrix", normalMat);
    shaderShadow->setUniform("uUseTexture", false);
    this->modelPlane->draw();

    drawAxis3D(camera->getViewProj(), 1, 2);
    drawPoints(&lightPosition, 1, camera->getViewProj(), {lightColor, 1.0}, 5);
}

void ShadowScene::drawLightView()
{
    shadowMap->bind();
    glClearColor(0.3f, 0.3f, 0.6f, 1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    shaderLight->use();
    shaderLight->setUniform("viewProj", cameraLight->getViewProj());

    shaderLight->setUniform("color", color);
    shaderLight->setUniform("kd", diffuse);
    shaderLight->setUniform("ks", specular);
    shaderLight->setUniform("lightPosition", lightPosition);
    shaderLight->setUniform("lightColor", lightColor);
    shaderLight->setUniform("lightIntensity", lightIntensity);
    shaderLight->setUniform("halfLambert", halfLambert);
    shaderLight->setUniform("cameraPosition", cameraLight->getViewPosition());

    auto normalMat = glm::transpose(glm::inverse(math::Mat3{modelMat}));
    shaderLight->setUniform("model", modelMat);
    shaderLight->setUniform("normalMatrix", normalMat);
    shaderLight->bindTexture("tex", this->texture);
    shaderLight->setUniform("useTexture", useTexture);
    this->model->draw();

    normalMat = glm::transpose(glm::inverse(math::Mat3{planeMat}));
    shaderLight->setUniform("model", planeMat);
    shaderLight->setUniform("normalMatrix", normalMat);
    shaderLight->setUniform("useTexture", false);
    this->modelPlane->draw();

    shadowMap->unbind();

    auto size = shadowMapResolution * 0.5f;
    // Y 轴乘以 -1 是为了让 纹理坐标的原点在左上角，Y 轴向下
    auto mat = camera2d->getViewProj() * math::scale({size, size * -1.f, 1.0f});
    shader2d->use();
    shader2d->setUniform("mvp", mat);
    shader2d->setUniform("color", math::Vec4{1, 1, 1, 1});
    shader2d->bindTexture(3, this->shadowMap->getColor());
    drawQuad();
}

void ShadowScene::drawShadowMap()
{
    genShadowMap();

    auto size = shadowMapResolution * 0.5f;
    // Y 轴乘以 -1 是为了让 纹理坐标的原点在左上角，Y 轴向下
    auto mat = camera2d->getViewProj() * math::scale({size, size * -1.f, 1.0f});

    shaderShowShadowMap->use();
    shaderShowShadowMap->setUniform("mvp", mat);
    shaderShowShadowMap->bindTexture("tex", this->shadowMap->getColor());
    drawQuad();
}

void ShadowScene::draw()
{
    BaseScene::draw();
    this->camera2d->setViewSize((float)this->width, (float)this->height);
    this->camera2d->update();

    this->cameraLight->setFov(glm::radians(120.f));
    this->cameraLight->setViewSize(shadowMapResolution, shadowMapResolution);
    this->cameraLight->lookAt(lightPosition, {0, 0, 0});
    this->cameraLight->update();

    modelMat = math::translate({0, 0.0f, 0});
    planeMat = math::scale({100, 100, 100});

    if(this->lightMove)
    {
        static auto timer = 0.0f;
        timer += static_cast<float>(TimeSys::getDelta() * 0.2);
        timer = std::fmod(timer, math::pi_2);
        lightPosition.x = std::sin(timer*6.0f) * 1.5f;
        lightPosition.y = std::sin(timer*4.0f) * 1.0f + 5.0f;
        lightPosition.z = std::sin(timer*2.0f) * 1.0f + 4.f;
    }

    if (showType == 1)
    {
        drawScene();
    }
    else if (showType == 2)
    {
        drawLightView();
    }
    else
    {
        drawShadowMap();
    }
}

void ShadowScene::drawSettings()
{
    ImGui::RadioButton("Scene", &showType, 1);
    ImGui::RadioButton("Light View", &showType, 2);
    ImGui::RadioButton("Depth Distance", &showType, 3);
    ImGui::RadioButton("Depth Z", &showType, 4);

    ImGui::Separator();
    ImGui::Checkbox("Use Texture", &useTexture);
    ImGui::ColorEdit3("Color", (float*)&color, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Diffuse", (float*)&diffuse);
    ImGui::ColorEdit3("Specular", (float*)&specular);

    ImGui::Separator();
    ImGui::Checkbox("Move Light", &this->lightMove);
    ImGui::ColorEdit3("Light Color", (float*)&lightColor, ImGuiColorEditFlags_Float);
    ImGui::DragFloat3("Light Position", (float*)&lightPosition, 0.01f);
    ImGui::SliderFloat("Light Intensity", (float*)&lightIntensity, 0.1f, 4.0f);
    ImGui::Checkbox("Half Lambert", &halfLambert);
    ImGui::SliderFloat("Bias", &depthBias, 0.0f, 3.0f);

    if(this->showType == 1)
    {
        ImGui::Separator();
        ImGui::RadioButton("ShadowMap", &shadowType, 1);
        ImGui::SameLine();
        ImGui::RadioButton("PCF", &shadowType, 2);
        ImGui::SameLine();
        ImGui::RadioButton("PCSS", &shadowType, 3);

        ImGui::Separator();
        ImGui::SliderInt("Filter Size", &samplerFilterSize, 1, 100);

        ImGui::RadioButton("PoissonDisk", &sampleDistribution, 0);
        ImGui::SameLine();
        ImGui::RadioButton("UniformDisk", &sampleDistribution, 1);
        ImGui::SameLine();
        ImGui::RadioButton("ALL", &sampleDistribution, 2);
    }
}

void ShadowScene::onMouseEvent(const MouseEvent* e)
{
    if (this->showType == 1)
    {
        BaseScene::onMouseEvent(e);
        return;
    }

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
        float scale = this->camera2d->getViewScale();
        scale = (e->scrollY > 0) ? scale * 0.8f : scale * 1.25f;
        this->camera2d->setViewScale(scale);
    }
    else if (e->mouseEventType == MouseEvent::kMouseMove)
    {
        if (this->holdLeftButton)
        {
            auto delta = e->posDelta;
            delta *= this->camera2d->getViewScale();
            this->camera2d->move({delta.x, -delta.y, 0});
        }
    }
}
