//
// Created by Yun on 2022/10/1.
//
#include "ShadingScene.h"
#include "OBJ_Loader.h"
#include "common/Texture.h"

namespace Game101_HW3
{

ShadingScene::ShadingScene()
        : BaseScene(ID, 0, 0), rasterizer(BufferWidth, BufferHeight),
      textureHeight("asset/model/spot/hmap.jpg"),
      texture("asset/model/spot/spot_texture.png")
{
    this->init();

    ShadingScene::reset();
}

ShadingScene::~ShadingScene()
{
    for (auto t : TriangleList)
    {
        delete t;
    }
}

SceneRef ShadingScene::create()
{
    struct enable_make_shared : public ShadingScene
    {
        enable_make_shared() = default;
    };
    return std::make_shared<enable_make_shared>();
}

math::Mat4 ShadingScene::get_view_matrix(const math::Vec3& pos)
{
    math::Mat4 view{1.0f};

    math::Mat4 translate{1.0f};
    translate = math::translate(-pos);

    view = translate * view;

    return view;
}

math::Mat4 ShadingScene::get_model_matrix(float rotation_angle)
{
    math::Mat4 model {1.0f};

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    model = glm::scale(model, {2.5f, 2.5f, 2.5f});

    rotation_angle = glm::radians(rotation_angle);
    model = glm::rotate(model, rotation_angle, rotateAxis);

    return model;
}

math::Mat4 ShadingScene::get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    // Students will implement this function

    math::Mat4 projection {1.0f};

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    projection = glm::perspective(eye_fov, aspect_ratio, zNear, zFar);

    return projection;
}

math::Vec3 ShadingScene::vertex_shader(const vertex_shader_payload& payload)
{
    return payload.position;
}

math::Vec3 ShadingScene::normal_fragment_shader(const fragment_shader_payload& payload)
{
    math::Vec3 return_color = (glm::normalize(payload.normal) + math::Vec3(1.0f, 1.0f, 1.0f)) / 2.f;

    return {return_color.x, return_color.y, return_color.z};
}

math::Vec3 ShadingScene::reflect(const math::Vec3& vec, const math::Vec3& axis)
{
    auto costheta = glm::dot(vec, axis);
    return glm::normalize(2 * costheta * axis - vec);
}

math::Vec3 ShadingScene::texture_fragment_shader(const fragment_shader_payload& payload)
{
    math::Vec3 return_color = {0, 0, 0};
    if (payload.texture)
    {
        // TODO: Get the texture value at the texture coordinates of the current fragment
        if (this->biLinear)
        {
            return_color = payload.texture->getColorBilinear(payload.tex_coords.x, payload.tex_coords.y);
        }
        else
        {
            return_color = payload.texture->getColor(payload.tex_coords.x, payload.tex_coords.y);
        }
    }
    math::Vec3 texture_color{return_color.x, return_color.y, return_color.z};

    math::Vec3 ka = math::Vec3(0.005, 0.005, 0.005);
    math::Vec3 kd = texture_color / 255.f;
    math::Vec3 ks = math::Vec3(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    math::Vec3 amb_light_intensity{10, 10, 10};
//    math::Vec3 eye_pos{0, 0, 10};

    float p = 150;

    math::Vec3 color = texture_color;
    math::Vec3 point = payload.view_pos;
    math::Vec3 normal = payload.normal;

    math::Vec3 result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.

        auto distance = glm::distance(light.position, point);
        auto attenuation = light.intensity / (distance * distance);
        auto lightDir = glm::normalize(light.position - point);
        auto viewDir = glm::normalize(-point);
        auto halfDir = glm::normalize(lightDir + viewDir);

        auto diffuse = kd * attenuation * glm::max(0.0f, glm::dot(normal, lightDir));
        auto specular = ks * attenuation * glm::pow(glm::max(0.0f, glm::dot(normal, halfDir)), p) ;

        result_color += diffuse + specular;
    }
    auto ambient = ka * amb_light_intensity;
    result_color += ambient;

    return result_color;
}

math::Vec3 ShadingScene::phong_fragment_shader(const fragment_shader_payload& payload)
{
    math::Vec3 ka = math::Vec3(0.005, 0.005, 0.005);
    math::Vec3 kd = payload.color;
    math::Vec3 ks = math::Vec3(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    math::Vec3 amb_light_intensity{10, 10, 10};
//    math::Vec3 eye_pos{0, 0, 10};

    float p = 150;

    math::Vec3 color = payload.color;
    math::Vec3 point = payload.view_pos;
    math::Vec3 normal = payload.normal;

    math::Vec3 result_color = {0, 0, 0};
    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.

        auto distance = glm::distance(light.position, point);
        auto attenuation = light.intensity / (distance * distance);
        auto lightDir = glm::normalize(light.position - point);
        auto viewDir = glm::normalize(-point);
        auto halfDir = glm::normalize(lightDir + viewDir);

        auto diffuse = kd * attenuation * glm::max(0.0f, glm::dot(normal, lightDir));
        auto specular = ks * attenuation * glm::pow(glm::max(0.0f, glm::dot(normal, halfDir)), p) ;

        result_color += diffuse + specular;
    }
    auto ambient = ka * amb_light_intensity;
    result_color += ambient;

    return result_color;
}

