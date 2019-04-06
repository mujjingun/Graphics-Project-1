#include "physicssystem.h"
#include "components.h"
#include "ecsengine.h"

namespace ou {

PhysicsSystem::PhysicsSystem()
{
}

void PhysicsSystem::update(ECSEngine& engine, float deltaTime)
{
    for (Entity& ent : engine.iterate<PosComponent, VelComponent>()) {
        ent.get<PosComponent>().pos += deltaTime * ent.get<VelComponent>().vel;
    }

    for (Entity& ent : engine.iterate<AngleComponent, AngleSpeedComponent>()) {
        ent.get<AngleComponent>().angle += deltaTime * ent.get<AngleSpeedComponent>().angleSpeed;
    }
}
}
