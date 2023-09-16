//
// Created by LEI XU on 4/11/19.
//

#pragma once

#include <Define.h>
#include "common/MathHelp.h"
#include "Texture.hpp"

namespace Game101_HW3
{

class Triangle{

public:
    math::Vec4 v[3]{}; /*the original coordinates of the triangle, v0, v1, v2 in counter clockwise order*/
    /*Per vertex values*/
    math::Vec3 color[3]{}; //color at each vertex;
    math::Vec2 tex_coords[3]{}; //texture u,v
    math::Vec3 normal[3]{}; //normal vector for each vertex

    Texture *tex= nullptr;
    Triangle();

    math::Vec4 a() const { return v[0]; }
    math::Vec4 b() const { return v[1]; }
    math::Vec4 c() const { return v[2]; }

    void setVertex(int ind, math::Vec4 ver); /*set i-th vertex coordinates */
    void setNormal(int ind, math::Vec3 n); /*set i-th vertex normal vector*/
    void setColor(int ind, float r, float g, float b); /*set i-th vertex color*/

    void setNormals(const std::array<math::Vec3, 3>& normals);
    void setColors(const std::array<math::Vec3, 3>& colors);
    void setTexCoord(int ind,math::Vec2 uv ); /*set i-th vertex texture coordinate*/
    std::array<math::Vec4, 3> toVector4() const;
};

}