math::Vec3 ShadingScene::displacement_fragment_shader(const fragment_shader_payload& payload)
{
    
    math::Vec3 ka = math::Vec3(0.005, 0.005, 0.005);
    math::Vec3 kd = payload.color;
    math::Vec3 ks = math::Vec3(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    math::Vec3 amb_light_intensity{10, 10, 10};
    math::Vec3 eye_pos{0, 0, 10};

    float p = 150;

    math::Vec3 color = payload.color; 
    math::Vec3 point = payload.view_pos;
    math::Vec3 normal = payload.normal;

    float kh = 0.2, kn = 0.1;
    
    // TODO: Implement displacement mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Position p = p + kn * n * h(u,v)
    // Normal n = normalize(TBN * ln)

    auto x = normal.x;
    auto y = normal.y;
    auto z = normal.z;
    auto n = normal;
    auto t = math::Vec3{x*y/std::sqrt(x*x+z*z), std::sqrt(x*x+z*z), z*y/std::sqrt(x*x+z*z)};
    t = glm::normalize(t);
    auto b = glm::cross(n, t);
    math::Mat3 TBN {t, b, n};

    auto stepW = 1.0f / payload.texture->width;
    auto stepH = 1.0f / payload.texture->height;
    auto u = payload.tex_coords.x;
    auto v = payload.tex_coords.y;
    auto h = [payload, this](float u, float v)
    {
        if (this->biLinear)
        {
            return glm::length(payload.texture->getColorBilinear(u,v));
        }
        else
        {
            return glm::length(payload.texture->getColor(u,v));
        }
    };

    auto dU = kh * kn * (h(u+stepW,v)-h(u,v));
    auto dV = kh * kn * (h(u,v+stepH)-h(u,v));
    math::Vec3 ln = {-dU, -dV, 1};

    point = point + kn * n * h(u,v);
    normal = glm::normalize(TBN * ln);

    math::Vec3 result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.

        auto distance = glm::distance(light.position, point);
        auto attenuation = light.intensity / (distance * distance);
        auto lightDir = glm::normalize(light.position - point);
        auto viewDir = glm::normalize(-point);
        auto halfDir = glm::normalize(lightDir + viewDir);

        auto diffuse = kd * attenuation * glm::max(0.0f, glm::dot(normal, lightDir));
        auto specular = ks * attenuation * glm::pow(glm::max(0.0f, glm::dot(normal, halfDir)), p) ;

        result_color += diffuse + specular;
    }
    auto ambient = ka * amb_light_intensity;
    result_color += ambient;

    return result_color;
}

