#include "physicssystem.h"
#include "components.h"
#include "ecs/ecsengine.h"

namespace ou {

PhysicsSystem::PhysicsSystem()
{
}

void PhysicsSystem::update(ECSEngine& engine, float deltaTime)
{
	Entity const& player = engine.getOneEnt<PlayerComponent>();
	glm::vec2 playerPos = player.get<PosComponent>().pos;
	for (Entity& ent : engine.iterate<PosComponent, VelComponent, GravityComponent>()) {
		glm::vec2 diff = ent.get<PosComponent>().pos - playerPos;
		float r = glm::length(diff);
		ent.get<VelComponent>().vel -= diff / (r * r) * deltaTime * 5.0f;
	}

    for (Entity& ent : engine.iterate<PosComponent, VelComponent>()) {
        ent.get<PosComponent>().pos += deltaTime * ent.get<VelComponent>().vel;
    }

    for (Entity& ent : engine.iterate<PosComponent, ApparentVelComponent>()) {
        ApparentVelComponent& vel = ent.get<ApparentVelComponent>();
        vel.vel = (ent.get<PosComponent>().pos - vel.lastPos) / deltaTime;
        vel.lastPos = ent.get<PosComponent>().pos;
    }

    for (Entity& ent : engine.iterate<AngleComponent, AngleSpeedComponent>()) {
        ent.get<AngleComponent>().angle += deltaTime * ent.get<AngleSpeedComponent>().angleSpeed;
    }

    for (Entity& ent1 : engine.iterate<CollidableComponent>()) {
        glm::vec2& pos1 = ent1.get<PosComponent>().pos;
        float r1 = ent1.get<CollidableComponent>().radius;

        for (Entity& ent2 : engine.iterate<CollidableComponent>()) {
            if (&ent1 == &ent2) {
                continue;
            }

            glm::vec2& pos2 = ent2.get<PosComponent>().pos;
            float r2 = ent2.get<CollidableComponent>().radius;

            float dist = glm::distance(pos1, pos2);
            if (dist < r1 + r2) {
                glm::vec2 repel = glm::normalize(pos2 - pos1) * (dist - (r1 + r2)) * .5f;
                pos1 += repel;
                pos2 -= repel;
            }
        }
    }
}
}
