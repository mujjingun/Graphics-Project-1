#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "ecs/entitysystem.h"

namespace ou {

class PhysicsSystem : public EntitySystem {
public:
    PhysicsSystem();

    void update(ECSEngine &engine, float deltaTime) override;
};
}

#endif // PHYSICSSYSTEM_H
