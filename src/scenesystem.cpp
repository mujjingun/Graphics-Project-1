#include "scenesystem.h"
#include "components.h"
#include "ecsengine.h"

namespace ou {

SceneSystem::SceneSystem()
{
}

void SceneSystem::update(ECSEngine &engine, float deltaTime)
{
    SceneComponent& scene = engine.getOne<SceneComponent>();
    scene.elapsedTime += deltaTime;
}
}
