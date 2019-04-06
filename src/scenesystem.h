#ifndef SCENESYSTEM_H
#define SCENESYSTEM_H

#include "entitysystem.h"

namespace ou {

class SceneSystem : public EntitySystem {
public:
    SceneSystem();

    void update(ECSEngine &engine, float deltaTime) override;
};
}

#endif // SCENESYSTEM_H
