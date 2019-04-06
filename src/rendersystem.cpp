#include "rendersystem.h"
#include "components.h"
#include "ecsengine.h"
#include "entity.h"
#include "pointshader.h"
#include "simpleshader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace ou {

namespace {
    const int AIRPLANE_BIG_WING = 0;
    const int AIRPLANE_SMALL_WING = 1;
    const int AIRPLANE_BODY = 2;
    const int AIRPLANE_BACK = 3;
    const int AIRPLANE_SIDEWINDER1 = 4;
    const int AIRPLANE_SIDEWINDER2 = 5;
    const int AIRPLANE_CENTER = 6;
    glm::vec3 airplane_color[7] = {
        { 150 / 255.0f, 129 / 255.0f, 183 / 255.0f }, // big_wing
        { 245 / 255.0f, 211 / 255.0f, 0 / 255.0f }, // small_wing
        { 111 / 255.0f, 85 / 255.0f, 157 / 255.0f }, // body
        { 150 / 255.0f, 129 / 255.0f, 183 / 255.0f }, // back
        { 245 / 255.0f, 211 / 255.0f, 0 / 255.0f }, // sidewinder1
        { 245 / 255.0f, 211 / 255.0f, 0 / 255.0f }, // sidewinder2
        { 100 / 255.0f, 255 / 255.0f, 255 / 255.0f } // center
    };

    glm::vec2 big_wing[6] = { { 0.0, 0.0 }, { -20.0, 15.0 }, { -20.0, 20.0 }, { 0.0, 23.0 }, { 20.0, 20.0 }, { 20.0, 15.0 } };
    glm::vec2 small_wing[6] = { { 0.0, -18.0 }, { -11.0, -12.0 }, { -12.0, -7.0 }, { 0.0, -10.0 }, { 12.0, -7.0 }, { 11.0, -12.0 } };
    glm::vec2 body[5] = { { 0.0, -25.0 }, { -6.0, 0.0 }, { -6.0, 22.0 }, { 6.0, 22.0 }, { 6.0, 0.0 } };
    glm::vec2 back[5] = { { 0.0, 25.0 }, { -7.0, 24.0 }, { -7.0, 21.0 }, { 7.0, 21.0 }, { 7.0, 24.0 } };
    glm::vec2 sidewinder1[5] = { { -20.0, 10.0 }, { -18.0, 3.0 }, { -16.0, 10.0 }, { -18.0, 20.0 }, { -20.0, 20.0 } };
    glm::vec2 sidewinder2[5] = { { 20.0, 10.0 }, { 18.0, 3.0 }, { 16.0, 10.0 }, { 18.0, 20.0 }, { 20.0, 20.0 } };
    glm::vec2 center[1] = { { 0.0, 0.0 } };

    struct PlaneData {
        VertexBuffer vbo;
        VertexArray vao;

        static PlaneData* self()
        {
            static PlaneData sself;
            return &sself;
        }

        void render(glm::mat4 mat)
        {
            SimpleShader::self()->setUniform(0, mat);

            vao.use();
            SimpleShader::self()->use();

            SimpleShader::self()->setUniform(1, airplane_color[AIRPLANE_BIG_WING]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

            SimpleShader::self()->setUniform(1, airplane_color[AIRPLANE_SMALL_WING]);
            glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

            SimpleShader::self()->setUniform(1, airplane_color[AIRPLANE_BODY]);
            glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

            SimpleShader::self()->setUniform(1, airplane_color[AIRPLANE_BACK]);
            glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

            SimpleShader::self()->setUniform(1, airplane_color[AIRPLANE_SIDEWINDER1]);
            glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

            SimpleShader::self()->setUniform(1, airplane_color[AIRPLANE_SIDEWINDER2]);
            glDrawArrays(GL_TRIANGLE_FAN, 27, 5);
        }

    private:
        PlaneData()
        {
            std::vector<glm::vec2> vertices;
            for (auto const& v : big_wing) {
                vertices.push_back(v);
            }
            for (auto const& v : small_wing) {
                vertices.push_back(v);
            }
            for (auto const& v : body) {
                vertices.push_back(v);
            }
            for (auto const& v : back) {
                vertices.push_back(v);
            }
            for (auto const& v : sidewinder1) {
                vertices.push_back(v);
            }
            for (auto const& v : sidewinder2) {
                vertices.push_back(v);
            }
            for (auto const& v : center) {
                vertices.push_back(v);
            }
            vbo.setData(vertices, GL_STATIC_DRAW);

            auto vertexBinding = vao.getBinding(0);
            vertexBinding.bindVertexBuffer(vbo, 0, sizeof(glm::vec2));

            auto vertexAttr = vao.enableVertexAttrib(0);
            vertexAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);
            vertexAttr.setBinding(vertexBinding);
        }
    };

