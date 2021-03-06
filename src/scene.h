#ifndef SCENE_H
#define SCENE_H

#include <chrono>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "graphics/framebuffer.h"
#include "graphics/renderbuffer.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/vertexarray.h"

namespace ou {

struct SceneStates;

class Scene {
    std::unique_ptr<SceneStates> m_s;

    void mouseClick();
    void mouseMove(int x, int y);
    void mouseEnter();

    void keyDown(unsigned char key);
    void keyUp(unsigned char key);

    void reshapeWindow(int width, int height);

public:
    Scene();
    ~Scene();
    Scene(Scene&&);
    Scene& operator=(Scene&&);

    void render();

    glm::dvec2 mouseDelta() const;

    friend class Callbacks;
};
}

#endif // SCENE_H
