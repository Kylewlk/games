//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include "rasterizer.hpp"

namespace Game101_HW3
{

rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<math::Vec3> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<math::Vec3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<math::Vec3> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_normals(const std::vector<math::Vec3>& normals)
{
    auto id = get_next_id();
    nor_buf.emplace(id, normals);

    normal_id = id;

    return {id};
}


// Bresenham's line drawing algorithm
void rst::rasterizer::draw_line(math::Vec3 begin, math::Vec3 end)
{
    auto x1 = begin.x;
    auto y1 = begin.y;
    auto x2 = end.x;
    auto y2 = end.y;

    math::Vec3 line_color = {255, 255, 255};

    int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;

    dx=x2-x1;
    dy=y2-y1;
    dx1=fabs(dx);
    dy1=fabs(dy);
    px=2*dy1-dx1;
    py=2*dx1-dy1;

    if(dy1<=dx1)
    {
        if(dx>=0)
        {
            x=x1;
            y=y1;
            xe=x2;
        }
        else
        {
            x=x2;
            y=y2;
            xe=x1;
        }
        math::Vec2i point = math::Vec2i(x, y);
        set_pixel(point,line_color);
        for(i=0;x<xe;i++)
        {
            x=x+1;
            if(px<0)
            {
                px=px+2*dy1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    y=y+1;
                }
                else
                {
                    y=y-1;
                }
                px=px+2*(dy1-dx1);
            }
//            delay(0);
            math::Vec2i point = math::Vec2i(x, y);
            set_pixel(point,line_color);
        }
    }
    else
    {
        if(dy>=0)
        {
            x=x1;
            y=y1;
            ye=y2;
        }
        else
        {
            x=x2;
            y=y2;
            ye=y1;
        }
        math::Vec2i point = math::Vec2i(x, y);
        set_pixel(point,line_color);
        for(i=0;y<ye;i++)
        {
            y=y+1;
            if(py<=0)
            {
                py=py+2*dx1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    x=x+1;
                }
                else
                {
                    x=x-1;
                }
                py=py+2*(dx1-dy1);
            }
//            delay(0);
            math::Vec2i point = math::Vec2i(x, y);
            set_pixel(point,line_color);
        }
    }
}

auto to_vec4(const math::Vec3& v3, float w = 1.0f)
{
    return math::Vec4(v3.x, v3.y, v3.z, w);
}

static bool insideTriangle(int x, int y, const math::Vec4* _v){
//    math::Vec3 v[3];
//    for(int i=0;i<3;i++)
//        v[i] = {_v[i].x,_v[i].y, 1.0};
//    math::Vec3 f0,f1,f2;
//    f0 = glm::cross(v[1], v[0]);
//    f1 = glm::cross(v[2], v[1]);
//    f2 = glm::cross(v[0], v[2]);
//    math::Vec3 p(x,y,1.);
//    if( (glm::dot(p, f0) * glm::dot(f0, v[2])>0)
//        && (glm::dot(p, f1) * glm::dot(f1, v[0])>0)
//        && (glm::dot(p, f2) * glm::dot(f2, v[1])>0))
//        return true;
//    return false;

    const math::Vec4* v = _v;
    float x_ = static_cast<float>(x);
    float y_ = static_cast<float>(y);

    float x1 = v[1].x - v[0].x;
    float y1 = v[1].y - v[0].y;
    float x2 = x_ - v[0].x;
    float y2 = y_ - v[0].y;

    // vector1(x1, y1, 0)  vector2(x2, y2, 0) , PS: v[0].z, v[1].z, v[2].z 是深度值，只用于深度判断
    // cross(vector1 , vector2).z =  x1 * y2 - x2 * y1
    float z1 = x1 * y2 - x2 * y1 ;

    x1 = v[2].x - v[1].x;
    y1 = v[2].y - v[1].y;
    x2 = x_ - v[1].x;
    y2 = y_ - v[1].y;
    float z2 = x1 * y2 - x2 * y1 ;

    x1 = v[0].x - v[2].x;
    y1 = v[0].y - v[2].y;
    x2 = x_ - v[2].x;
    y2 = y_ - v[2].y;
    float z3 = x1 * y2 - x2 * y1;

    return  (z1 >= 0 && z2 >= 0 && z3 >= 0) || (z1 <= 0 && z2 <= 0 && z3 <= 0);

}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const math::Vec4* v){
    float c1 = (x*(v[1].y - v[2].y) + (v[2].x - v[1].x)*y + v[1].x*v[2].y - v[2].x*v[1].y) / (v[0].x*(v[1].y - v[2].y) + (v[2].x - v[1].x)*v[0].y + v[1].x*v[2].y - v[2].x*v[1].y);
    float c2 = (x*(v[2].y - v[0].y) + (v[0].x - v[2].x)*y + v[2].x*v[0].y - v[0].x*v[2].y) / (v[1].x*(v[2].y - v[0].y) + (v[0].x - v[2].x)*v[1].y + v[2].x*v[0].y - v[0].x*v[2].y);
    float c3 = (x*(v[0].y - v[1].y) + (v[1].x - v[0].x)*y + v[0].x*v[1].y - v[1].x*v[0].y) / (v[2].x*(v[0].y - v[1].y) + (v[1].x - v[0].x)*v[2].y + v[0].x*v[1].y - v[1].x*v[0].y);
    return {c1,c2,c3};
}

