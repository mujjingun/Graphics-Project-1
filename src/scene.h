#ifndef SCENE_H
#define SCENE_H

#include <chrono>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "framebuffer.h"
#include "renderbuffer.h"
#include "shader.h"
#include "texture.h"
#include "vertexarray.h"

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

    void realRender();

public:
    Scene();
    ~Scene();
    Scene(Scene&&);
    Scene& operator=(Scene&&);

    void render();

    int windowWidth() const;
    int windowHeight() const;

    double deltaTime() const;

    glm::dvec2 mouseDelta() const;
    glm::mat4 viewProjMat() const;

    friend class Callbacks;
};
}

#endif // SCENE_H
