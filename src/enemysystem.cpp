#include "enemysystem.h"
#include "components.h"
#include "ecsengine.h"

#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace ou {

EnemySystem::EnemySystem()
{
}

void EnemySystem::update(ECSEngine& engine, float deltaTime)
{
    SceneComponent const& scene = engine.getOne<SceneComponent>();
    Entity& player = engine.getOneEnt<PlayerComponent>();

    std::uniform_real_distribution<> posDist(-1, 1);
    std::discrete_distribution<> typeDist({ 2, 1, 2, 1, 2 });
    std::discrete_distribution<> cntDist({ 0, 3, 4, 3, 2, 1 });
    EnemyType types[] = {
        EnemyType::CAR,
        EnemyType::HOUSE,
        EnemyType::COCKTAIL,
        EnemyType::SQUID,
        EnemyType::BALLOON,
    };

    m_elapsed += deltaTime;

    if (m_elapsed > m_nextEnemyTime) {
        m_elapsed -= m_nextEnemyTime;

        // make a new row of enemies
        int cnt = cntDist(m_gen);
        EnemyType type = types[typeDist(m_gen)];

        if (type == EnemyType::SQUID) {
            cnt = 1;
            m_nextEnemyTime = 2;
        } else {
            m_nextEnemyTime = 1;
        }

        for (int i = 0; i < cnt; ++i) {
            float x;
            if (cnt == 1) {
                x = float(posDist(m_gen)) * 0.5f;
            } else {
                x = 2.f / (cnt - 1) * (float(posDist(m_gen)) * 0.2f + 1.f) * i - 1.f;
            }
            x += float(posDist(m_gen)) * 0.1f;
            x = glm::clamp(x, -1.f, 1.f);
            float fastness = scene.elapsedTime * 0.01f;

            EnemyComponent enemy;
            enemy.type = type;
            enemy.timeSinceHit = 10;

            CollidableComponent collide;

            PosComponent pos;
            pos.pos = glm::vec2(x, scene.aspectRatio);

            VelComponent vel;

            HealthComponent health;

            switch (type) {
            case EnemyType::HOUSE:
                collide.radius = 0.15f;
                enemy.shrapnelCount = 20;
                health.maxHealth = 10;
                vel.vel.x = 0;
                vel.vel.y = -0.1f;
                break;
            case EnemyType::CAR:
                collide.radius = 0.10f;
                enemy.shrapnelCount = 10;
                health.maxHealth = 5;
                vel.vel.x = 0.5f;
                vel.vel.y = -0.5f;
                break;
            case EnemyType::COCKTAIL:
                collide.radius = 0.08f;
                enemy.shrapnelCount = 5;
                health.maxHealth = 2;
                vel.vel.x = 0;
                vel.vel.y = -0.3f;
                break;
            case EnemyType::SQUID:
                collide.radius = 0.2f;
                enemy.shrapnelCount = 20;
                health.maxHealth = 30;
                vel.vel.x = 0;
                vel.vel.y = -0.1f;
                break;
            case EnemyType::BALLOON:
                collide.radius = 0.08f;
                enemy.shrapnelCount = 5;
                health.maxHealth = 1;
                vel.vel.x = 0.1f;
                vel.vel.y = -0.8f;
                break;
            }

            pos.pos.y += collide.radius;
            health.health = health.maxHealth;
            vel.vel.y -= fastness;

            AngleComponent angle;
            AngleSpeedComponent angleSpeed;

            std::normal_distribution<> normalDist;
            std::uniform_real_distribution<> angleDist(0, 360);
            angle.angle = float(angleDist(m_gen));
            angleSpeed.angleSpeed = float(normalDist(m_gen)) * 360;

            engine.addEntity(Entity({ enemy, collide, pos, vel, angle, angleSpeed, health }));
        }
    }

    engine.removeEntities<EnemyComponent>([&](Entity& ent) {
        PosComponent pos = ent.get<PosComponent>();
        if (pos.pos.y < -scene.aspectRatio - 0.2f) {
            player.get<HealthComponent>().health -= 5;
            // TODO: do damage animation
            return true;
        }
        return false;
    });

    for (Entity& ent : engine.iterate<EnemyComponent>()) {
        if (ent.has<CollisionComponent>()) {
            ent.removeComponent<CollisionComponent>();
            ent.get<EnemyComponent>().timeSinceHit = 0;
            ent.get<HealthComponent>().health -= 1;
            ent.get<PosComponent>().pos.y += 0.02f;
        }

        ent.get<EnemyComponent>().timeSinceHit += deltaTime;
        glm::vec2 pos = ent.get<PosComponent>().pos;
        if (pos.x < -1 || pos.x > 1) {
            VelComponent& vel = ent.get<VelComponent>();
            vel.vel.x = -vel.vel.x;
        }
    }

    std::vector<Entity> shrapnels;
    engine.removeEntities<EnemyComponent>([&](Entity& ent) {
        bool isDead = ent.get<HealthComponent>().health <= 0;
        if (isDead) {
            std::normal_distribution<float> normalDist;
            std::uniform_real_distribution<float> angleDist(0, 360);

            for (int i = 0; i < ent.get<EnemyComponent>().shrapnelCount; ++i) {
                ShrapnelComponent shrapnel;
                shrapnel.type = ent.get<EnemyComponent>().type;
                shrapnel.scale = normalDist(m_gen) * 0.02f + 0.05f;
                shrapnel.elapsedTime = 0;

                AngleComponent angle;
                angle.angle = angleDist(m_gen);

                AngleSpeedComponent angleSpeed;
                angleSpeed.angleSpeed = normalDist(m_gen) * 360;

                PosComponent pos;
                pos.pos = ent.get<PosComponent>().pos;

                VelComponent vel;
                float velAngle = angleDist(m_gen);
                vel.vel = ent.get<VelComponent>().vel;
                vel.vel += glm::vec2(glm::cos(velAngle), glm::sin(velAngle)) * float(3 + normalDist(m_gen));

                shrapnels.push_back(Entity({ shrapnel, pos, vel, angle, angleSpeed }));
            }
        }
        return isDead;
    });

    for (Entity& shrapnel : shrapnels) {
        engine.addEntity(std::move(shrapnel));
    }
}
}