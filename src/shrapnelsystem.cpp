#include "shrapnelsystem.h"
#include "components.h"
#include "ecsengine.h"
#include "entity.h"

namespace ou {

ShrapnelSystem::ShrapnelSystem()
{
}

void ShrapnelSystem::update(ECSEngine& engine, float deltaTime)
{
    for (Entity& ent : engine.iterate<ShrapnelComponent>()) {
        ent.get<ShrapnelComponent>().elapsedTime += deltaTime;
    }

    engine.removeEntities<ShrapnelComponent>([](Entity& ent) {
        return ent.get<ShrapnelComponent>().elapsedTime > 2;
    });
}
}
