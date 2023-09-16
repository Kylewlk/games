//
// Created by LEI XU on 4/11/19.
//

#include "Triangle.hpp"
#include <algorithm>
#include <array>

namespace Game101_HW3
{

Triangle::Triangle()
{

}

void Triangle::setVertex(int ind, math::Vec4 ver){
    v[ind] = ver;
}
void Triangle::setNormal(int ind, math::Vec3 n){
    normal[ind] = n;
}
void Triangle::setColor(int ind, float r, float g, float b) {
    if((r<0.0) || (r>255.) ||
       (g<0.0) || (g>255.) ||
       (b<0.0) || (b>255.)) {
        fprintf(stderr, "ERROR! Invalid color values");
        fflush(stderr);
        exit(-1);
    }

    color[ind] = math::Vec3((float)r/255.,(float)g/255.,(float)b/255.);
}
void Triangle::setTexCoord(int ind, math::Vec2 uv) {
    tex_coords[ind] = uv;
}

std::array<math::Vec4, 3> Triangle::toVector4() const
{
    std::array<math::Vec4, 3> res{};
    std::transform(std::begin(v), std::end(v), res.begin(),
                   [](auto& vec) { return math::Vec4(vec.x, vec.y, vec.z, 1.f); });
    return res;
}

void Triangle::setNormals(const std::array<math::Vec3, 3>& normals)
{
    normal[0] = normals[0];
    normal[1] = normals[1];
    normal[2] = normals[2];
}

void Triangle::setColors(const std::array<math::Vec3, 3>& colors)
{
    auto first_color = colors[0];
    setColor(0, colors[0][0], colors[0][1], colors[0][2]);
    setColor(1, colors[1][0], colors[1][1], colors[1][2]);
    setColor(2, colors[2][0], colors[2][1], colors[2][2]);
}

}