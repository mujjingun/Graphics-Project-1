#include "projectilesystem.h"
#include "components.h"
#include "ecs/ecsengine.h"

#include <iostream>

namespace ou {

ProjectileSystem::ProjectileSystem()
{
}

void ProjectileSystem::update(ECSEngine& engine, float deltaTime)
{
    SceneComponent scene = engine.getOne<SceneComponent>();

    engine.removeEntities<ProjectileComponent>([&](Entity& ent) {
        glm::vec2 pos = ent.get<PosComponent>().pos;
        bool isOutOfScreen = pos.y < -scene.aspectRatio || pos.y > scene.aspectRatio;
        return isOutOfScreen;
    });

    for (Entity& bullet : engine.iterate<ProjectileComponent>()) {
        glm::vec2 bulletPos = bullet.get<PosComponent>().pos;
		bullet.get<ProjectileComponent>().elapsedTime += deltaTime;

        for (Entity& collidable : engine.iterate<CollidableComponent>()) {
            glm::vec2 enemyPos = collidable.get<PosComponent>().pos;
            CollidableComponent& collide = collidable.get<CollidableComponent>();

            if (collidable.has<PlayerComponent>() && bullet.get<ProjectileComponent>().elapsedTime < 0.3f) {
                continue;
            }

            // collision detection
			float dist = glm::distance(enemyPos, bulletPos);
            if (dist < collide.radius) {
				glm::vec2 normal = glm::normalize(bulletPos - enemyPos);
				glm::vec2 diff = normal * (collide.radius - dist);
				bullet.get<PosComponent>().pos += diff;
				bullet.get<VelComponent>().vel = glm::length(bullet.get<VelComponent>().vel) * 0.8f * normal;
				bullet.get<ProjectileComponent>().count -= 1;
                collidable.addComponent(CollisionComponent{ bullet.get<ProjectileComponent>().type });
            }
        }
    }

    engine.removeEntities<ProjectileComponent>(
        [](Entity& ent) {
			ProjectileComponent const &comp = ent.get<ProjectileComponent>();
			return comp.count <= 0 || comp.elapsedTime > 5;
        });
}
}
