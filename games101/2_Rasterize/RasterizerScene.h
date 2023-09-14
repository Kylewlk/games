//
// Created by Yun on 2022/10/1.
//
#pragma once
#include "Define.h"
#include "BaseScene.h"
#include "rasterizer.hpp"

class RasterizerScene;
using RasterizerSceneRef = std::shared_ptr<RasterizerScene>;

class RasterizerScene : public BaseScene
{
public:
    static constexpr const char* ID = "01 Rasterizer";

    static SceneRef create();

    ~RasterizerScene() override;
    RasterizerScene(const RasterizerScene&) = delete;
    RasterizerScene& operator=(const RasterizerScene&) = delete;

private:

    math::Mat4 get_view_matrix(const math::Vec3& eye_pos);
    math::Mat4 get_model_matrix(float rotation_angle);
    math::Mat4 get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar);

    RasterizerScene();

    void draw() override;
    void reset() override;
    void drawSettings() override;

    const int BufferWidth = 600;
    const int BufferHeight = 600;
    Game101_HW2::rst::rasterizer rasterizer;
    Game101_HW2::rst::pos_buf_id pos_id{};
    Game101_HW2::rst::ind_buf_id ind_id{};
    Game101_HW2::rst::col_buf_id col_id{};
    math::Vec3 eye_pos{};
    float angle{0.0f};
    math::Vec3 rotateAxis{0.0f, 1.0f, 0.0f};

    float rotation = 0;

};
