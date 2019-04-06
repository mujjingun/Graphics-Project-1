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

    engine.removeEntities<ShrapnelComponent>([&](Entity& ent) {
        ent.get<ShrapnelComponent>().elapsedTime += deltaTime;

        glm::vec2 playerPos = player.get<PosComponent>().pos;
        glm::vec2 shrapnelPos = ent.get<PosComponent>().pos;

        float playerRadius = player.get<CollidableComponent>().radius;

        if (glm::distance(playerPos, shrapnelPos) < playerRadius) {
            if (ent.get<ShrapnelComponent>().type != EntityType::PLAYER) {
                if (ent.get<ShrapnelComponent>().type == EntityType::BALLOON) {
                    HealthComponent& health = player.get<HealthComponent>();
                    health.health = std::min(health.maxHealth, health.health + 5);
                } else {
                    player.get<HealthComponent>().health -= 1;
                    player.get<PlayerComponent>().timeSinceHit = 0;
                }
                return true;
            }
        }

        return ent.get<ShrapnelComponent>().elapsedTime > 2;
    });
}
}
