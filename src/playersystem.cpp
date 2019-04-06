#include "playersystem.h"
#include "components.h"
#include "ecsengine.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace ou {

PlayerSystem::PlayerSystem()
{
}

void PlayerSystem::update(ECSEngine& engine, float deltaTime)
{
    SceneComponent const& scene = engine.getOne<SceneComponent>();
    InputComponent const& input = engine.getOne<InputComponent>();

    Entity& player = engine.getOneEnt<PlayerComponent>();
    PlayerComponent& comp = player.get<PlayerComponent>();
    PosComponent& pos = player.get<PosComponent>();
    HealthComponent& health = player.get<HealthComponent>();

    comp.timeSinceBullet += deltaTime;
    comp.timeSinceHit += deltaTime;
    comp.timeSinceDead += deltaTime;

    // player dead
    if (!comp.isDead && health.health <= 0) {
        comp.isDead = true;
        comp.timeSinceDead = 0;

        std::normal_distribution<float> normalDist;
        std::uniform_real_distribution<float> angleDist(0, 360);

        for (int i = 0; i < 30; ++i) {
            ShrapnelComponent shrapnel;
            shrapnel.type = EntityType::PLAYER;
            shrapnel.scale = normalDist(engine.rand()) * 0.01f + 0.02f;
            shrapnel.elapsedTime = 0;

            AngleComponent angle;
            angle.angle = angleDist(engine.rand());

            AngleSpeedComponent angleSpeed;
            angleSpeed.angleSpeed = normalDist(engine.rand()) * 360;

            PosComponent pos;
            pos.pos = player.get<PosComponent>().pos;

            VelComponent vel;
            float velAngle = angleDist(engine.rand());
            vel.vel = glm::vec2(glm::cos(velAngle), glm::sin(velAngle)) * float(3 + normalDist(engine.rand()));

            engine.addEntity(Entity({ shrapnel, pos, vel, angle, angleSpeed }));
        }
    }

    if (comp.isDead) {
        return;
    }

    float bulletInterval = 0.05f;
    while (comp.timeSinceBullet > bulletInterval) {
        comp.timeSinceBullet -= bulletInterval;

        ProjectileComponent projectile;
        projectile.type = ProjectileComponent::Type::Player;

        PosComponent bulletPos;
        bulletPos.pos = pos.pos + glm::vec2(0, 0.1f);

        VelComponent vel;
        vel.vel = { 0, 3.0f };

        engine.addEntity(Entity({ projectile, bulletPos, vel }));
    }

    // move plane
    glm::vec2 moveDelta = {};
    if (input.isKeyPressed('d')) {
        moveDelta.x += 1;
    }
    if (input.isKeyPressed('a')) {
        moveDelta.x -= 1;
    }
    if (input.isKeyPressed('w')) {
        moveDelta.y += 1;
    }
    if (input.isKeyPressed('s')) {
        moveDelta.y -= 1;
    }

    // normalize speed
    float moveSpeed = 2;
    if (glm::length(moveDelta) > 0) {
        moveDelta = glm::normalize(moveDelta) * deltaTime * moveSpeed;
    }
    comp.dest = glm::clamp(comp.dest + moveDelta,
        -glm::vec2(1.0f, scene.aspectRatio), glm::vec2(1.0f, scene.aspectRatio));

    // apply smoothing
    float smoothing = 1 - glm::exp(-deltaTime * 15);
    glm::vec2 smoothedDelta = (comp.dest - pos.pos) * smoothing;
    pos.pos += smoothedDelta;
}
}
