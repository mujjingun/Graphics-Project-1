#include "house.h"

#include "scene.h"
#include "simpleshader.h"
#include "vertexarray.h"
#include "vertexbuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
        { 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
    };
}

struct PointData {
    VertexBuffer vbo;
    VertexArray vao;

    static PointData* self()
    {
        static PointData sself;
        return &sself;
    }

private:
    PointData()
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

struct HouseStates {
    float time;
};

void House::render()
{
    m_s->time += m_scene->deltaTime();

    m_pos.y -= m_scene->deltaTime() * 0.2;

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(m_pos, 0.0f))
        * glm::scale(glm::mat4(1.0), glm::vec3(0.005f))
            * glm::rotate(glm::mat4(1.0), m_s->time, glm::vec3(0, 0, 1));

    PointData::self()->vao.use();
    SimpleShader::self()->use();

    SimpleShader::self()->setUniform(0, m_scene->viewProjMat() * modelMat);

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

House::House(Scene* scene)
    : Enemy(glm::vec2(0, scene->aspectRatio()))
    , m_scene(scene)
    , m_s(new HouseStates{})
{
}

House::~House() = default;

House::House(House&&) = default;

House& House::operator=(House&&) = default;
}
