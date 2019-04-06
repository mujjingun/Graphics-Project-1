#ifndef ENEMYSYSTEM_H
#define ENEMYSYSTEM_H

#include "entitysystem.h"
#include <random>

namespace ou {

class EnemySystem : public EntitySystem {
    std::mt19937 m_gen{ std::random_device{}() };
    float m_elapsed = 0;
    float m_nextEnemyTime = 2;

public:
    EnemySystem();

    void update(ECSEngine& engine, float deltaTime) override;
};
}

#endif // ENEMYSYSTEM_H
