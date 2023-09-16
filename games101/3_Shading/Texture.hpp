//
// Created by LEI XU on 4/27/19.
//
#pragma once

#include "global.hpp"
#include <Define.h>

namespace Game101_HW3
{

class Texture{
private:

    std::vector<uint8_t> image_data;

public:
    explicit Texture(const std::string& name)
    {
        int channels_in_file;
        uint8_t* imageData = stbi_load(name.c_str(), &width, &height, &channels_in_file, STBI_rgb);
        if (imageData == nullptr)
        {
            throw std::runtime_error("Load image failed!");
        }
        image_data.resize(width * height * 3);
        memcpy(image_data.data(), imageData, image_data.size());

        stbi_image_free(imageData);
    }

    int width{};
    int height{};

    math::Vec3 getColor(float u, float v) const
    {
        auto u_img = u * static_cast<float>(width);
        auto v_img = (1 - std::fmod(v, 1.0f)) * static_cast<float>(height);

        int index = (int(v_img) % height) * width * 3 + (int(u_img) % width)  * 3;

        return {image_data[index], image_data[index + 1], image_data[index+2]};
    }

    math::Vec3  getColorBilinear(float u, float v)
    {
        v = 1.0f - std::fmod(v, 1.0f);

        float uf = u * static_cast<float>(width) - std::floor(u * static_cast<float>(width));
        float vf = v * static_cast<float>(height) - std::floor(v * static_cast<float>(height));

        int u_img = int(u * static_cast<float>(width));
        int v_img = int(v * static_cast<float>(height));

        int i0 = (v_img % height) * width * 3 + (u_img % width)  * 3;
        int i1 = (v_img % height) * width * 3 + ((u_img + 1) % width)  * 3;

        int j0 = ((v_img + 1) % height) * width * 3 + (u_img % width)  * 3;
        int j1 = ((v_img + 1) % height) * width * 3 + ((u_img + 1) % width)  * 3;

        auto c1 = glm::mix(math::Vec3 {image_data[i0], image_data[i0+1], image_data[i0+2]},
                           math::Vec3{image_data[i1], image_data[i1+1], image_data[i1+2]}, uf);
        auto c2 = glm::mix(math::Vec3 {image_data[j0], image_data[j0+1], image_data[j0+2]},
                           math::Vec3{image_data[j1], image_data[j1+1], image_data[j1+2]}, uf);

        return glm::mix(c1, c2, vf);
    }
};

}