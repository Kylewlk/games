//
// Created by LEI XU on 4/11/19.
//

#include "Triangle.hpp"
#include <algorithm>
#include <array>

namespace Game101_HW2
{

Triangle::Triangle()
{
    v[0] = {0, 0, 0};
    v[1] = {0, 0, 0};
    v[2] = {0, 0, 0};

    color[0] = {0.0, 0.0, 0.0};
    color[1] = {0.0, 0.0, 0.0};
    color[2] = {0.0, 0.0, 0.0};

    tex_coords[0] = {0.0, 0.0};
    tex_coords[1] = {0.0, 0.0};
    tex_coords[2] = {0.0, 0.0};
}

void Triangle::setVertex(int ind, math::Vec3 ver)
{
    v[ind] = ver;
}
void Triangle::setNormal(int ind, math::Vec3 n)
{
    normal[ind] = n;
}
void Triangle::setColor(int ind, float r, float g, float b)
{
    if ((r < 0.0f) || (r > 255.f) || (g < 0.0f) || (g > 255.f) || (b < 0.0f) || (b > 255.f))
    {
        fprintf(stderr, "ERROR! Invalid color values");
        fflush(stderr);
        exit(-1);
    }

    color[ind] = math::Vec3((float) r / 255., (float) g / 255., (float) b / 255.);
}
void Triangle::setTexCoord(int ind, float s, float t)
{
    tex_coords[ind] = math::Vec2(s, t);
}

std::array<math::Vec4, 3> Triangle::toVector4() const
{
    std::array<math::Vec4, 3> res{};
    std::transform(std::begin(v), std::end(v), res.begin(),
                   [](auto& vec) { return math::Vec4(vec.x, vec.y, vec.z, 1.f); });
    return res;
}

}