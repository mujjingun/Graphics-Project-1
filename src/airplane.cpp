#include "airplane.h"
#include "scene.h"
#include "shader.h"

#include <GL/glew.h>

#include <GL/freeglut.h>

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <list>

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

    Shader& plane_shader()
    {
        static Shader shader("shaders/simple.vert.glsl", "shaders/simple.frag.glsl");
        return shader;
    }
}

struct AirplaneStates {
    double time = 0;
    double lastBullet = 0;
    glm::vec2 pos{ 0, -1 };

    std::vector<glm::vec2> bullets;
};

void Airplane::render()
{
    double delta = m_scene->deltaTime();
    m_s->time += delta;

    if (m_s->time > m_s->lastBullet + 0.1) {
        m_s->lastBullet = m_s->time;
        m_s->bullets.push_back(m_s->pos + glm::vec2(0, 0.08));
    }
    for (auto& p : m_s->bullets) {
        p += glm::vec2(0, delta * 3);
    }
    // remove bullets
    m_s->bullets.erase(
        std::remove_if(m_s->bullets.begin(), m_s->bullets.end(),
            [](glm::vec2 p) {
                return p.y > 1.0f;
            }),
        m_s->bullets.end());

    double moveSpeed = 2;
    if (m_scene->isKeyPressed('d')) {
        m_s->pos.x += delta * moveSpeed;
    }
    if (m_scene->isKeyPressed('a')) {
        m_s->pos.x -= delta * moveSpeed;
    }
    if (m_scene->isKeyPressed('w')) {
        m_s->pos.y += delta * moveSpeed;
    }
    if (m_scene->isKeyPressed('s')) {
        m_s->pos.y -= delta * moveSpeed;
    }
    m_s->pos = glm::clamp(m_s->pos, -1.0f, 1.0f);

    double aspectRatio = m_scene->windowHeight() / double(m_scene->windowWidth());
    glm::mat4 projMat = glm::ortho(-1.0, 1.0, -aspectRatio, aspectRatio, -1000.0, 1000.0);
    glm::mat4 viewMat = glm::mat4(1.0f);

    double scaleFactor = (glm::sin(m_s->time * 20) * 0.2 + 2.0) / 500.0;
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(m_s->pos, 0.0f))
        * glm::scale(glm::mat4(1.0), glm::vec3(float(scaleFactor)))
        * glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0, 0, 1));

    float tiltAngle = 45;
    if (m_scene->isKeyPressed('d')) {
        modelMat = modelMat * glm::rotate(glm::mat4(1.0), glm::radians(tiltAngle), glm::vec3(0, 1, 0));
    }
    if (m_scene->isKeyPressed('a')) {
        modelMat = modelMat * glm::rotate(glm::mat4(1.0), glm::radians(-tiltAngle), glm::vec3(0, 1, 0));
    }

    m_vao.use();
    plane_shader().use();

    plane_shader().setUniform(0, projMat * viewMat * modelMat);

    plane_shader().setUniform(1, airplane_color[AIRPLANE_BIG_WING]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    plane_shader().setUniform(1, airplane_color[AIRPLANE_SMALL_WING]);
    glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

    plane_shader().setUniform(1, airplane_color[AIRPLANE_BODY]);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

    plane_shader().setUniform(1, airplane_color[AIRPLANE_BACK]);
    glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

    plane_shader().setUniform(1, airplane_color[AIRPLANE_SIDEWINDER1]);
    glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

    plane_shader().setUniform(1, airplane_color[AIRPLANE_SIDEWINDER2]);
    glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

    for (auto const& p : m_s->bullets) {
        glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(p, 0.0f));

        plane_shader().setUniform(0, projMat * viewMat * modelMat);
        plane_shader().setUniform(1, airplane_color[AIRPLANE_CENTER]);
        glPointSize(5.0);
        glDrawArrays(GL_POINTS, 32, 1);
    }
}

Airplane::Airplane(Scene* scene)
    : m_scene(scene)
    , m_s(new AirplaneStates{})
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
    m_vbo.setData(vertices, GL_STATIC_DRAW);

    auto vertexBinding = m_vao.getBinding(0);
    vertexBinding.bindVertexBuffer(m_vbo, 0, sizeof(glm::vec2));

    auto vertexAttr = m_vao.enableVertexAttrib(0);
    vertexAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);
    vertexAttr.setBinding(vertexBinding);
}

Airplane::Airplane(Airplane&&) = default;
Airplane& Airplane::operator=(Airplane&&) = default;
Airplane::~Airplane() = default;
}
