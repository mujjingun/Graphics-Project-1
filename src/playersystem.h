#ifndef PLAYERSYSTEM_H
#define PLAYERSYSTEM_H

#include "entitysystem.h"
#include <random>

namespace ou {

class PlayerSystem : public EntitySystem {
    std::mt19937 m_gen{ std::random_device{}() };

public:
    PlayerSystem();

    void update(ECSEngine &engine, float deltaTime) override;
};
}

#endif // PLAYERSYSTEM_H
