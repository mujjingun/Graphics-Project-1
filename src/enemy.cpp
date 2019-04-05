#include "enemy.h"

#include "airplane.h"
#include "hpbar.h"
#include "scene.h"
#include "simpleshader.h"
#include "vertexarray.h"
#include "vertexbuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <random>
#include <vector>

namespace ou {

namespace {
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

struct Shrapnel {
    glm::vec2 pos;
    glm::vec2 vel;
    float rotation;
    float rotationSpeed;
    float scale;
};

struct HouseStates {
    Enemy::Type type;
    glm::vec2 pos;
    float speed;
    float slideSpeed;
    float radius = 0.15f;
    float angle;
    float angleSpeed;
    float time = 0;
    float lastHitTime = -1;
    int hitCount = 0;
    int maxHitCount = 10;
    int shrapnelCount = 10;
    bool isExploding = false;
    float explosionTime;
    std::vector<Shrapnel> explodeBits;
    HpBar hpBar;

    std::mt19937 gen{ std::random_device{}() };
};

void Enemy::render()
{
    const double delta = m_scene->deltaTime();
    m_s->time += delta;

    if (!m_s->isExploding) {
        m_s->pos.x += float(delta) * m_s->slideSpeed;

        if (m_s->pos.x > 1.0f || m_s->pos.x < -1.0f) {
            m_s->slideSpeed = -m_s->slideSpeed;
        }

        m_s->pos.y -= float(delta) * m_s->speed;
        m_s->angle += m_s->angleSpeed * float(delta);

        float damageScale = 1 + glm::max(0.0f, m_s->lastHitTime - m_s->time + 0.2f) * 1.5f;

        glm::vec2 pos = m_s->pos;
        pos.x -= m_scene->airplane()->pos().x * 0.05f;

        if (m_s->type == Type::HOUSE) {
            float scale = glm::sin(m_s->time * 10) * 0.5f + 1.0f;
            glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos, 0))
                * glm::translate(glm::mat4(1.0), glm::vec3(0, -.15f, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(1, scale, 1))
                * glm::translate(glm::mat4(1.0), glm::vec3(0, +.15f, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(m_s->radius * damageScale));
            HouseData::self()->render(m_scene->viewProjMat(), modelMat);
        } else if (m_s->type == Type::CAR) {
            float scale = glm::sin(m_s->time * 10) * 0.5f + 1.0f;
            glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(scale, 1, 1))
                * glm::scale(glm::mat4(1.0), glm::vec3(m_s->radius * damageScale));
            CarData::self()->render(m_scene->viewProjMat(), modelMat);
        } else if (m_s->type == Type::COCKTAIL) {
            glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos, 0))
                * glm::rotate(glm::mat4(1.0), glm::radians(m_s->angle), glm::vec3(0, 0, 1))
                * glm::scale(glm::mat4(1.0), glm::vec3(m_s->radius * damageScale));
            CocktailData::self()->render(m_scene->viewProjMat(), modelMat);
        } else if (m_s->type == Type::SQUID) {
            float scale = glm::sin(m_s->time * 20) * 0.3f + 1.0f;
            glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos, 0))
                * glm::translate(glm::mat4(1.0), glm::vec3(0, -.15f, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(1, scale, 1))
                * glm::translate(glm::mat4(1.0), glm::vec3(0, +.15f, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(m_s->radius * damageScale));
            SquidData::self()->render(m_scene->viewProjMat(), modelMat);
        } else if (m_s->type == Type::BALOON) {
            glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(m_s->radius * damageScale));
            BaloonData::self()->render(m_scene->viewProjMat(), modelMat);
        }

        glm::mat4 hpBarModelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos + glm::vec2(0, m_s->radius), 0.0f))
            * glm::scale(glm::mat4(1.0), glm::vec3(0.1f, 0.01f, 0.0f));
        m_s->hpBar.render(m_scene->viewProjMat() * hpBarModelMat, 1 - float(m_s->hitCount) / m_s->maxHitCount);
    } else {
        for (auto& bit : m_s->explodeBits) {
            bit.pos += bit.vel * float(delta);
            bit.rotation += bit.rotationSpeed * float(delta);
        }

        for (auto const& bit : m_s->explodeBits) {
            glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(bit.pos, 0.0f))
                * glm::scale(glm::mat4(1.0), glm::vec3(bit.scale))
                * glm::rotate(glm::mat4(1.0), glm::radians(bit.rotation), glm::vec3(0, 0, 1));

            if (m_s->type == Type::HOUSE) {
                HouseData::self()->render(m_scene->viewProjMat(), modelMat);
            } else if (m_s->type == Type::CAR) {
                CarData::self()->render(m_scene->viewProjMat(), modelMat);
            } else if (m_s->type == Type::COCKTAIL) {
                CocktailData::self()->render(m_scene->viewProjMat(), modelMat);
            } else if (m_s->type == Type::SQUID) {
                SquidData::self()->render(m_scene->viewProjMat(), modelMat);
            } else if (m_s->type == Type::BALOON) {
                BaloonData::self()->render(m_scene->viewProjMat(), modelMat);
            }
        }
    }
}

