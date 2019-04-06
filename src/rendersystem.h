#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "entitysystem.h"
#include "pointdata.h"
#include "hpbar.h"

namespace ou {

class RenderSystem : public EntitySystem {
    PointData m_pointsBuf;
    HpBar m_hpBar;

public:
    RenderSystem();

    void update(ECSEngine &engine, float deltaTime) override;
};
}

#endif // RENDERSYSTEM_H
