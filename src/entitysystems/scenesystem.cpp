#include "scenesystem.h"
#include "components.h"
#include "ecs/ecsengine.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ou {

SceneSystem::SceneSystem()
{
}

void SceneSystem::update(ECSEngine& engine, float deltaTime)
{
    SceneComponent& scene = engine.getOne<SceneComponent>();
    scene.elapsedTime += deltaTime;

    PlayerComponent playerComp = engine.getOne<PlayerComponent>();
    if (playerComp.isDead && playerComp.timeSinceDead > 3) {

        // reset game
        Entity& player = engine.getOneEnt<PlayerComponent>();
        player.get<PlayerComponent>().dest = { 0, -1 };
        player.get<PlayerComponent>().timeSinceBullet = -1;
        player.get<PlayerComponent>().timeSinceHit = 10;
        player.get<PlayerComponent>().isDead = false;

        player.get<PosComponent>().pos = { 0, -scene.aspectRatio };

        player.get<HealthComponent>().health = 100;
        player.get<HealthComponent>().maxHealth = 100;

        engine.removeEntities<ShrapnelComponent>();
        engine.removeEntities<EnemyComponent>();
        engine.removeEntities<ProjectileComponent>();

        scene.elapsedTime = 0;
        scene.score = 0;
    }
}
}