void rst::rasterizer::draw(std::vector<Triangle *> &TriangleList) {

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    math::Mat4 mvp = projection * view * model;
    for (const auto& t:TriangleList)
    {
        Triangle newtri = *t;

        std::array<math::Vec4, 3> mm {
                (view * model * t->v[0]),
                (view * model * t->v[1]),
                (view * model * t->v[2])
        };

        std::array<math::Vec3, 3> viewspace_pos;

        std::transform(mm.begin(), mm.end(), viewspace_pos.begin(), [](auto& v) {
            return math::Vec3{v.x, v.y, v.z};
        });

        math::Vec4 v[] = {
                mvp * t->v[0],
                mvp * t->v[1],
                mvp * t->v[2]
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec.x/=vec.w;
            vec.y/=vec.w;
            vec.z/=vec.w;
        }

        math::Mat4 inv_trans = glm::transpose(glm::inverse(view * model));
        math::Vec4 n[] = {
                inv_trans * to_vec4(t->normal[0], 0.0f),
                inv_trans * to_vec4(t->normal[1], 0.0f),
                inv_trans * to_vec4(t->normal[2], 0.0f)
        };

        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x = 0.5f*width*(vert.x+1.0f);
            vert.y = 0.5f*height*(vert.y+1.0f);
            vert.z = vert.z * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            //screen space coordinates
            newtri.setVertex(i, v[i]);
        }

        for (int i = 0; i < 3; ++i)
        {
            //view space normal
            newtri.setNormal(i, {n[i].x, n[i].y, n[i].z});
        }

        newtri.setColor(0, 148,121.0,92.0);
        newtri.setColor(1, 148,121.0,92.0);
        newtri.setColor(2, 148,121.0,92.0);

        // Also pass view space vertice position
        rasterize_triangle(newtri, viewspace_pos);
    }
}

static math::Vec3 interpolate(float alpha, float beta, float gamma, const math::Vec3& vert1, const math::Vec3& vert2, const math::Vec3& vert3, float weight)
{
    return (alpha * vert1 + beta * vert2 + gamma * vert3) / weight;
}

