#include "background.h"
#include "pointdata.h"
#include "pointshader.h"
#include "scene.h"
#include "vertexbuffer.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <list>
#include <random>

namespace ou {

namespace {
    const int MAX_STARS = 200;
}

struct Star {
    glm::vec2 pos;
    int size;
    glm::vec3 color;
};

struct BackgroundStates {
    PointData buf{ MAX_STARS };
    std::list<Star> stars{};

    double time = 0;
    double nextStarTime = -20;

    std::random_device dev{};
    std::mt19937 gen{ dev() };
};

Background::Background(Scene* scene)
    : m_scene(scene)
    , m_s(new BackgroundStates{})
{
}

void Background::render()
{
    double delta = m_scene->deltaTime();
    m_s->time += delta;

    // 10 occurances every second
    std::exponential_distribution<> timeDist(20);
    std::uniform_real_distribution<> xDist(-1, 1);
    std::discrete_distribution<> distanceDist({ 5, 4, 3, 2, 1 });

    glm::vec3 colors[] = {
        { 161, 191, 255 },
        { 223, 231, 255 },
        { 255, 249, 255 },
        { 255, 191, 126 },
        { 255, 121, 0 },
    };

    std::uniform_int_distribution<> colorDist(0, 4);
    while (m_s->stars.size() <= MAX_STARS && m_s->time > m_s->nextStarTime) {
        // make a new star
        Star star;
        star.size = distanceDist(m_s->gen) + 1;
        star.color = colors[colorDist(m_s->gen)] / 255.0f * 0.4f;

        float y = m_scene->aspectRatio();
        y -= (m_s->time - m_s->nextStarTime) * star.size * 0.5;
        star.pos = glm::vec2(xDist(m_s->gen), y);

        if (star.pos.y >= -m_scene->aspectRatio()) {
            m_s->stars.push_back(star);
        }

        double timeUntilNext = timeDist(m_s->gen);
        m_s->nextStarTime += timeUntilNext;
    }

    for (auto& star : m_s->stars) {
        star.pos.y -= delta * star.size * 0.5;
    }

    m_s->stars.erase(
        std::remove_if(m_s->stars.begin(), m_s->stars.end(),
            [&](Star const& p) {
                return p.pos.y < -m_scene->aspectRatio();
            }),
        m_s->stars.end());

    std::vector<PointAttrib> attribs;
    for (auto const& star : m_s->stars) {
        attribs.push_back(PointAttrib{ star.pos, float(star.size), star.color });
    }
    m_s->buf.vbo.updateData(attribs);

    PointShader::self()->setUniform(0, m_scene->viewProjMat());

    m_s->buf.vao.use();
    PointShader::self()->use();
    glDrawArrays(GL_POINTS, 0, int(m_s->stars.size()));
}

Background::~Background() = default;
Background::Background(Background&&) = default;
Background& Background::operator=(Background&&) = default;
}
