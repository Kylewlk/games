//
// Created by Yun on 2022/10/1.
//
#pragma once
#include "Define.h"
#include "camera/Camera2D.h"
#include "BaseScene.h"
#include "rasterizer.hpp"

namespace Game101_HW3
{

class ShadingScene : public BaseScene
{
public:
    static constexpr const char* ID = "03 Shading";

    static SceneRef create();

    ~ShadingScene() override;
    ShadingScene(const ShadingScene&) = delete;
    ShadingScene& operator=(const ShadingScene&) = delete;

private:
    struct light
    {
        math::Vec3 position;
        math::Vec3 intensity;
    };

    math::Mat4 get_view_matrix(const math::Vec3& eye_pos);
    math::Mat4 get_model_matrix(float rotation_angle);
    math::Mat4 get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar);
    math::Vec3 vertex_shader(const vertex_shader_payload& payload);
    math::Vec3 normal_fragment_shader(const fragment_shader_payload& payload);
    math::Vec3 reflect(const math::Vec3& vec, const math::Vec3& axis);
    math::Vec3 texture_fragment_shader(const fragment_shader_payload& payload);
    math::Vec3 phong_fragment_shader(const fragment_shader_payload& payload);
    math::Vec3 displacement_fragment_shader(const fragment_shader_payload& payload);
    math::Vec3 bump_fragment_shader(const fragment_shader_payload& payload);
    void update_tex();

    ShadingScene();
    bool init();

    void draw() override;
    void reset() override;
    void drawSettings() override;

    const int BufferWidth = 600;
    const int BufferHeight = 600;
    rst::rasterizer rasterizer;
    rst::pos_buf_id pos_id{};
    rst::ind_buf_id ind_id{};
    rst::col_buf_id col_id{};
    std::vector<Triangle*> TriangleList;
    math::Vec3 eye_pos{};
    float angle{0.0f};
    math::Vec3 rotateAxis{0.0f, 1.0f, 0.0f};
    int shadingType{0};
    int currentType{0};
    Game101_HW3::Texture textureHeight;
    Game101_HW3::Texture texture;

    float rotation = 0;
    bool biLinear{false};
};

}