#ifndef HOUSE_H
#define HOUSE_H

#include "enemy.h"
#include <memory>

namespace ou {

class Scene;
struct HouseStates;

class House : public Enemy {

    Scene* m_scene;

    std::unique_ptr<HouseStates> m_s;

public:
    House(Scene* scene);
    ~House() override;
    House(House&&);
    House& operator=(House&&);

    void render() override;
};
}

#endif // HOUSE_H
