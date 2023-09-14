//
// Created by Yun on 2022/10/1.
//
#include "RasterizerScene.h"

using namespace Game101_HW2;
#include "common/Texture.h"

RasterizerScene::RasterizerScene()
    : BaseScene(ID, 0, 0), rasterizer(BufferWidth, BufferHeight)
{
    rasterizer.clear(rst::Buffers::Color);

    eye_pos = {0, 0, 9};

    std::vector<math::Vec3> pos
        {
            {2, 0, -2},
            {0, 2, -2},
            {-2, 0, -2},
            {3.5, -1, -5},
            {2.5, 1.5, -5},
            {-1, 0.5, -5}
        };

    std::vector<math::Vec3i> ind
        {
            {0, 1, 2},
            {3, 4, 5}
        };

    std::vector<math::Vec3> cols
        {
            {217.0, 238.0, 185.0},
            {217.0, 0.0, 185.0},
            {0.0, 238.0, 185.0},
            {185.0, 217.0, 238.0},
            {185.0, 217.0, 238.0},
            {185.0, 217.0, 238.0}
        };

    pos_id = rasterizer.load_positions(pos);
    ind_id = rasterizer.load_indices(ind);
    col_id = rasterizer.load_colors(cols);

    texture = Texture::create(GL_RGB32F, BufferWidth, BufferHeight);
    texture->setSampler(GL_NEAREST);

    RasterizerScene::reset();
}

RasterizerScene::~RasterizerScene()
{
}

SceneRef RasterizerScene::create()
{
    struct enable_make_shared : public RasterizerScene
    {
        enable_make_shared() = default;
    };
    return std::make_shared<enable_make_shared>();
}

math::Mat4 RasterizerScene::get_view_matrix(const math::Vec3& eye_pos)
{
    math::Mat4 view{1.0f};

    math::Mat4 translate{1.0f};
    translate = math::translate(-eye_pos);

    view = translate * view;

    return view;
}

math::Mat4 RasterizerScene::get_model_matrix(float rotation_angle)
{
    math::Mat4 model {1.0f};

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.

    rotation_angle = glm::radians(rotation_angle);

//    model = glm::eulerAngleZ(rotation_angle);

    model = glm::rotate(model, rotation_angle, rotateAxis);

    return model;
}

math::Mat4 RasterizerScene::get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    // Students will implement this function

    math::Mat4 projection {1.0f};

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    projection = glm::perspective(eye_fov, aspect_ratio, zNear, zFar);

    return projection;
}


void RasterizerScene::reset()
{
    this->rasterizer.super_sampling = false;
    this->angle = 0.0f;
    this->rotateAxis = {0.0f, 1.0f, 0.0f};
    BaseScene::reset();
}


void RasterizerScene::draw()
{
    rasterizer.clear(rst::Buffers::Color | rst::Buffers::Depth);

    rasterizer.set_model(get_model_matrix(angle));
    rasterizer.set_view(get_view_matrix(eye_pos));
    rasterizer.set_projection(get_projection_matrix(45, 1, 0.1, 50));

    rasterizer.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);

    texture->update(0, 0, BufferWidth, BufferHeight, GL_RGB, GL_FLOAT, rasterizer.frame_buffer().data());


    BaseScene::draw();
}

void RasterizerScene::drawSettings()
{
    ImGui::Checkbox("Super-Sampling", &rasterizer.super_sampling);

    ImGui::DragFloat("Angle", &this->angle, 0.5f);
    ImGui::DragFloat3("Axis", (float*)&this->rotateAxis);
}

