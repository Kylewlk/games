//
// Created by Yun on 2022/10/1.
//
#include "TransformScene.h"
#include "common/Texture.h"

using namespace Game101_HW1;


TransformScene::TransformScene()
    : BaseScene(ID, 0, 0), rasterizer(BufferWidth, BufferHeight)
{
    this->texture = Texture::create(GL_RGB32F, BufferWidth, BufferHeight);

    rasterizer.clear(rst::Buffers::Color);

    eye_pos = {0, 0, 5};

    std::vector<math::Vec3> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<math::Vec3i> ind{{0, 1, 2}};

    pos_id = rasterizer.load_positions(pos);
    ind_id = rasterizer.load_indices(ind);

    TransformScene::reset();
}

TransformScene::~TransformScene()
{
}

SceneRef TransformScene::create()
{
    struct enable_make_shared : public TransformScene
    {
        enable_make_shared() = default;
    };
    return std::make_shared<enable_make_shared>();
}


math::Mat4 TransformScene::get_view_matrix(const math::Vec3& eye_pos)
{
    math::Mat4 view{1.0f};

    math::Mat4 translate{1.0f};
    translate = math::translate(-eye_pos);

    view = translate * view;

    return view;
}

math::Mat4 TransformScene::get_model_matrix(float rotation_angle)
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

math::Mat4 TransformScene::get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    // Students will implement this function

    math::Mat4 projection {1.0f};

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    projection = glm::perspective(eye_fov, aspect_ratio, zNear, zFar);

    return projection;
}

void TransformScene::reset()
{
    this->angle = 0.0f;
    this->rotateAxis = {0.0f, 0.0f, 1.0f};
    BaseScene::reset();
}

void TransformScene::draw()
{
    rasterizer.clear(rst::Buffers::Color | rst::Buffers::Depth);
    rasterizer.set_model(get_model_matrix(angle));
    rasterizer.set_view(get_view_matrix(eye_pos));
    rasterizer.set_projection(get_projection_matrix(45, 1, 0.1, 50));
    rasterizer.draw(pos_id, ind_id, rst::Primitive::Triangle);

    this->texture->update(0, 0, BufferWidth, BufferHeight, GL_RGB, GL_FLOAT, rasterizer.frame_buffer().data());

    BaseScene::draw();
}

void TransformScene::drawSettings()
{
    ImGui::DragFloat("Angle", &this->angle, 0.5f);
    ImGui::DragFloat3("Axis", (float*)&this->rotateAxis);
}
