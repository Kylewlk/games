//
// Created by LEI XU on 4/11/19.
//

#ifndef RASTERIZER_TRIANGLE_H2
#define RASTERIZER_TRIANGLE_H2

#include <Define.h>
#include "common/MathHelp.h"

namespace Game101_HW2
{
class Triangle
{

public:
    math::Vec3 v[3]; /*the original coordinates of the triangle, v0, v1, v2 in counter clockwise order*/
    /*Per vertex values*/
    math::Vec3 color[3];      //color at each vertex;
    math::Vec2 tex_coords[3]; //texture u,v
    math::Vec3 normal[3];     //normal vector for each vertex

    //Texture *tex;
    Triangle();

    void setVertex(int ind, math::Vec3 ver);                  /*set i-th vertex coordinates */
    void setNormal(int ind, math::Vec3 n);                    /*set i-th vertex normal vector*/
    void setColor(int ind, float r, float g, float b);        /*set i-th vertex color*/
    math::Vec3 getColor() const { return color[0] * 255.0f; } // Only one color per triangle.
    void setTexCoord(int ind, float s, float t);              /*set i-th vertex texture coordinate*/
    std::array<math::Vec4, 3> toVector4() const;
};
}


#endif //RASTERIZER_TRIANGLE_H
