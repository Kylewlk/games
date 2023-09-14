//
// Created by goksu on 4/6/19.
//

#pragma once

#include "Triangle.hpp"
#include <Define.h>
#include "common/MathHelp.h"

namespace Game101_HW1
{
namespace rst {
enum class Buffers
{
    Color = 1,
    Depth = 2
};

inline Buffers operator|(Buffers a, Buffers b)
{
    return Buffers((int)a | (int)b);
}

inline Buffers operator&(Buffers a, Buffers b)
{
    return Buffers((int)a & (int)b);
}

enum class Primitive
{
    Line,
    Triangle
};

/*
 * For the curious : The draw function takes two buffer id's as its arguments.
 * These two structs make sure that if you mix up with their orders, the
 * compiler won't compile it. Aka : Type safety
 * */
struct pos_buf_id
{
    int pos_id = 0;
};

struct ind_buf_id
{
    int ind_id = 0;
};

class rasterizer
{
  public:
    rasterizer(int w, int h);
    pos_buf_id load_positions(const std::vector<math::Vec3>& positions);
    ind_buf_id load_indices(const std::vector<math::Vec3i>& indices);

    void set_model(const math::Mat4& m);
    void set_view(const math::Mat4& v);
    void set_projection(const math::Mat4& p);

    void set_pixel(const math::Vec3& point, const math::Vec3& color);

    void clear(Buffers buff);

    void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, Primitive type);

    std::vector<math::Vec3>& frame_buffer() { return frame_buf; }

  private:
    void draw_line(math::Vec3 begin, math::Vec3 end);
    void rasterize_wireframe(const Triangle& t);

  private:
    math::Mat4 model;
    math::Mat4 view;
    math::Mat4 projection;

    std::map<int, std::vector<math::Vec3>> pos_buf;
    std::map<int, std::vector<math::Vec3i>> ind_buf;

    std::vector<math::Vec3> frame_buf;
    std::vector<float> depth_buf;
    int get_index(int x, int y);

    int width, height;

    int next_id = 0;
    int get_next_id() { return next_id++; }
};
} // namespace rst
}