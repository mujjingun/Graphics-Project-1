#ifndef MODELS_H
#define MODELS_H

#include <glm/glm.hpp>
#include <vector>

#include "simpleshader.h"
#include "graphics/vertexarray.h"
#include "graphics/vertexbuffer.h"

namespace ou {

namespace {
    struct Attribute {
        glm::vec2 pos;
        glm::vec3 color;
    };

    std::vector<std::uint16_t> make_indices(std::initializer_list<int> sizes)
    {
        std::vector<std::uint16_t> indices;
        std::uint16_t i = 0;
        for (int size : sizes) {
            for (int j = 0; j < size; ++j) {
                indices.push_back(i++);
            }
            indices.push_back(-1);
        }
        return indices;
    }

    template <std::size_t N, Attribute (&Attr)[N], std::size_t... Sizes>
    class ModelData {
        VertexBuffer vbo, ebo;
        VertexArray vao;
        int count;

        ModelData()
        {
            vbo.setData(Attr, GL_STATIC_DRAW);

            auto indices = make_indices({ Sizes... });
            ebo.setData(indices, GL_STATIC_DRAW);
            vao.bindIndexBuffer(ebo);

            count = int(indices.size());

            auto vertexBinding = vao.getBinding(0);
            vertexBinding.bindVertexBuffer(vbo, 0, sizeof(Attribute));

            auto vertexAttr = vao.enableVertexAttrib(0);
            vertexAttr.setFormat(2, GL_FLOAT, GL_FALSE, offsetof(Attribute, pos));
            vertexAttr.setBinding(vertexBinding);

            auto colorAttr = vao.enableVertexAttrib(1);
            colorAttr.setFormat(3, GL_FLOAT, GL_FALSE, offsetof(Attribute, color));
            colorAttr.setBinding(vertexBinding);
        }

    public:
        static ModelData* self()
        {
            static ModelData sself;
            return &sself;
        }

        void render(glm::mat4 mat)
        {
            SimpleShader::self()->setUniform(0, mat);

            vao.use();
            SimpleShader::self()->use();
            glDrawElements(GL_TRIANGLE_FAN, count, GL_UNSIGNED_SHORT, nullptr);
        }
    };

    glm::vec3 plane_color[] = {
        { 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },
        { 245 / 255.0f, 211 / 255.0f, 0 / 255.0f },
        { 111 / 255.0f, 85 / 255.0f, 157 / 255.0f },
        { 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },
        { 245 / 255.0f, 211 / 255.0f, 0 / 255.0f },
        { 245 / 255.0f, 211 / 255.0f, 0 / 255.0f },
    };

    Attribute plane_attributes[] = {
        // big_wing
        { { 0.0, 0.0 }, plane_color[0] },
        { { -20.0, 15.0 }, plane_color[0] },
        { { -20.0, 20.0 }, plane_color[0] },
        { { 0.0, 23.0 }, plane_color[0] },
        { { 20.0, 20.0 }, plane_color[0] },
        { { 20.0, 15.0 }, plane_color[0] },

        // small_wing
        { { 0.0, -18.0 }, plane_color[1] },
        { { -11.0, -12.0 }, plane_color[1] },
        { { -12.0, -7.0 }, plane_color[1] },
        { { 0.0, -10.0 }, plane_color[1] },
        { { 12.0, -7.0 }, plane_color[1] },
        { { 11.0, -12.0 }, plane_color[1] },

        // body
        { { 0.0, -25.0 }, plane_color[2] },
        { { -6.0, 0.0 }, plane_color[2] },
        { { -6.0, 22.0 }, plane_color[2] },
        { { 6.0, 22.0 }, plane_color[2] },
        { { 6.0, 0.0 }, plane_color[2] },

        // back
        { { 0.0, 25.0 }, plane_color[3] },
        { { -7.0, 24.0 }, plane_color[3] },
        { { -7.0, 21.0 }, plane_color[3] },
        { { 7.0, 21.0 }, plane_color[3] },
        { { 7.0, 24.0 }, plane_color[3] },

        // sidewinder1
        { { -20.0, 10.0 }, plane_color[4] },
        { { -18.0, 3.0 }, plane_color[4] },
        { { -16.0, 10.0 }, plane_color[4] },
        { { -18.0, 20.0 }, plane_color[4] },
        { { -20.0, 20.0 }, plane_color[4] },

        // sidewinder2
        { { 20.0, 10.0 }, plane_color[5] },
        { { 18.0, 3.0 }, plane_color[5] },
        { { 16.0, 10.0 }, plane_color[5] },
        { { 18.0, 20.0 }, plane_color[5] },
        { { 20.0, 20.0 }, plane_color[5] },
    };
    using PlaneData = ModelData<32, plane_attributes, 6, 6, 5, 5, 5, 5>;

