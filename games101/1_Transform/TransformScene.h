//
// Created by Yun on 2022/10/1.
//
#pragma once
#include "Define.h"
#include "camera/Camera2D.h"
#include "BaseScene.h"
#include "rasterizer.hpp"
#include "Triangle.hpp"


class TransformScene : public BaseScene
{
public:
    static constexpr const char* ID = "01 Transform";

    static SceneRef create();

    ~TransformScene() override;
    TransformScene(const TransformScene&) = delete;
    TransformScene& operator=(const TransformScene&) = delete;

private:

    math::Mat4 get_view_matrix(const math::Vec3& eye_pos);
    math::Mat4 get_model_matrix(float rotation_angle);
    math::Mat4 get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar);

    TransformScene();

    void draw() override;
    void reset() override;
    void drawSettings() override;

    const int BufferWidth = 700;
    const int BufferHeight = 700;
    Game101_HW1::rst::rasterizer rasterizer;
    Game101_HW1::rst::pos_buf_id pos_id{};
    Game101_HW1::rst::ind_buf_id ind_id{};
    math::Vec3 eye_pos{};
    float angle{0.0f};
    math::Vec3 rotateAxis{0.0f, 0.0f, 1.0f};

    float rotation = 0;
};
