//
// Created by goksu on 4/6/19.
//

#pragma once

#include <Define.h>
#include <algorithm>
#include "global.hpp"
#include "Triangle.hpp"

namespace Game101_HW2::rst
{
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
     * For the curious : The draw function takes two buffer id's as its arguments. These two structs
     * make sure that if you mix up with their orders, the compiler won't compile it.
     * Aka : Type safety
     * */
    struct pos_buf_id
    {
        int pos_id = 0;
    };

    struct ind_buf_id
    {
        int ind_id = 0;
    };

    struct col_buf_id
    {
        int col_id = 0;
    };

    class rasterizer
    {
    public:
        rasterizer(int w, int h);
        pos_buf_id load_positions(const std::vector<math::Vec3>& positions);
        ind_buf_id load_indices(const std::vector<math::Vec3i>& indices);
        col_buf_id load_colors(const std::vector<math::Vec3>& colors);

        void set_model(const math::Mat4& m);
        void set_view(const math::Mat4& v);
        void set_projection(const math::Mat4& p);

        void set_pixel(const math::Vec3& point, const math::Vec3& color);

        void clear(Buffers buff);

        void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type);

        std::vector<math::Vec3>& frame_buffer() { return frame_buf; }

        bool super_sampling{false};

    private:
        void draw_line(math::Vec3 begin, math::Vec3 end);

        void rasterize_triangle(const Triangle& t);

        // VERTEX SHADER -> MVP -> Clipping -> /.W -> VIEWPORT -> DRAWLINE/DRAWTRI -> FRAGSHADER

    private:
        math::Mat4 model;
        math::Mat4 view;
        math::Mat4 projection;

        std::map<int, std::vector<math::Vec3>> pos_buf;
        std::map<int, std::vector<math::Vec3i>> ind_buf;
        std::map<int, std::vector<math::Vec3>> col_buf;

        std::vector<math::Vec3> frame_buf;
        std::vector<float> depth_buf;
//        int get_index(int x, int y);
        int get_index(float x, float y) const;

        std::vector<math::Vec3> frame_buf_ss;
        std::vector<float> depth_buf_ss;
        int get_index_ss(float x, float y) const;

        int width, height;

        int next_id = 0;
        int get_next_id() { return next_id++; }
    };
}
