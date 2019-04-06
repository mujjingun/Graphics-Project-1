#include "projectilesystem.h"
#include "components.h"
#include "ecs/ecsengine.h"

#include <iostream>

namespace ou {

ProjectileSystem::ProjectileSystem()
{
}

void ProjectileSystem::update(ECSEngine& engine, float)
{
    SceneComponent scene = engine.getOne<SceneComponent>();

    engine.removeEntities<ProjectileComponent>([&](Entity& ent) {
        glm::vec2 pos = ent.get<PosComponent>().pos;
        bool isOutOfScreen = pos.y < -scene.aspectRatio || pos.y > scene.aspectRatio
            || pos.x < -1 || pos.x > 1;
        return isOutOfScreen;
    });

    for (Entity& bullet : engine.iterate<ProjectileComponent>()) {
        glm::vec2 bulletPos = bullet.get<PosComponent>().pos;

        for (Entity& collidable : engine.iterate<CollidableComponent>()) {
            glm::vec2 enemyPos = collidable.get<PosComponent>().pos;
            CollidableComponent& collide = collidable.get<CollidableComponent>();

            if (collidable.has<PlayerComponent>()) {
                continue;
            }

            // collision detection
            if (glm::distance(enemyPos, bulletPos) < collide.radius) {
                bullet.addComponent(CollisionComponent{});
                collidable.addComponent(CollisionComponent{});
            }
        }
    }

    engine.removeEntities<ProjectileComponent, CollisionComponent>();
}
}