    const int HOUSE_ROOF = 0;
    const int HOUSE_BODY = 1;
    const int HOUSE_CHIMNEY = 2;
    const int HOUSE_DOOR = 3;
    const int HOUSE_WINDOW = 4;

    glm::vec2 roof[3] = { { -12.0, 0.0 }, { 0.0, 12.0 }, { 12.0, 0.0 } };
    glm::vec2 house_body[4] = { { -12.0, -14.0 }, { -12.0, 0.0 }, { 12.0, 0.0 }, { 12.0, -14.0 } };
    glm::vec2 chimney[4] = { { 6.0, 6.0 }, { 6.0, 14.0 }, { 10.0, 14.0 }, { 10.0, 2.0 } };
    glm::vec2 door[4] = { { -8.0, -14.0 }, { -8.0, -8.0 }, { -4.0, -8.0 }, { -4.0, -14.0 } };
    glm::vec2 window[4] = { { 4.0, -6.0 }, { 4.0, -2.0 }, { 8.0, -2.0 }, { 8.0, -6.0 } };

    glm::vec3 house_color[5] = {
        { 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
        { 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
        { 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
        { 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
        { 44 / 255.0f, 180 / 255.0f, 49 / 255.0f },
    };

    struct HouseData {
        VertexBuffer vbo;
        VertexArray vao;

        static HouseData* self()
        {
            static HouseData me;
            return &me;
        }

        void render(glm::mat4 viewProjMat, glm::mat4 mat)
        {
            mat = mat * glm::scale(glm::mat4(1.0f), glm::vec3(1 / 18.0f));
            SimpleShader::self()->setUniform(0, viewProjMat * mat);

            vao.use();
            SimpleShader::self()->use();

            SimpleShader::self()->setUniform(1, house_color[HOUSE_ROOF]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

            SimpleShader::self()->setUniform(1, house_color[HOUSE_BODY]);
            glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

            SimpleShader::self()->setUniform(1, house_color[HOUSE_CHIMNEY]);
            glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

            SimpleShader::self()->setUniform(1, house_color[HOUSE_DOOR]);
            glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

            SimpleShader::self()->setUniform(1, house_color[HOUSE_WINDOW]);
            glDrawArrays(GL_TRIANGLE_FAN, 15, 4);
        }

    private:
        HouseData()
        {
            std::vector<glm::vec2> vertices;
            for (auto const& v : roof) {
                vertices.push_back(v);
            }
            for (auto const& v : house_body) {
                vertices.push_back(v);
            }
            for (auto const& v : chimney) {
                vertices.push_back(v);
            }
            for (auto const& v : door) {
                vertices.push_back(v);
            }
            for (auto const& v : window) {
                vertices.push_back(v);
            }

            vbo.setData(vertices, GL_STATIC_DRAW);

            auto vertexBinding = vao.getBinding(0);
            vertexBinding.bindVertexBuffer(vbo, 0, sizeof(glm::vec2));

            auto vertexAttr = vao.enableVertexAttrib(0);
            vertexAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);
            vertexAttr.setBinding(vertexBinding);
        }
    };

    const int CAR_BODY = 0;
    const int CAR_FRAME = 1;
    const int CAR_WINDOW = 2;
    const int CAR_LEFT_LIGHT = 3;
    const int CAR_RIGHT_LIGHT = 4;
    const int CAR_LEFT_WHEEL = 5;
    const int CAR_RIGHT_WHEEL = 6;

    glm::vec2 car_body[4] = { { -16.0, -8.0 }, { -16.0, 0.0 }, { 16.0, 0.0 }, { 16.0, -8.0 } };
    glm::vec2 car_frame[4] = { { -10.0, 0.0 }, { -10.0, 10.0 }, { 10.0, 10.0 }, { 10.0, 0.0 } };
    glm::vec2 car_window[4] = { { -8.0, 0.0 }, { -8.0, 8.0 }, { 8.0, 8.0 }, { 8.0, 0.0 } };
    glm::vec2 car_left_light[4] = { { -9.0, -6.0 }, { -10.0, -5.0 }, { -9.0, -4.0 }, { -8.0, -5.0 } };
    glm::vec2 car_right_light[4] = { { 9.0, -6.0 }, { 8.0, -5.0 }, { 9.0, -4.0 }, { 10.0, -5.0 } };
    glm::vec2 car_left_wheel[4] = { { -10.0, -12.0 }, { -10.0, -8.0 }, { -6.0, -8.0 }, { -6.0, -12.0 } };
    glm::vec2 car_right_wheel[4] = { { 6.0, -12.0 }, { 6.0, -8.0 }, { 10.0, -8.0 }, { 10.0, -12.0 } };

    glm::vec3 car_color[7] = {
        { 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
        { 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
        { 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
        { 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
        { 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
        { 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
        { 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
    };

    struct CarData {
        VertexBuffer vbo;
        VertexArray vao;

        static CarData* self()
        {
            static CarData me;
            return &me;
        }

        void render(glm::mat4 viewProjMat, glm::mat4 mat)
        {
            mat = mat * glm::scale(glm::mat4(1.0f), glm::vec3(1 / 18.0f));
            SimpleShader::self()->setUniform(0, viewProjMat * mat);

            vao.use();
            SimpleShader::self()->use();

            SimpleShader::self()->setUniform(1, car_color[CAR_BODY]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            SimpleShader::self()->setUniform(1, car_color[CAR_FRAME]);
            glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

            SimpleShader::self()->setUniform(1, car_color[CAR_WINDOW]);
            glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

            SimpleShader::self()->setUniform(1, car_color[CAR_LEFT_LIGHT]);
            glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

            SimpleShader::self()->setUniform(1, car_color[CAR_RIGHT_LIGHT]);
            glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

            SimpleShader::self()->setUniform(1, car_color[CAR_LEFT_WHEEL]);
            glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

            SimpleShader::self()->setUniform(1, car_color[CAR_RIGHT_WHEEL]);
            glDrawArrays(GL_TRIANGLE_FAN, 24, 4);
        }

    private:
        CarData()
        {
            std::vector<glm::vec2> vertices;
            for (auto const& v : car_body) {
                vertices.push_back(v);
            }
            for (auto const& v : car_frame) {
                vertices.push_back(v);
            }
            for (auto const& v : car_window) {
                vertices.push_back(v);
            }
            for (auto const& v : car_left_light) {
                vertices.push_back(v);
            }
            for (auto const& v : car_right_light) {
                vertices.push_back(v);
            }
            for (auto const& v : car_left_wheel) {
                vertices.push_back(v);
            }
            for (auto const& v : car_right_wheel) {
                vertices.push_back(v);
            }

            vbo.setData(vertices, GL_STATIC_DRAW);

            auto vertexBinding = vao.getBinding(0);
            vertexBinding.bindVertexBuffer(vbo, 0, sizeof(glm::vec2));

            auto vertexAttr = vao.enableVertexAttrib(0);
            vertexAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);
            vertexAttr.setBinding(vertexBinding);
        }
    };

    const int COCKTAIL_NECK = 0;
    const int COCKTAIL_LIQUID = 1;
    const int COCKTAIL_REMAIN = 2;
    const int COCKTAIL_STRAW = 3;
    const int COCKTAIL_DECO = 4;

    glm::vec2 neck[6] = {
        { -6.0, -12.0 },
        { -6.0, -11.0 },
        { -1.0, 0.0 },
        { 1.0, 0.0 },
        { 6.0, -11.0 },
        { 6.0, -12.0 },
    };
    glm::vec2 liquid[6] = {
        { -1.0, 0.0 },
        { -9.0, 4.0 },
        { -12.0, 7.0 },
        { 12.0, 7.0 },
        { 9.0, 4.0 },
        { 1.0, 0.0 },
    };
    glm::vec2 remain[4] = { { -12.0, 7.0 }, { -12.0, 10.0 }, { 12.0, 10.0 }, { 12.0, 7.0 } };
    glm::vec2 straw[4] = { { 7.0, 7.0 }, { 12.0, 12.0 }, { 14.0, 12.0 }, { 9.0, 7.0 } };
    glm::vec2 deco[8] = {
        { 12.0, 12.0 },
        { 10.0, 14.0 },
        { 10.0, 16.0 },
        { 12.0, 18.0 },
        { 14.0, 18.0 },
        { 16.0, 16.0 },
        { 16.0, 14.0 },
        { 14.0, 12.0 },
    };

    glm::vec3 cocktail_color[5] = {
        { 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
        { 0 / 255.0f, 63 / 255.0f, 122 / 255.0f },
        { 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
        { 191 / 255.0f, 255 / 255.0f, 0 / 255.0f },
        { 218 / 255.0f, 165 / 255.0f, 32 / 255.0f }
    };

    struct CocktailData {
        VertexBuffer vbo;
        VertexArray vao;

        static CocktailData* self()
        {
            static CocktailData me;
            return &me;
        }

        void render(glm::mat4 viewProjMat, glm::mat4 mat)
        {
            mat = mat * glm::scale(glm::mat4(1.0f), glm::vec3(1 / 18.0f));
            SimpleShader::self()->setUniform(0, viewProjMat * mat);

            vao.use();
            SimpleShader::self()->use();

            SimpleShader::self()->setUniform(1, cocktail_color[COCKTAIL_NECK]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

            SimpleShader::self()->setUniform(1, cocktail_color[COCKTAIL_LIQUID]);
            glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

            SimpleShader::self()->setUniform(1, cocktail_color[COCKTAIL_REMAIN]);
            glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

            SimpleShader::self()->setUniform(1, cocktail_color[COCKTAIL_STRAW]);
            glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

            SimpleShader::self()->setUniform(1, cocktail_color[COCKTAIL_DECO]);
            glDrawArrays(GL_TRIANGLE_FAN, 20, 8);
        }

    private:
        CocktailData()
        {
            std::vector<glm::vec2> vertices;
            for (auto const& v : neck) {
                vertices.push_back(v);
            }
            for (auto const& v : liquid) {
                vertices.push_back(v);
            }
            for (auto const& v : remain) {
                vertices.push_back(v);
            }
            for (auto const& v : straw) {
                vertices.push_back(v);
            }
            for (auto const& v : deco) {
                vertices.push_back(v);
            }

            vbo.setData(vertices, GL_STATIC_DRAW);

            auto vertexBinding = vao.getBinding(0);
            vertexBinding.bindVertexBuffer(vbo, 0, sizeof(glm::vec2));

            auto vertexAttr = vao.enableVertexAttrib(0);
            vertexAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);
            vertexAttr.setBinding(vertexBinding);
        }
    };

    glm::vec2 squid_head[3] = {
        { 18, 31 },
        { 6, 23 },
        { 26, 23 },
    };
    glm::vec2 squid_body[5] = {
        { 18, 29 },
        { 21, 20 },
        { 20, 7 },
        { 12, 8 },
        { 13, 24 },
    };
    glm::vec2 squid_tentacle1[3] = {
        { 12, 8 },
        { 20, 7 },
        { 1, 6 },
    };
    glm::vec2 squid_tentacle2[3] = {
        { 12, 8 },
        { 20, 7 },
        { 8, 0 },
    };
    glm::vec2 squid_tentacle3[3] = {
        { 12, 8 },
        { 20, 7 },
        { 14, 1 },
    };
    glm::vec2 squid_tentacle4[3] = {
        { 12, 8 },
        { 20, 7 },
        { 18, 0 },
    };
    glm::vec2 squid_tentacle5[3] = {
        { 12, 8 },
        { 20, 7 },
        { 28, 1 },
    };
    glm::vec2 squid_eye1[4] = {
        { 10, 8 },
        { 10, 10 },
        { 12, 10 },
        { 12, 8 },
    };
    glm::vec2 squid_eye2[4] = {
        { 18, 8 },
        { 18, 10 },
        { 20, 10 },
        { 20, 8 },
    };

    glm::vec3 squid_color[5] = {
        { 215 / 255.0f, 167 / 255.0f, 0 / 255.0f },
        { 113 / 255.0f, 87 / 255.0f, 0 / 255.0f },
        { 80 / 255.0f, 62 / 255.0f, 0 / 255.0f },
        { 10 / 255.0f, 10 / 255.0f, 10 / 255.0f },
    };

    struct SquidData {
        VertexBuffer vbo;
        VertexArray vao;

        static SquidData* self()
        {
            static SquidData me;
            return &me;
        }

        void render(glm::mat4 viewProjMat, glm::mat4 mat)
        {
            mat = mat * glm::translate(glm::mat4(1.0), glm::vec3(-1))
                * glm::scale(glm::mat4(1.0), glm::vec3(1.f / 16));
            SimpleShader::self()->setUniform(0, viewProjMat * mat);

            vao.use();
            SimpleShader::self()->use();

            SimpleShader::self()->setUniform(1, squid_color[0]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

            SimpleShader::self()->setUniform(1, squid_color[1]);
            glDrawArrays(GL_TRIANGLE_FAN, 3, 5);

            SimpleShader::self()->setUniform(1, squid_color[3]);
            glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

            SimpleShader::self()->setUniform(1, squid_color[3]);
            glDrawArrays(GL_TRIANGLE_FAN, 27, 4);

            SimpleShader::self()->setUniform(1, squid_color[2]);
            glDrawArrays(GL_TRIANGLE_FAN, 8, 3);

            SimpleShader::self()->setUniform(1, squid_color[2]);
            glDrawArrays(GL_TRIANGLE_FAN, 11, 3);

            SimpleShader::self()->setUniform(1, squid_color[2]);
            glDrawArrays(GL_TRIANGLE_FAN, 14, 3);

            SimpleShader::self()->setUniform(1, squid_color[2]);
            glDrawArrays(GL_TRIANGLE_FAN, 17, 3);

            SimpleShader::self()->setUniform(1, squid_color[2]);
            glDrawArrays(GL_TRIANGLE_FAN, 20, 3);
        }

    private:
        SquidData()
        {
            std::vector<glm::vec2> vertices;
            for (auto const& v : squid_head) {
                vertices.push_back(v);
            }
            for (auto const& v : squid_body) {
                vertices.push_back(v);
            }
            for (auto const& v : squid_tentacle1) {
                vertices.push_back(v);
            }
            for (auto const& v : squid_tentacle2) {
                vertices.push_back(v);
            }
            for (auto const& v : squid_tentacle3) {
                vertices.push_back(v);
            }
            for (auto const& v : squid_tentacle4) {
                vertices.push_back(v);
            }
            for (auto const& v : squid_tentacle5) {
                vertices.push_back(v);
            }
            for (auto const& v : squid_eye1) {
                vertices.push_back(v);
            }
            for (auto const& v : squid_eye2) {
                vertices.push_back(v);
            }

            vbo.setData(vertices, GL_STATIC_DRAW);

            auto vertexBinding = vao.getBinding(0);
            vertexBinding.bindVertexBuffer(vbo, 0, sizeof(glm::vec2));

            auto vertexAttr = vao.enableVertexAttrib(0);
            vertexAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);
            vertexAttr.setBinding(vertexBinding);
        }
    };

    glm::vec2 baloon_body[13] = {
        { 14, 4 },
        { 12, 6 },
        { 9, 10 },
        { 7, 16 },
        { 7, 21 },
        { 9, 26 },
        { 13, 28 },
        { 20, 29 },
        { 25, 26 },
        { 28, 21 },
        { 27, 13 },
        { 23, 9 },
        { 17, 5 },
    };
    glm::vec2 baloon_tail[3] = {
        { 14, 4 },
        { 10, 1 },
        { 17, 0 },
    };

    glm::vec3 baloon_color[1] = {
        { 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
    };

    struct BaloonData {
        VertexBuffer vbo;
        VertexArray vao;

        static BaloonData* self()
        {
            static BaloonData me;
            return &me;
        }

        void render(glm::mat4 viewProjMat, glm::mat4 mat)
        {
            mat = mat * glm::translate(glm::mat4(1.0), glm::vec3(-1))
                * glm::scale(glm::mat4(1.0), glm::vec3(1.f / 16));
            SimpleShader::self()->setUniform(0, viewProjMat * mat);

            vao.use();
            SimpleShader::self()->use();

            SimpleShader::self()->setUniform(1, baloon_color[0]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 13);
            glDrawArrays(GL_TRIANGLE_FAN, 13, 3);
        }

    private:
        BaloonData()
        {
            std::vector<glm::vec2> vertices;
            for (auto const& v : baloon_body) {
                vertices.push_back(v);
            }
            for (auto const& v : baloon_tail) {
                vertices.push_back(v);
            }

            vbo.setData(vertices, GL_STATIC_DRAW);

            auto vertexBinding = vao.getBinding(0);
            vertexBinding.bindVertexBuffer(vbo, 0, sizeof(glm::vec2));

            auto vertexAttr = vao.enableVertexAttrib(0);
            vertexAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);
            vertexAttr.setBinding(vertexBinding);
        }
    };
}

RenderSystem::RenderSystem()
    : m_pointsBuf(500)
{
}

void RenderSystem::update(ECSEngine& engine, float deltaTime)
{
    SceneComponent const& scene = engine.getOne<SceneComponent>();
    Entity const& player = engine.getOneEnt<PlayerComponent>();

    //    if (m_s->damageTime + 0.3f > m_s->time) {
    //        float mag = m_s->damageTime + 0.3f - m_s->time;
    //        float t = m_s->time - m_s->damageTime;
    //        float y = mag * glm::sin(t * 80) * 0.1f;
    //        viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(y, y, 0));
    //    }

    // render background stars and projectiles
    std::vector<PointAttrib> attribs;
    for (Entity& ent : engine.iterate<StarComponent>()) {
        StarComponent star = ent.get<StarComponent>();
        PosComponent pos = ent.get<PosComponent>();

        PointAttrib attrib;
        attrib.pos = pos.pos;
        attrib.pos -= player.get<PosComponent>().pos * float(star.size) * 0.1f;
        attrib.color = star.color;
        attrib.size = star.size;
        attribs.push_back(attrib);
    }

    // projectiles
    for (Entity& proj : engine.iterate<ProjectileComponent>()) {
        PosComponent pos = proj.get<PosComponent>();

        PointAttrib attrib;
        attrib.pos = pos.pos;
        attrib.color = { 5, 5, 5 };
        attrib.size = 5;
        attribs.push_back(attrib);
    }

    m_pointsBuf.vbo.updateData(attribs);

    PointShader::self()->setUniform(0, scene.viewProjMat);

    m_pointsBuf.vao.use();
    PointShader::self()->use();
    glDrawArrays(GL_POINTS, 0, int(attribs.size()));

    // render shrapnels
    auto render = [](EnemyType type, glm::mat4 viewProjMat, glm::mat4 modelMat){
        if (type == EnemyType::HOUSE) {
            HouseData::self()->render(viewProjMat, modelMat);
        } else if (type == EnemyType::CAR) {
            CarData::self()->render(viewProjMat, modelMat);
        } else if (type == EnemyType::COCKTAIL) {
            CocktailData::self()->render(viewProjMat, modelMat);
        } else if (type == EnemyType::SQUID) {
            SquidData::self()->render(viewProjMat, modelMat);
        } else if (type == EnemyType::BALLOON) {
            BaloonData::self()->render(viewProjMat, modelMat);
        }
    };

    for (Entity& shrapnel : engine.iterate<ShrapnelComponent>()) {
        ShrapnelComponent comp = shrapnel.get<ShrapnelComponent>();
        float angle = shrapnel.get<AngleComponent>().angle;
        glm::vec2 pos = shrapnel.get<PosComponent>().pos;

        glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos, 0))
                * glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(0, 0, 1))
                * glm::scale(glm::mat4(1.0), glm::vec3(comp.scale));

        render(comp.type, scene.viewProjMat, modelMat);
    }

    // render enemies
    for (Entity& enemy : engine.iterate<EnemyComponent>()) {
        EnemyComponent comp = enemy.get<EnemyComponent>();
        CollidableComponent collide = enemy.get<CollidableComponent>();

        float damageScale = 1 + glm::max(0.0f, 0.1f - comp.timeSinceHit) * 5.f;

        glm::vec2 pos = enemy.get<PosComponent>().pos;
        pos.x -= player.get<PosComponent>().pos.x * 0.05f;

        float angle = enemy.get<AngleComponent>().angle;

        glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos, 0));
        float scale;
        switch (comp.type) {
        case EnemyType::HOUSE:
            scale = glm::sin(scene.elapsedTime * 10) * 0.5f + 1.0f;
            modelMat = modelMat
                * glm::translate(glm::mat4(1.0), glm::vec3(0, -.15f, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(1, scale, 1))
                * glm::translate(glm::mat4(1.0), glm::vec3(0, +.15f, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(collide.radius * damageScale));
            break;
        case EnemyType::CAR:
            scale = glm::sin(scene.elapsedTime * 10) * 0.5f + 1.0f;
            modelMat = modelMat
                * glm::scale(glm::mat4(1.0), glm::vec3(scale, 1, 1))
                * glm::scale(glm::mat4(1.0), glm::vec3(collide.radius * damageScale));
            break;
        case EnemyType::COCKTAIL:
            modelMat = modelMat
                * glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(0, 0, 1))
                * glm::scale(glm::mat4(1.0), glm::vec3(collide.radius * damageScale));
            break;
        case EnemyType::SQUID:
            scale = glm::sin(scene.elapsedTime * 20) * 0.3f + 1.0f;
            modelMat = modelMat
                * glm::translate(glm::mat4(1.0), glm::vec3(0, -.15f, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(1, scale, 1))
                * glm::translate(glm::mat4(1.0), glm::vec3(0, +.15f, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(collide.radius * damageScale));
            break;
        case EnemyType::BALLOON:
            modelMat = modelMat
                * glm::scale(glm::mat4(1.0), glm::vec3(collide.radius * damageScale));
            break;
        }

        render(comp.type, scene.viewProjMat, modelMat);

        HealthComponent health = enemy.get<HealthComponent>();
        glm::mat4 hpBarModelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos + glm::vec2(0, collide.radius), 0.0f))
            * glm::scale(glm::mat4(1.0), glm::vec3(0.1f, 0.01f, 0.0f));
        m_hpBar.render(scene.viewProjMat * hpBarModelMat, float(health.health) / health.maxHealth);
    }

    // render player
    float scaleFactor = (glm::sin(scene.elapsedTime * 20) * 0.1f + 2.0f) / 500.0f;
    glm::vec2 pos = player.get<PosComponent>().pos;
    float offset = scene.elapsedTime < 1 ? 1.0f - float(scene.elapsedTime) : 0.0f;
    pos -= glm::vec2(0.0f, glm::pow(offset, 2.0f) * 2.0f);
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos, 0.0f))
        * glm::scale(glm::mat4(1.0), glm::vec3(scaleFactor))
        * glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0, 0, 1));

    PlaneData::self()->render(scene.viewProjMat * modelMat);

    // render health bar
    glm::mat4 hpBarProjMat = glm::ortho(-1.0f, 1.0f,
        -scene.aspectRatio, scene.aspectRatio, -1000.0f, 1000.0f);

    float hpBarScale = 0.02f;
    glm::mat4 hpBarModelMat = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -scene.aspectRatio, 0.0f))
        * glm::scale(glm::mat4(1.0), glm::vec3(1.f, hpBarScale, 0.0f));

    HealthComponent playerHealth = player.get<HealthComponent>();
    float playerHealthRatio = float(playerHealth.health) / playerHealth.maxHealth;
    m_hpBar.render(hpBarProjMat * hpBarModelMat, playerHealthRatio);
}
}
