//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"

namespace Game101_HW2
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

auto to_vec4(const math::Vec3& v3, float w = 1.0f)
{
    return math::Vec4(v3.x, v3.y, v3.z, w);
}


static bool insideTriangle(float x, float y, const math::Vec3* v)
{   
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    float x1 = v[1].x - v[0].x;
    float y1 = v[1].y - v[0].y;
    float x2 = x - v[0].x;
    float y2 = y - v[0].y;

    // vector1(x1, y1, 0)  vector2(x2, y2, 0) , PS: v[0].z, v[1].z, v[2].z 是深度值，只用于深度判断
    // cross(vector1 , vector2).z =  x1 * y2 - x2 * y1
    float z1 = x1 * y2 - x2 * y1 ;

    x1 = v[2].x - v[1].x;
    y1 = v[2].y - v[1].y;
    x2 = x - v[1].x;
    y2 = y - v[1].y;
    float z2 = x1 * y2 - x2 * y1 ;

    x1 = v[0].x - v[2].x;
    y1 = v[0].y - v[2].y;
    x2 = x - v[2].x;
    y2 = y - v[2].y;
    float z3 = x1 * y2 - x2 * y1;

    return  (z1 > 0 && z2 > 0 && z3 > 0) || (z1 < 0 && z2 < 0 && z3 < 0);
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const math::Vec3* v)
{
    float c1 = (x*(v[1].y - v[2].y) + (v[2].x - v[1].x)*y + v[1].x*v[2].y - v[2].x*v[1].y) / (v[0].x*(v[1].y - v[2].y) + (v[2].x - v[1].x)*v[0].y + v[1].x*v[2].y - v[2].x*v[1].y);
    float c2 = (x*(v[2].y - v[0].y) + (v[0].x - v[2].x)*y + v[2].x*v[0].y - v[0].x*v[2].y) / (v[1].x*(v[2].y - v[0].y) + (v[0].x - v[2].x)*v[1].y + v[2].x*v[0].y - v[0].x*v[2].y);
    float c3 = (x*(v[0].y - v[1].y) + (v[1].x - v[0].x)*y + v[0].x*v[1].y - v[1].x*v[0].y) / (v[2].x*(v[0].y - v[1].y) + (v[1].x - v[0].x)*v[2].y + v[0].x*v[1].y - v[1].x*v[0].y);
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    math::Mat4 mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        math::Vec4 v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w;
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x = 0.5f*width*(vert.x+1.0f);
            vert.y = 0.5f*height*(vert.y+1.0f);
            vert.z = vert.z * f1 + f2;
        }

        for (int j = 0; j < 3; ++j)
        {
            t.setVertex(j, {v[j].x, v[j].y, v[j].z} );
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t)
{
    auto v = t.toVector4();

    // TODO : Find out the bounding box of current triangle.
    // iterate through the pixel and find if the current pixel is inside the triangle
    float left = static_cast<float>(width);
    float right = 0;
    float bottom = static_cast<float>(height);
    float top = 0;

    for (const auto& vert : v)
    {
        float x = std::floor(vert.x);
        float y = std::ceil(vert.y);
        if (x < left)
        {
            left = std::max(x, 0.0f);
        }
        if (x > right)
        {
            right = std::min(x, static_cast<float>(width));
        }
        if (y < bottom)
        {
            bottom = std::max(y, 0.0f);
        }
        if (y > top)
        {
            top = std::min(y, static_cast<float>(height));
        }
    }

    // If so, use the following code to get the interpolated z value.
    //auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
    //float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    //float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    //z_interpolated *= w_reciprocal;

    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.

    float step = 1.0f;
    float center = 0.5f;
    math::Vec3* frameBuf = this->frame_buf.data();
    float* depthBuf = this->depth_buf.data();
    if (this->super_sampling)
    {
        step = 0.5f;
        frameBuf = this->frame_buf_ss.data();
        depthBuf = this->depth_buf_ss.data();
    }
    int index{};
    for (auto x = left; x < right; x += step)
    {
        for (auto y = bottom; y < top; y += step)
        {
            if (insideTriangle(x, y, t.v))
            {
                index = this->super_sampling ? get_index_ss(x, y) : get_index(x, y);

                auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
                float w_reciprocal = 1.0f/(alpha / v[0].w + beta / v[1].w + gamma / v[2].w);
                float z_interpolated = alpha * v[0].z / v[0].w + beta * v[1].z / v[1].w + gamma * v[2].z / v[2].w;
                z_interpolated *= w_reciprocal;

                // 深度比较失败
                if (depthBuf[index] < z_interpolated)
                {
                    continue;
                }

                depthBuf[index] = z_interpolated;
                frameBuf[index] = t.color[0] * alpha + t.color[1] * beta + t.color[2] * gamma;
            }
        }
    }

    if (this->super_sampling)
    {
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                auto& c1 = frame_buf_ss[(i * 2 + 0) * width * 2 + j * 2 + 0];
                auto& c2 = frame_buf_ss[(i * 2 + 0) * width * 2 + j * 2 + 1];
                auto& c3 = frame_buf_ss[(i * 2 + 1) * width * 2 + j * 2 + 0];
                auto& c4 = frame_buf_ss[(i * 2 + 1) * width * 2 + j * 2 + 1];

                frame_buf[i * width + j] = (c1 + c2 + c3 + c4) * 0.25f;
            }
        }
    }
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
        std::fill(frame_buf.begin(), frame_buf.end(), math::Vec3{0.0f, 0.1f, 0.2f});
        if (this->super_sampling)
        {
            std::fill(frame_buf_ss.begin(), frame_buf_ss.end(), math::Vec3{0.0f, 0.1f, 0.2f});
        }
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
        if (this->super_sampling)
        {
            std::fill(depth_buf_ss.begin(), depth_buf_ss.end(), std::numeric_limits<float>::infinity());
        }
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);

    frame_buf_ss.resize(w * h * 4);
    depth_buf_ss.resize(w * h * 4);
}

//int rst::rasterizer::get_index(int x, int y)
//{
//    return (height-1-y)*width + x;
//}

void rst::rasterizer::set_pixel(const math::Vec3& point, const math::Vec3& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y)*width + point.x;
    frame_buf[ind] = color;

}
int rst::rasterizer::get_index(float x, float y) const
{
    return (height-1- static_cast<int>(y) )*width + static_cast<int>(x);;
}

int rst::rasterizer::get_index_ss(float x, float y) const
{
    return (height * 2 -1- static_cast<int>(y * 2.0f) ) * width * 2 + static_cast<int>(x * 2.0f);;
}

}