// taken and modified from glm.
bool intersectLineSphere(
    glm::vec2 const& point0, glm::vec2 const& point1,
    glm::vec2 const& sphereCenter, float sphereRadius,
    glm::vec2& intersectionPoint1, glm::vec2& intersectionNormal1,
    glm::vec2& intersectionPoint2, glm::vec2& intersectionNormal2)
{
    float Epsilon = std::numeric_limits<float>::epsilon();
    glm::vec2 dir = normalize(point1 - point0);
    glm::vec2 diff = sphereCenter - point0;
    float t0 = dot(diff, dir);
    float dSquared = dot(diff, diff) - t0 * t0;
    if (dSquared > sphereRadius * sphereRadius) {
        return false;
    }
    float t1 = glm::sqrt(sphereRadius * sphereRadius - dSquared);
    if (t0 < t1 + Epsilon) {
        t1 = -t1;
    }
    float len = length(point1 - point0);
    if (t0 - t1 > len || t0 + t1 < len) {
        return false;
    }
    intersectionPoint1 = point0 + dir * (t0 - t1);
    intersectionNormal1 = (intersectionPoint1 - sphereCenter) / sphereRadius;
    intersectionPoint2 = point0 + dir * (t0 + t1);
    intersectionNormal2 = (intersectionPoint2 - sphereCenter) / sphereRadius;
    return true;
}

bool Enemy::collide(glm::vec2 a, glm::vec2 b)
{
    if (m_s->isExploding) {
        return false;
    }

    glm::vec2 i1, i2, o1, o2;
    if (!intersectLineSphere(a, b, m_s->pos, m_s->radius, i1, i2, o1, o2)) {
        return false;
    }

    // collide
    m_s->lastHitTime = m_s->time;
    m_s->radius += 0.002;
    m_s->hitCount++;
    m_s->pos.y += 0.02;

    std::normal_distribution<> normalDist;
    m_s->angleSpeed = float(normalDist(m_s->gen)) * 360;

    if (m_s->hitCount >= m_s->maxHitCount) {
        m_s->isExploding = true;
        m_s->explosionTime = m_s->time;

        for (int i = 0; i < m_s->shrapnelCount; ++i) {
            std::uniform_real_distribution<> angleDist(0, 360);
            Shrapnel bit;
            bit.pos = m_s->pos + glm::vec2(normalDist(m_s->gen), normalDist(m_s->gen)) * m_s->radius;
            double angle = angleDist(m_s->gen);
            bit.vel = glm::vec2(glm::cos(angle), glm::sin(angle)) * float(3 + normalDist(m_s->gen));
            bit.rotation = float(angle);
            bit.rotationSpeed = float(normalDist(m_s->gen) * 360);
            bit.scale = float(normalDist(m_s->gen) * 0.02 + 0.05);
            m_s->explodeBits.push_back(bit);
        }
    }
    return true;
}

bool Enemy::doRemove()
{
    bool isFailure = m_s->isExploding && m_s->explosionTime + 2 < m_s->time;
    return isFailure || isSuccess();
}

bool Enemy::isSuccess()
{
    return m_s->pos.y < -m_scene->aspectRatio() - 0.2f;
}

Enemy::Enemy(Scene* scene, Type type, float x, float fastness)
    : m_scene(scene)
    , m_s(new HouseStates{})
{
    m_s->type = type;
    m_s->pos = { x, scene->aspectRatio() + 0.2f };

    if (m_s->type == Type::HOUSE) {
        m_s->radius = 0.15f;
        m_s->speed = 0.2f;
        m_s->maxHitCount = 10;
        m_s->shrapnelCount = 20;
        m_s->slideSpeed = 0;
    } else if (m_s->type == Type::CAR) {
        m_s->radius = 0.10f;
        m_s->speed = 0.5f;
        m_s->maxHitCount = 5;
        m_s->shrapnelCount = 10;
        m_s->slideSpeed = 0.5f;
    } else if (m_s->type == Type::COCKTAIL) {
        m_s->radius = 0.08f;
        m_s->speed = 0.4f;
        m_s->maxHitCount = 2;
        m_s->shrapnelCount = 5;
        m_s->slideSpeed = 0;
    } else if (m_s->type == Type::SQUID) {
        m_s->radius = 0.2f;
        m_s->speed = 0.1f;
        m_s->maxHitCount = 30;
        m_s->shrapnelCount = 20;
        m_s->slideSpeed = 0;
    } else if (m_s->type == Type::BALOON) {
        // TODO: make baloons change color
        m_s->radius = 0.08f;
        m_s->speed = 0.1f;
        m_s->maxHitCount = 1;
        m_s->shrapnelCount = 5;
        m_s->slideSpeed = 0.1f;
    }

    m_s->speed += fastness;

    std::normal_distribution<> normalDist;
    std::uniform_real_distribution<> angleDist(0, 360);
    m_s->angle = float(angleDist(m_s->gen));
    m_s->angleSpeed = float(normalDist(m_s->gen)) * 360;
}

Enemy::~Enemy() = default;

Enemy::Enemy(Enemy&&) = default;

Enemy& Enemy::operator=(Enemy&&) = default;
}
