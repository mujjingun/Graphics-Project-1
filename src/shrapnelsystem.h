#ifndef SHRAPNELSYSTEM_H
#define SHRAPNELSYSTEM_H

#include "entitysystem.h"

namespace ou {

class ShrapnelSystem : public EntitySystem {
public:
    ShrapnelSystem();

    void update(ECSEngine &engine, float deltaTime) override;
};
}

#endif // SHRAPNELSYSTEM_H
