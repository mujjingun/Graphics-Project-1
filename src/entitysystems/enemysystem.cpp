#include "enemysystem.h"
#include "components.h"
#include "ecs/ecsengine.h"

#include <iostream>

namespace ou {

EnemySystem::EnemySystem()
{
}

void EnemySystem::update(ECSEngine& engine, float deltaTime)
{
    SceneComponent& scene = engine.getOne<SceneComponent>();
    Entity& player = engine.getOneEnt<PlayerComponent>();

    std::uniform_real_distribution<float> posDist(-1, 1);
    std::discrete_distribution<> typeDist({ 2, 1, 2, 1, 2 });
    std::discrete_distribution<> cntDist({ 0, 3, 4, 3, 2, 1 });
    EntityType types[] = {
        EntityType::CAR,
        EntityType::HOUSE,
        EntityType::COCKTAIL,
        EntityType::SQUID,
        EntityType::BALLOON,
    };

    m_elapsed += deltaTime;

    if (m_elapsed > m_nextEnemyTime) {
        m_elapsed -= m_nextEnemyTime;

        // make a new row of enemies
        int cnt = cntDist(engine.rand());
        EntityType type = types[typeDist(engine.rand())];

        m_nextEnemyTime = 1;
        if (type == EntityType::SQUID) {
            cnt = 1;
            m_nextEnemyTime = 2;
        } else if (type == EntityType::HOUSE) {
            cnt = cnt % 3;
        }

        for (int i = 0; i < cnt; ++i) {
            float x;
            if (cnt == 1) {
                x = posDist(engine.rand()) * 0.5f;
            } else {
                x = 2.f / (cnt - 1) * (posDist(engine.rand()) * 0.2f + 1.f) * i - 1.f;
            }
            x += posDist(engine.rand()) * 0.1f;
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
            case EntityType::HOUSE:
                collide.radius = 0.15f;
                enemy.shrapnelCount = 20;
                health.maxHealth = 10;
                vel.vel.x = 0;
                vel.vel.y = -0.1f;
                break;
            case EntityType::CAR:
                collide.radius = 0.10f;
                enemy.shrapnelCount = 10;
                health.maxHealth = 5;
                vel.vel.x = 0.5f * glm::sign(posDist(engine.rand()));
                vel.vel.y = -0.5f;
                break;
            case EntityType::COCKTAIL:
                collide.radius = 0.08f;
                enemy.shrapnelCount = 5;
                health.maxHealth = 2;
                vel.vel.x = 0;
                vel.vel.y = -0.3f;
                break;
            case EntityType::SQUID:
                collide.radius = 0.2f;
                enemy.shrapnelCount = 20;
                health.maxHealth = 30;
                vel.vel.x = 0.2f;
                vel.vel.y = -0.1f;
                break;
            case EntityType::BALLOON:
                collide.radius = 0.08f;
                enemy.shrapnelCount = 5;
                health.maxHealth = 1;
                vel.vel.x = 0.2f;
                vel.vel.y = -0.8f;
                break;
            default:
                break;
            }

            pos.pos.y += collide.radius;
            health.health = health.maxHealth;
            vel.vel.y -= fastness;

            AngleComponent angle;
            AngleSpeedComponent angleSpeed;

            std::normal_distribution<> normalDist;
            std::uniform_real_distribution<> angleDist(0, 360);
            angle.angle = float(angleDist(engine.rand()));
            angleSpeed.angleSpeed = float(normalDist(engine.rand())) * 360;

            engine.addEntity(Entity({ enemy, collide, pos, vel, angle, angleSpeed, health }));
        }
    }

    engine.removeEntities<EnemyComponent>([&](Entity& ent) {
        PosComponent pos = ent.get<PosComponent>();
        if (pos.pos.y < -scene.aspectRatio - 0.2f) {
            if (player.get<HealthComponent>().health > 0) {
				if (ent.get<EnemyComponent>().type == EntityType::BALLOON) {
					player.get<HealthComponent>().health += 2;
				}
				else {
					player.get<HealthComponent>().health -= 5;
					player.get<PlayerComponent>().timeSinceHit = 0;
				}
            }
            return true;
        }
        return false;
    });