    glm::vec3 house_color[] = {
        { 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
        { 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
        { 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
        { 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
        { 44 / 255.0f, 180 / 255.0f, 49 / 255.0f },
    };

    Attribute house_attributes[] = {
        // roof
        { { -12.0, 0.0 }, house_color[0] },
        { { 0.0, 12.0 }, house_color[0] },
        { { 12.0, 0.0 }, house_color[0] },

        // house_body
        { { -12.0, -14.0 }, house_color[1] },
        { { -12.0, 0.0 }, house_color[1] },
        { { 12.0, 0.0 }, house_color[1] },
        { { 12.0, -14.0 }, house_color[1] },

        // chimney
        { { 6.0, 6.0 }, house_color[2] },
        { { 6.0, 14.0 }, house_color[2] },
        { { 10.0, 14.0 }, house_color[2] },
        { { 10.0, 2.0 }, house_color[2] },

        // door
        { { -8.0, -14.0 }, house_color[3] },
        { { -8.0, -8.0 }, house_color[3] },
        { { -4.0, -8.0 }, house_color[3] },
        { { -4.0, -14.0 }, house_color[3] },

        // window
        { { 4.0, -6.0 }, house_color[4] },
        { { 4.0, -2.0 }, house_color[4] },
        { { 8.0, -2.0 }, house_color[4] },
        { { 8.0, -6.0 }, house_color[4] },
    };
    using HouseData = ModelData<19, house_attributes, 3, 4, 4, 4, 4>;

    glm::vec3 car_color[7] = {
        { 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
        { 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
        { 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
        { 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
        { 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
        { 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
        { 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
    };

    Attribute car_attributes[] = {
        // car_body
        { { -16.0, -8.0 }, car_color[0] },
        { { -16.0, 0.0 }, car_color[0] },
        { { 16.0, 0.0 }, car_color[0] },
        { { 16.0, -8.0 }, car_color[0] },

        // car_frame
        { { -10.0, 0.0 }, car_color[1] },
        { { -10.0, 10.0 }, car_color[1] },
        { { 10.0, 10.0 }, car_color[1] },
        { { 10.0, 0.0 }, car_color[1] },

        // car_window
        { { -8.0, 0.0 }, car_color[2] },
        { { -8.0, 8.0 }, car_color[2] },
        { { 8.0, 8.0 }, car_color[2] },
        { { 8.0, 0.0 }, car_color[2] },

        // car_left_light
        { { -9.0, -6.0 }, car_color[3] },
        { { -10.0, -5.0 }, car_color[3] },
        { { -9.0, -4.0 }, car_color[3] },
        { { -8.0, -5.0 }, car_color[3] },

        // car_right_light
        { { 9.0, -6.0 }, car_color[4] },
        { { 8.0, -5.0 }, car_color[4] },
        { { 9.0, -4.0 }, car_color[4] },
        { { 10.0, -5.0 }, car_color[4] },

        // car_left_wheel
        { { -10.0, -12.0 }, car_color[5] },
        { { -10.0, -8.0 }, car_color[5] },
        { { -6.0, -8.0 }, car_color[5] },
        { { -6.0, -12.0 }, car_color[5] },

        // car_right_wheel
        { { 6.0, -12.0 }, car_color[6] },
        { { 6.0, -8.0 }, car_color[6] },
        { { 10.0, -8.0 }, car_color[6] },
        { { 10.0, -12.0 }, car_color[6] },
    };
    using CarData = ModelData<28, car_attributes, 4, 4, 4, 4, 4, 4, 4>;

    glm::vec3 cocktail_color[5] = {
        { 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
        { 0 / 255.0f, 63 / 255.0f, 122 / 255.0f },
        { 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
        { 191 / 255.0f, 255 / 255.0f, 0 / 255.0f },
        { 218 / 255.0f, 165 / 255.0f, 32 / 255.0f }
    };

    Attribute cocktail_attributes[] = {
        // neck
        { { -6.0, -12.0 }, cocktail_color[0] },
        { { -6.0, -11.0 }, cocktail_color[0] },
        { { -1.0, 0.0 }, cocktail_color[0] },
        { { 1.0, 0.0 }, cocktail_color[0] },
        { { 6.0, -11.0 }, cocktail_color[0] },
        { { 6.0, -12.0 }, cocktail_color[0] },

        // liquid
        { { -1.0, 0.0 }, cocktail_color[1] },
        { { -9.0, 4.0 }, cocktail_color[1] },
        { { -12.0, 7.0 }, cocktail_color[1] },
        { { 12.0, 7.0 }, cocktail_color[1] },
        { { 9.0, 4.0 }, cocktail_color[1] },
        { { 1.0, 0.0 }, cocktail_color[1] },

        // remain
        { { -12.0, 7.0 }, cocktail_color[2] },
        { { -12.0, 10.0 }, cocktail_color[2] },
        { { 12.0, 10.0 }, cocktail_color[2] },
        { { 12.0, 7.0 }, cocktail_color[2] },

        // straw
        { { 7.0, 7.0 }, cocktail_color[3] },
        { { 12.0, 12.0 }, cocktail_color[3] },
        { { 14.0, 12.0 }, cocktail_color[3] },
        { { 9.0, 7.0 }, cocktail_color[3] },

        // deco
        { { 12.0, 12.0 }, cocktail_color[4] },
        { { 10.0, 14.0 }, cocktail_color[4] },
        { { 10.0, 16.0 }, cocktail_color[4] },
        { { 12.0, 18.0 }, cocktail_color[4] },
        { { 14.0, 18.0 }, cocktail_color[4] },
        { { 16.0, 16.0 }, cocktail_color[4] },
        { { 16.0, 14.0 }, cocktail_color[4] },
        { { 14.0, 12.0 }, cocktail_color[4] },
    };
    using CocktailData = ModelData<28, cocktail_attributes, 6, 6, 4, 4, 8>;

    glm::vec3 squid_color[5] = {
        { 215 / 255.0f, 167 / 255.0f, 0 / 255.0f },
        { 113 / 255.0f, 87 / 255.0f, 0 / 255.0f },
        { 80 / 255.0f, 62 / 255.0f, 0 / 255.0f },
        { 10 / 255.0f, 10 / 255.0f, 10 / 255.0f },
    };

    Attribute squid_attributes[] = {
        // squid_head
        { { 18, 31 }, squid_color[0] },
        { { 6, 23 }, squid_color[0] },
        { { 26, 23 }, squid_color[0] },
        // squid_body
        { { 18, 29 }, squid_color[1] },
        { { 21, 20 }, squid_color[1] },
        { { 20, 7 }, squid_color[1] },
        { { 12, 8 }, squid_color[1] },
        { { 13, 24 }, squid_color[1] },
        // squid_tentacle1
        { { 12, 8 }, squid_color[2] },
        { { 20, 7 }, squid_color[2] },
        { { 1, 6 }, squid_color[2] },
        // squid_tentacle2
        { { 12, 8 }, squid_color[2] },
        { { 20, 7 }, squid_color[2] },
        { { 8, 0 }, squid_color[2] },
        // squid_tentacle3
        { { 12, 8 }, squid_color[2] },
        { { 20, 7 }, squid_color[2] },
        { { 14, 1 }, squid_color[2] },
        // squid_tentacle4
        { { 12, 8 }, squid_color[2] },
        { { 20, 7 }, squid_color[2] },
        { { 18, 0 }, squid_color[2] },
        // squid_tentacle5
        { { 12, 8 }, squid_color[2] },
        { { 20, 7 }, squid_color[2] },
        { { 28, 1 }, squid_color[2] },
        // squid_eye1
        { { 10, 8 }, squid_color[3] },
        { { 10, 10 }, squid_color[3] },
        { { 12, 10 }, squid_color[3] },
        { { 12, 8 }, squid_color[3] },
        // squid_eye2
        { { 18, 8 }, squid_color[3] },
        { { 18, 10 }, squid_color[3] },
        { { 20, 10 }, squid_color[3] },
        { { 20, 8 }, squid_color[3] },
    };

    using SquidData = ModelData<31, squid_attributes, 3, 5, 3, 3, 3, 3, 3, 4, 4>;

    glm::vec3 balloon_color[1] = {
        { 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
    };

    Attribute balloon_attributes[] = {
        // balloon_body
        { { 14, 4 }, balloon_color[0] },
        { { 12, 6 }, balloon_color[0] },
        { { 9, 10 }, balloon_color[0] },
        { { 7, 16 }, balloon_color[0] },
        { { 7, 21 }, balloon_color[0] },
        { { 9, 26 }, balloon_color[0] },
        { { 13, 28 }, balloon_color[0] },
        { { 20, 29 }, balloon_color[0] },
        { { 25, 26 }, balloon_color[0] },
        { { 28, 21 }, balloon_color[0] },
        { { 27, 13 }, balloon_color[0] },
        { { 23, 9 }, balloon_color[0] },
        { { 17, 5 }, balloon_color[0] },

        // balloon_tail
        { { 14, 4 }, balloon_color[0] },
        { { 10, 1 }, balloon_color[0] },
        { { 17, 0 }, balloon_color[0] },
    };
    using BalloonData = ModelData<16, balloon_attributes, 13, 3>;
}
}

#endif // MODELS_H