math::Vec3 ShadingScene::bump_fragment_shader(const fragment_shader_payload& payload)
{
    
    math::Vec3 ka = math::Vec3(0.005, 0.005, 0.005);
    math::Vec3 kd = payload.color;
    math::Vec3 ks = math::Vec3(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    math::Vec3 amb_light_intensity{10, 10, 10};
//    math::Vec3 eye_pos{0, 0, 10};

    float p = 150;

    math::Vec3 color = payload.color; 
    math::Vec3 point = payload.view_pos;
    math::Vec3 normal = payload.normal;

    float kh = 0.2, kn = 0.1;

    // TODO: Implement bump mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Normal n = normalize(TBN * ln)

    auto x = normal.x;
    auto y = normal.y;
    auto z = normal.z;
    auto n = normal;
    auto t = math::Vec3{x*y/std::sqrt(x*x+z*z), std::sqrt(x*x+z*z), z*y/std::sqrt(x*x+z*z)};
    t = glm::normalize(t);
    auto b = glm::cross(n, t);
    math::Mat3 TBN {t, b, n};

    auto stepW = 1.0f / payload.texture->width;
    auto stepH = 1.0f / payload.texture->height;
    auto u = payload.tex_coords.x;
    auto v = payload.tex_coords.y;
    auto h = [payload, this](float u, float v)
    {
        if (this->biLinear)
        {
            return glm::length(payload.texture->getColorBilinear(u,v));
        }
        else
        {
            return glm::length(payload.texture->getColor(u,v));
        }
    };

    auto dU = kh * kn * (h(u+stepW,v)-h(u,v));
    auto dV = kh * kn * (h(u,v+stepH)-h(u,v));
    math::Vec3 ln = {-dU, -dV, 1};
    normal = glm::normalize(TBN * ln);

    math::Vec3 result_color = {0, 0, 0};
    result_color = normal;
    return result_color;
}


bool ShadingScene::init()
{

    BaseScene::texture = ::Texture::create(GL_RGB32F, BufferWidth, BufferHeight);

    eye_pos = {0, 0, 10};
    this->angle = 140.0f;

    objl::Loader Loader;
    // Load .obj File
    Loader.LoadFile("asset/model/spot/spot_triangulated_good.obj");

    for(auto mesh:Loader.LoadedMeshes)
    {
        for(int i=0;i<mesh.Vertices.size();i+=3)
        {
            auto* t = new Triangle();
            for(int j=0;j<3;j++)
            {
                t->setVertex(j,math::Vec4(mesh.Vertices[i+j].Position.X,mesh.Vertices[i+j].Position.Y,mesh.Vertices[i+j].Position.Z,1.0));
                t->setNormal(j,math::Vec3(mesh.Vertices[i+j].Normal.X,mesh.Vertices[i+j].Normal.Y,mesh.Vertices[i+j].Normal.Z));
                t->setTexCoord(j,math::Vec2(mesh.Vertices[i+j].TextureCoordinate.X, mesh.Vertices[i+j].TextureCoordinate.Y));
            }
            TriangleList.push_back(t);
        }
    }

    rasterizer.set_texture(&textureHeight);

    rasterizer.set_vertex_shader( [this](const auto& payload) { return this->vertex_shader(payload); });
    rasterizer.set_fragment_shader( [this](const auto& payload) { return this->phong_fragment_shader(payload); });

    this->currentType = 2;
    this->shadingType = 2;
    update_tex();

    return true;
}

void ShadingScene::update_tex()
{
    this->currentType = shadingType;

    rasterizer.clear(rst::Buffers::Color | rst::Buffers::Depth);

    rasterizer.set_model(get_model_matrix(angle));
    rasterizer.set_view(get_view_matrix(eye_pos));
    rasterizer.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

    //r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
    rasterizer.draw(TriangleList);

    BaseScene::texture->update(0, 0, BufferWidth, BufferHeight, GL_RGB, GL_FLOAT, rasterizer.frame_buffer().data());
}


void ShadingScene::reset()
{
    this->angle = 140.0f;
    this->rotateAxis = {0.0f, 1.0f, 0.0f};
    this->update_tex();

    BaseScene::reset();
}


void ShadingScene::draw()
{

    BaseScene::draw();
}

void ShadingScene::drawSettings()
{
    bool needUpdate{false};
    needUpdate = ImGui::DragFloat("Angle", &this->angle, 0.5f);
    needUpdate = ImGui::DragFloat3("Axis", (float*)&this->rotateAxis) || needUpdate;

    ImGui::Separator();
    ImGui::RadioButton("Texture", &this->shadingType, 0);
    ImGui::RadioButton("Normal", &this->shadingType, 1);
    ImGui::RadioButton("Phong", &this->shadingType, 2);
    ImGui::RadioButton("bump", &this->shadingType, 3);
    ImGui::RadioButton("displacement", &this->shadingType, 4);
    if (ImGui::Checkbox("Bilinear", & this->biLinear))
    {
        needUpdate = true;
    }

    if (this->currentType != this->shadingType)
    {
        if (this->shadingType == 0)
        {
            rasterizer.set_texture(&texture);
            rasterizer.set_fragment_shader( [this](const auto& payload) { return this->texture_fragment_shader(payload); });
        }
        else if(this->shadingType == 1)
        {
            rasterizer.set_texture(&textureHeight);
            rasterizer.set_fragment_shader( [this](const auto& payload) { return this->normal_fragment_shader(payload); });
        }
        else if(this->shadingType == 2)
        {
            rasterizer.set_texture(&textureHeight);
            rasterizer.set_fragment_shader( [this](const auto& payload) { return this->phong_fragment_shader(payload); });
        }
        else if(this->shadingType == 3)
        {
            rasterizer.set_texture(&textureHeight);
            rasterizer.set_fragment_shader( [this](const auto& payload) { return this->bump_fragment_shader(payload); });
        }
        else if(this->shadingType == 4)
        {
            rasterizer.set_texture(&textureHeight);
            rasterizer.set_fragment_shader( [this](const auto& payload) { return this->displacement_fragment_shader(payload); });
        }

        update_tex();
        this->currentType = shadingType;

        needUpdate = false;
    }

    if (needUpdate)
    {
        this->update_tex();
    }
}

}