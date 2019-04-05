#include "airplane.h"
#include "enemy.h"
#include "hpbar.h"
#include "pointdata.h"
#include "pointshader.h"
#include "scene.h"
#include "simpleshader.h"

#include <GL/glew.h>

#include <GL/freeglut.h>

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <list>
#include <random>

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

        void render()
        {
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

    const int MAX_BULLETS = 200;
}

struct Shrapnel {
    glm::vec2 pos;
    glm::vec2 vel;
    float rotation;
    float rotationSpeed;
    float scale;
};

struct AirplaneStates {
    float time = 0;
    float lastBullet = 0;
    glm::vec2 pos{ 0, -1 };
    glm::vec2 dest{ 0, -1 };

    int health = 100;
    int maxHealth = 100;
    std::vector<glm::vec2> bullets;

    PointData bulletBuf{ MAX_BULLETS };

    bool isExploding;
    std::list<Shrapnel> shrapnels;
    float explodeTime;

    std::mt19937 gen{ std::random_device{}() };
};

void Airplane::render()
{
    const double delta = m_scene->deltaTime();
    m_s->time += delta;

    // shoot bullets
    if (m_s->time >= 1 && !m_s->isExploding) {
        if (m_s->time > m_s->lastBullet + 0.05f) {
            m_s->lastBullet = m_s->time;
            m_s->bullets.push_back(m_s->pos + glm::vec2(0, 0.08));
        }
    }

    for (auto& p : m_s->bullets) {
        p += glm::vec2(0, delta * 3);
    }

    // remove bullets
    m_s->bullets.erase(
        std::remove_if(m_s->bullets.begin(), m_s->bullets.end(),
            [&](glm::vec2 p) {
                bool leftScreen = (p.y > m_scene->aspectRatio());
                bool isCollide = false;
                for (const auto& enemy : m_scene->enemies()) {
                    if (enemy->collide(p + glm::vec2(0, -1), p)) {
                        isCollide = true;
                        break;
                    }
                }
                return leftScreen || isCollide;
            }),
        m_s->bullets.end());

    // move plane
    glm::vec2 moveDelta = {};
    if (m_scene->isKeyPressed('d')) {
        moveDelta.x += 1;
    }
    if (m_scene->isKeyPressed('a')) {
        moveDelta.x -= 1;
    }
    if (m_scene->isKeyPressed('w')) {
        moveDelta.y += 1;
    }
    if (m_scene->isKeyPressed('s')) {
        moveDelta.y -= 1;
    }

    // normalize speed
    double moveSpeed = 2;
    if (glm::length(moveDelta) > 0) {
        moveDelta = glm::normalize(moveDelta) * float(delta * moveSpeed);
    }
    m_s->dest = glm::clamp(m_s->dest + moveDelta,
        glm::vec2(-1.0f, -m_scene->aspectRatio()), glm::vec2(1.0f, m_scene->aspectRatio()));

    // apply smoothing
    double smoothing = 1 - glm::exp(-delta * 15);
    glm::vec2 smoothedDelta = (m_s->dest - m_s->pos) * float(smoothing);
    m_s->pos += smoothedDelta;

    // build transformation matrices
    float scaleFactor = (glm::sin(m_s->time * 20) * 0.1f + 2.0f) / 500.0f;
    glm::vec2 pos = m_s->pos;
    float offset = m_s->time < 1 ? 1.0f - float(m_s->time) : 0.0f;
    pos -= glm::vec2(0.0f, glm::pow(offset, 2.0f) * 2.0f);
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos, 0.0f))
        * glm::scale(glm::mat4(1.0), glm::vec3(scaleFactor))
        * glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0, 0, 1));

    if (!m_s->isExploding) {
        // render the plane
        SimpleShader::self()->setUniform(0, m_scene->viewProjMat() * modelMat);
        PlaneData::self()->render();
    } else {
        for (auto& bit : m_s->shrapnels) {
            bit.pos += bit.vel * float(delta);
            bit.rotation += bit.rotationSpeed * float(delta);
        }

        for (auto const& bit : m_s->shrapnels) {
            glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(bit.pos, 0.0f))
                * glm::scale(glm::mat4(1.0), glm::vec3(scaleFactor * bit.scale))
                * glm::rotate(glm::mat4(1.0), glm::radians(bit.rotation), glm::vec3(0, 0, 1));

            SimpleShader::self()->setUniform(0, m_scene->viewProjMat() * modelMat);

            PlaneData::self()->render();
        }
    }

    // render bullets
    std::vector<PointAttrib> bullets;
    for (auto const& p : m_s->bullets) {
        bullets.push_back(PointAttrib{ p, 5.0f, airplane_color[AIRPLANE_CENTER] * 5.0f });
    }
    m_s->bulletBuf.vbo.updateData(bullets);

    PointShader::self()->setUniform(0, m_scene->viewProjMat());

    m_s->bulletBuf.vao.use();
    PointShader::self()->use();
    glDrawArrays(GL_POINTS, 0, int(bullets.size()));
}

void Airplane::takeDamage()
{
    if (m_s->isExploding) {
        return;
    }

    m_s->health -= 5;

    if (m_s->health <= 0) {
        m_s->isExploding = true;
        m_s->explodeTime = m_s->time;

        std::normal_distribution<> normalDist;
        for (int i = 0; i < 30; ++i) {
            std::uniform_real_distribution<> angleDist(0, 360);
            Shrapnel bit;
            bit.pos = m_s->pos + glm::vec2(normalDist(m_s->gen), normalDist(m_s->gen)) * 0.1f;
            double angle = angleDist(m_s->gen);
            bit.vel = glm::vec2(glm::cos(angle), glm::sin(angle)) * float(3 + normalDist(m_s->gen));
            bit.rotation = float(angle);
            bit.rotationSpeed = float(normalDist(m_s->gen) * 360);
            bit.scale = float(normalDist(m_s->gen) * 0.05 + 0.3);
            m_s->shrapnels.push_back(bit);
        }
    }
}

float Airplane::healthRatio() const
{
    return float(m_s->health) / m_s->maxHealth;
}

bool Airplane::isGameOver()
{
    return m_s->isExploding && m_s->time > m_s->explodeTime + 2;
}

glm::vec2 Airplane::pos() const
{
    return m_s->pos;
}

Airplane::Airplane(Scene* scene)
    : m_scene(scene)
    , m_s(new AirplaneStates{})
{
}

Airplane::Airplane(Airplane&&) = default;
Airplane& Airplane::operator=(Airplane&&) = default;
Airplane::~Airplane() = default;
}
