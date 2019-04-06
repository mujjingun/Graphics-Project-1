#ifndef PLAYERSYSTEM_H
#define PLAYERSYSTEM_H

#include "entitysystem.h"

namespace ou {

class PlayerSystem : public EntitySystem {
public:
    PlayerSystem();

    void update(ECSEngine &engine, float deltaTime) override;
};
}

#endif // PLAYERSYSTEM_H
