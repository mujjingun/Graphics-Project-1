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

    comp.timeSinceBullet += deltaTime;

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
