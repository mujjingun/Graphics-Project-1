#ifndef AIRPLANE_H
#define AIRPLANE_H

#include "vertexarray.h"
#include "vertexbuffer.h"

#include <glm/glm.hpp>
#include <memory>

namespace ou {

class Scene;
struct AirplaneStates;

class Airplane {
    Scene* m_scene;

    std::unique_ptr<AirplaneStates> m_s;

public:
    Airplane(Scene* scene);
    Airplane(Airplane&&);
    Airplane& operator=(Airplane&&);
    ~Airplane();

    void render();

    void takeDamage();

    bool isGameOver();

    glm::vec2 pos() const;
};
}

#endif // AIRPLANE_H