    for (Entity& ent : engine.iterate<EnemyComponent>()) {
        if (ent.has<CollisionComponent>()) {
            ent.get<EnemyComponent>().timeSinceHit = 0;
			switch (ent.get<CollisionComponent>().type) {
			case ProjectileComponent::Type::Bullet:
				ent.get<HealthComponent>().health -= 1;
				break;
			case ProjectileComponent::Type::Missile:
				ent.get<HealthComponent>().health -= 5;
				break;
			}
            ent.get<PosComponent>().pos.y += 0.05f;
			ent.removeComponent<CollisionComponent>();
        }

        ent.get<EnemyComponent>().timeSinceHit += deltaTime;
        glm::vec2& pos = ent.get<PosComponent>().pos;
        if (pos.x < -1 || pos.x > 1) {
            glm::vec2& vel = ent.get<VelComponent>().vel;
            vel.x = -vel.x;
        }

		if (ent.get<EnemyComponent>().type == EntityType::BALLOON) {
			glm::vec2& vel = ent.get<VelComponent>().vel;
			if (pos.x < player.get<PosComponent>().pos.x) {
				vel.x = glm::abs(vel.x);
			}
			else {
				vel.x = -glm::abs(vel.x);
			}
		}

        if (ent.get<EnemyComponent>().type == EntityType::SQUID) {
            glm::vec2& vel = ent.get<VelComponent>().vel;
            if (pos.x < player.get<PosComponent>().pos.x) {
                vel.x = -glm::abs(vel.x);
            } else {
                vel.x = glm::abs(vel.x);
            }
            pos.x = glm::clamp(pos.x, -0.9f, 0.9f);
        }

        pos.x = glm::clamp(pos.x, -1.0f, 1.0f);
    }

    std::vector<Entity> shrapnels;
    engine.removeEntities<EnemyComponent>([&](Entity& ent) {
        bool isDead = ent.get<HealthComponent>().health <= 0;
        if (isDead) {

            // update scores
            switch (ent.get<EnemyComponent>().type) {
            case EntityType::HOUSE:
                scene.score += 5;
                break;
            case EntityType::CAR:
                scene.score += 3;
                break;
            case EntityType::COCKTAIL:
                scene.score += 2;
                break;
            case EntityType::SQUID:
                scene.score += 10;
                break;
            case EntityType::BALLOON:
                scene.score += 1;
                break;
            default:
                break;
            }

            // emit shrapnels
            std::normal_distribution<float> normalDist;
            std::uniform_real_distribution<float> angleDist(0, 360);

            for (int i = 0; i < ent.get<EnemyComponent>().shrapnelCount; ++i) {
                ShrapnelComponent shrapnel;
                shrapnel.type = ent.get<EnemyComponent>().type;
                shrapnel.scale = normalDist(engine.rand()) * 0.02f + 0.05f;
                shrapnel.elapsedTime = 0;

                AngleComponent angle;
                angle.angle = angleDist(engine.rand());

                AngleSpeedComponent angleSpeed;
                angleSpeed.angleSpeed = normalDist(engine.rand()) * 360;

                PosComponent pos;
                pos.pos = ent.get<PosComponent>().pos;

                VelComponent vel;
                float velAngle = angleDist(engine.rand());
                vel.vel = ent.get<VelComponent>().vel;
                vel.vel += glm::vec2(glm::cos(velAngle), glm::sin(velAngle)) * float(3 + normalDist(engine.rand()));

				Entity ent({ shrapnel, pos, vel, angle, angleSpeed });
				
				if (shrapnel.type == EntityType::BALLOON) {
					ent.addComponent(GravityComponent{});
				}

                shrapnels.push_back(std::move(ent));
            }
        }
        return isDead;
    });

    for (Entity& shrapnel : shrapnels) {
        engine.addEntity(std::move(shrapnel));
    }
}
}