static math::Vec2 interpolate(float alpha, float beta, float gamma, const math::Vec2& vert1, const math::Vec2& vert2, const math::Vec2& vert3, float weight)
{
    auto u = (alpha * vert1[0] + beta * vert2[0] + gamma * vert3[0]);
    auto v = (alpha * vert1[1] + beta * vert2[1] + gamma * vert3[1]);

    u /= weight;
    v /= weight;

    return math::Vec2(u, v);
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t, const std::array<math::Vec3, 3>& view_pos) 
{
    // TODO: From your HW3, get the triangle rasterization code.

    auto v = t.v;

    // TODO : Find out the bounding box of current triangle.
    // iterate through the pixel and find if the current pixel is inside the triangle
    int left = width - 1;
    int right = 0;
    int bottom = height - 1;
    int top = 0;

    for (const auto& vert : t.v)
    {
        int x = (int) vert.x;
        int y = (int)std::ceil(vert.y);
        if (x < left)
        {
            left = std::max(x, 0);
        }
        if (x > right)
        {
            right = std::min(x, width - 1);
        }
        if (y < bottom)
        {
            bottom = std::max(y, 0);
        }
        if (y > top)
        {
            top = std::min(y, height - 1);
        }
    }

    for (int x = left; x <= right; ++x)
    {
        for (int y = bottom; y <= top; ++y)
        {
            if (insideTriangle(x, y, t.v))
            {
                auto index = get_index(x, y);

                auto[alpha, beta, gamma] = computeBarycentric2D((float)x, (float)y, t.v);
                if (std::isnan(alpha) || std::isinf(alpha)
                    || std::isnan(beta)  || std::isinf(beta)
                    || std::isnan(gamma) || std::isinf(gamma) )
                {
                    continue;
                }

                 float Z = 1.0f / (alpha / v[0].w + beta / v[1].w + gamma / v[2].w);
                 float zp = alpha * v[0].z / v[0].w + beta * v[1].z / v[1].w + gamma * v[2].z / v[2].w;
                 zp *= Z;

                // 深度比较失败
                if (this->depth_buf[index] <= zp)
                {
                    continue;
                }

                // TODO: Interpolate the attributes:
//                float weight = (alpha / v[0].w + beta / v[1].w + gamma / v[2].w);
                float weight = 1.0f;
                auto interpolated_color = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], weight);
                auto interpolated_normal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], weight);
                auto interpolated_texcoords = interpolate(alpha, beta, gamma, t.tex_coords[0], t.tex_coords[1], t.tex_coords[2], weight);
                auto interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos[0], view_pos[1], view_pos[2], weight);

                fragment_shader_payload payload( interpolated_color, glm::normalize(interpolated_normal), interpolated_texcoords, texture ? &*texture : nullptr);
                payload.view_pos = interpolated_shadingcoords;
                // Use: Instead of passing the triangle's color directly to the frame buffer, pass the color to the shaders first to get the final color;
                auto pixel_color = fragment_shader(payload);

                this->depth_buf[index] = zp;
                this->frame_buf[index] = pixel_color;
            }
        }
    }


    // TODO: Inside your rasterization loop:
    //    * v[i].w() is the vertex view space depth value z.
    //    * Z is interpolated view space depth for the current pixel
    //    * zp is depth between zNear and zFar, used for z-buffer

    // float Z = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    // float zp = alpha * v[0].z / v[0].w() + beta * v[1].z / v[1].w() + gamma * v[2].z / v[2].w();
    // zp *= Z;

    // TODO: Interpolate the attributes:
    // auto interpolated_color
    // auto interpolated_normal
    // auto interpolated_texcoords
    // auto interpolated_shadingcoords

    // Use: fragment_shader_payload payload( interpolated_color, interpolated_normal.normalized(), interpolated_texcoords, texture ? &*texture : nullptr);
    // Use: payload.view_pos = interpolated_shadingcoords;
    // Use: Instead of passing the triangle's color directly to the frame buffer, pass the color to the shaders first to get the final color;
    // Use: auto pixel_color = fragment_shader(payload);

 
}

void rst::rasterizer::set_model(const math::Mat4& m)
{
    model = m;
}

void rst::rasterizer::set_view(const math::Mat4& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const math::Mat4& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), math::Vec3{0.f, 0.1f, 0.18f});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);

    texture = nullptr;
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-y)*width + x;
}

void rst::rasterizer::set_pixel(const math::Vec2i &point, const math::Vec3 &color)
{
    //old index: auto ind = point.y + point.x * width;
    int ind = (height-point.y)*width + point.x;
    frame_buf[ind] = color;
}

void rst::rasterizer::set_vertex_shader(std::function<math::Vec3(vertex_shader_payload)> vert_shader)
{
    vertex_shader = vert_shader;
}

void rst::rasterizer::set_fragment_shader(std::function<math::Vec3(fragment_shader_payload)> frag_shader)
{
    fragment_shader = frag_shader;
}

}
