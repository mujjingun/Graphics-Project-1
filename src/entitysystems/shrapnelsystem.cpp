#include "shrapnelsystem.h"
#include "components.h"
#include "ecs/ecsengine.h"
#include "ecs/entity.h"

#include <iostream>

namespace ou {

ShrapnelSystem::ShrapnelSystem()
{
}

void ShrapnelSystem::update(ECSEngine& engine, float deltaTime)
{
    Entity& player = engine.getOneEnt<PlayerComponent>();
    SceneComponent const& scene = engine.getOne<SceneComponent>();

    engine.removeEntities<ShrapnelComponent>([&](Entity& ent) {
        ent.get<ShrapnelComponent>().elapsedTime += deltaTime;

        glm::vec2 playerPos = player.get<PosComponent>().pos;
        glm::vec2 shrapnelPos = ent.get<PosComponent>().pos;

        float playerRadius = player.get<CollidableComponent>().radius;

        if (glm::distance(playerPos, shrapnelPos) < playerRadius) {
            if (ent.get<ShrapnelComponent>().type != EntityType::PLAYER) {
                if (ent.get<ShrapnelComponent>().type == EntityType::BALLOON
                    || ent.get<ShrapnelComponent>().type == EntityType::CAKE) {
                    HealthComponent& health = player.get<HealthComponent>();
                    health.health = std::min(health.maxHealth, health.health + int(ent.get<ShrapnelComponent>().scale * 100));
                } else {
                    player.get<HealthComponent>().health -= 1;
                    player.get<PlayerComponent>().timeSinceHit = 0;
                }
                return true;
            }
        }

        return shrapnelPos.y < -scene.aspectRatio || ent.get<ShrapnelComponent>().elapsedTime > 4;
    });
}
}
