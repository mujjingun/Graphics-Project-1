#ifndef AIRPLANE_H
#define AIRPLANE_H

#include "vertexarray.h"
#include "vertexbuffer.h"

#include <memory>

namespace ou {

class Scene;
struct AirplaneStates;

class Airplane {
    Scene* m_scene;

    std::unique_ptr<AirplaneStates> m_s;

    VertexArray m_vao;
    VertexBuffer m_vbo;

public:
    Airplane(Scene* scene);
    Airplane(Airplane &&);
    Airplane& operator=(Airplane &&);
    ~Airplane();

    void render();
};
}

#endif // AIRPLANE_H
