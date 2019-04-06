#ifndef PROJECTILESYSTEM_H
#define PROJECTILESYSTEM_H

#include "entitysystem.h"

namespace ou {

class ProjectileSystem : public EntitySystem {
public:
    ProjectileSystem();

    void update(ECSEngine &engine, float deltaTime) override;
};
}

#endif // PROJECTILESYSTEM_H
