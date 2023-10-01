//
// Created by DELL on 2022/3/3.
//
#pragma once
#include "Define.h"
#include "camera/Camera3D.h"
#include "BaseScene.h"

class ShadowScene : public BaseScene
{
public:
    static constexpr const char* ID = "Shadow";

    static SceneRef create();

    ~ShadowScene() override;
    ShadowScene(const ShadowScene&) = delete;
    ShadowScene& operator=(const ShadowScene&) = delete;


private:
    ShadowScene();
    bool init();

    void draw() override;
    void reset() override;
    void drawSettings() override;

    ModelRef model;
    ModelRef modelPlane;
    TextureRef texture;
    ShaderRef shaderLight;

    int showType = 1;
    math::Vec3 color{0.6f, 0.6f, 0.6f};
    math::Vec3 diffuse{0.8f, 0.8f, 0.8f};
    math::Vec3 specular{0.1f, 0.1f, 0.1f};

    math::Vec3 lightPosition{0.0f, 5.0f, 3.0f};
    math::Vec3 lightColor{1.0, 1.0, 1.0};
    float lightIntensity{1.0f} ;
    bool lightMove{true};
    bool useTexture{true};
    bool halfLambert{false};
};
