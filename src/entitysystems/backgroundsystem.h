#ifndef BACKGROUNDSYSTEM_H
#define BACKGROUNDSYSTEM_H

#include "ecs/entitysystem.h"
#include <random>

namespace ou {

class BackgroundSystem : public EntitySystem {
    float m_elapsed = 0;
    float m_next = -20;

public:
    BackgroundSystem();

    void update(ECSEngine& engine, float deltaTime) override;
};
}

#endif // BACKGROUNDSYSTEM_H
