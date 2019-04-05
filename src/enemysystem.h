#ifndef ENEMYSYSTEM_H
#define ENEMYSYSTEM_H

#include "entitysystem.h"

namespace ou {

class EnemySystem : public EntitySystem {
public:
    EnemySystem();

    void update(ECSEngine &engine, float deltaTime) override;
};
}

#endif // ENEMYSYSTEM_H
