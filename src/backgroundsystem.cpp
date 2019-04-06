#include "backgroundsystem.h"
#include "components.h"
#include "ecsengine.h"
#include "entity.h"

#include <iostream>

namespace ou {

BackgroundSystem::BackgroundSystem()
{
}

void BackgroundSystem::update(ECSEngine& engine, float deltaTime)
{
    m_elapsed += deltaTime;

    // number of occurances every second
    std::exponential_distribution<float> timeDist(20);
    std::uniform_real_distribution<float> xDist(-1.5, 1.5);
    std::discrete_distribution<> distanceDist({ 25, 16, 9, 4, 1 });

    glm::vec3 colors[] = {
        { 161, 191, 255 },
        { 223, 231, 255 },
        { 255, 249, 255 },
        { 255, 191, 126 },
        { 255, 121, 0 },
    };

    SceneComponent const& scene = engine.getOne<SceneComponent>();

    std::uniform_int_distribution<> colorDist(0, 4);

    // TODO: cap number of stars
    while (m_elapsed > m_next) {
        // make a new star
        StarComponent star;
        star.size = distanceDist(engine.rand()) + 1;
        star.color = colors[colorDist(engine.rand())] / 255.0f * 0.7f;

        PosComponent pos;
        float y = scene.aspectRatio * 2;
        y -= (m_elapsed - m_next) * star.size * 0.5f;
        pos.pos = glm::vec2(xDist(engine.rand()), y);

        VelComponent vel;
        vel.vel = glm::vec2(0, -star.size * 0.5);

        if (pos.pos.y >= -scene.aspectRatio) {
            engine.addEntity(Entity({ star, pos, vel }));
        }

        m_elapsed -= m_next;
        m_next = timeDist(engine.rand());
    }

    engine.removeEntities<StarComponent>([&](Entity& ent) {
        return ent.get<PosComponent>().pos.y < -scene.aspectRatio * 2;
    });
}
}
