//
// Created by LEI XU on 4/27/19.
//
#pragma once

#include "Texture.hpp"
namespace Game101_HW3
{

struct fragment_shader_payload
{
    fragment_shader_payload()
    {
        texture = nullptr;
    }

    fragment_shader_payload(const math::Vec3& col, const math::Vec3& nor,const math::Vec2& tc, Texture* tex) :
         color(col), normal(nor), tex_coords(tc), texture(tex) {}


    math::Vec3 view_pos;
    math::Vec3 color;
    math::Vec3 normal;
    math::Vec2 tex_coords;
    Texture* texture;
};

struct vertex_shader_payload
{
    math::Vec3 position;
};

}
