#include "playersystem.h"
#include "components.h"
#include "ecs/ecsengine.h"

#include <glm/glm.hpp>
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
    ApparentVelComponent& appVel = player.get<ApparentVelComponent>();

    comp.timeSinceHit += deltaTime;

	if (player.has<CollisionComponent>()) {
		player.get<PlayerComponent>().timeSinceHit = 0;

		switch (player.get<CollisionComponent>().type) {
		case ProjectileComponent::Type::Bullet:
			health.health -= 1;
			break;
		case ProjectileComponent::Type::Missile:
			health.health -= 5;
			break;
		}

		player.removeComponent<CollisionComponent>();
	}

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
        comp.timeSinceDead += deltaTime;
        return;
    }

    // shoot bullets
    comp.timeSinceBullet += deltaTime;

    float bulletInterval = 0.05f;
    while (comp.timeSinceBullet > bulletInterval) {
        comp.timeSinceBullet -= bulletInterval;

        ProjectileComponent projectile;
        projectile.type = ProjectileComponent::Type::Bullet;
		projectile.count = 3;
		projectile.elapsedTime = 0;

        VelComponent vel;
        vel.vel = { 0, 3.0f };
        vel.vel.x += player.get<ApparentVelComponent>().vel.x;

        PosComponent bulletPos;
        bulletPos.pos = pos.pos + glm::vec2(0, 0.1f) + vel.vel * comp.timeSinceBullet;

        engine.addEntity(Entity({ projectile, bulletPos, vel }));
    }

    // shoot missiles
    if (input.isKeyPressed(' ')) {
        comp.timeSinceMissile += deltaTime;

        float missileInterval = 0.5f;
        while (comp.timeSinceMissile > missileInterval) {
            comp.timeSinceMissile -= missileInterval;

            ProjectileComponent projectile;
            projectile.type = ProjectileComponent::Type::Missile;
			projectile.count = 3;
			projectile.elapsedTime = 0;

            VelComponent vel;
            vel.vel = { 0, 2.5f };
            vel.vel += player.get<ApparentVelComponent>().vel;

            PosComponent bulletPos;
            bulletPos.pos = pos.pos + glm::vec2(0.09f, -0.05f) + vel.vel * comp.timeSinceBullet;

            engine.addEntity(Entity({ projectile, bulletPos, vel }));

            bulletPos.pos = pos.pos + glm::vec2(-0.09f, -0.05f) + vel.vel * comp.timeSinceBullet;

            engine.addEntity(Entity({ projectile, bulletPos, vel }));
        }
    } else {
        comp.timeSinceMissile = 0.5f;
    }

    // leave streaks
    comp.timeSinceStreak += deltaTime;
    comp.streakColorElapsed += deltaTime;

    float timeSinceStreakOrig = comp.timeSinceStreak;

    float streakInterval = 0.05f;
    while (comp.timeSinceStreak > streakInterval) {
        comp.timeSinceStreak -= streakInterval;

        StreakComponent streak;
        streak.color = glm::mix(comp.lastStreakColor, comp.nextStreakColor, comp.streakColorElapsed);

        VelComponent vel = { { 0, -2.0f } };

        float offset = glm::pow(scene.elapsedTime < 1 ? 1.0f - float(scene.elapsedTime) : 0.0f, 2.0f);

        PosComponent streakPos;
        streakPos.pos = pos.pos + glm::vec2(0.05f, -0.08f) + vel.vel * comp.timeSinceStreak;
        streakPos.pos.y -= offset;
        streakPos.pos += appVel.vel * (timeSinceStreakOrig - comp.timeSinceStreak);

        engine.addEntity(Entity({ streak, streakPos, vel }));

        streakPos.pos = pos.pos + glm::vec2(-0.05f, -0.08f) + vel.vel * comp.timeSinceStreak;
        streakPos.pos.y -= offset;
        streakPos.pos += appVel.vel * (timeSinceStreakOrig - comp.timeSinceStreak);
        engine.addEntity(Entity({ streak, streakPos, vel }));
    }

    if (comp.streakColorElapsed >= 1) {
        comp.streakColorElapsed -= 1;

        std::uniform_real_distribution<float> colorDist(0, 0.7f);

        comp.lastStreakColor = comp.nextStreakColor;
        comp.nextStreakColor = glm::vec3(colorDist(engine.rand()), colorDist(engine.rand()), 1);
    }

    engine.removeEntities<StreakComponent>([&](Entity& ent) {
        return ent.get<PosComponent>().pos.y < -scene.aspectRatio;
    });

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
    float moveSpeed = 2.0f;
    if (glm::length(moveDelta) > 0) {
        moveDelta = glm::normalize(moveDelta) * deltaTime * moveSpeed;
    }
    comp.dest = glm::clamp(comp.dest + moveDelta,
        -glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, scene.aspectRatio));

    // apply smoothing
    float smoothing = 1 - glm::exp(-deltaTime * 15);
    glm::vec2 smoothedDelta = (comp.dest - pos.pos) * smoothing;
    pos.pos += smoothedDelta;

    // suicide
    if (input.isKeyPressed('r')) {
        health.health = 0;
        comp.timeSinceHit = 0;
    }
}
}
