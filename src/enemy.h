#ifndef HOUSE_H
#define HOUSE_H

#include <glm/glm.hpp>
#include <memory>

namespace ou {

class Scene;
struct HouseStates;

class Enemy {

    Scene* m_scene;

    std::unique_ptr<HouseStates> m_s;

public:
    enum class Type {
        HOUSE,
        CAR,
        COCKTAIL,
    };

    Enemy(Scene* scene, Type type, float x);
    ~Enemy();
    Enemy(Enemy&&);
    Enemy& operator=(Enemy&&);

    void render();
    bool collide(glm::vec2 a, glm::vec2 b);
    bool doRemove();
    bool isSuccess();
};
}

#endif // HOUSE_H
