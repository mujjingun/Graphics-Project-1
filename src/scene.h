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

class Airplane;
class Enemy;

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
    bool isKeyPressed(unsigned char key) const;

    double deltaTime() const;

    glm::dvec2 mouseDelta() const;
    glm::mat4 viewProjMat() const;

    float aspectRatio() const;

    friend class Callbacks;
    std::vector<std::unique_ptr<Enemy>>& enemies();
    std::unique_ptr<Airplane>& airplane();
};
}

#endif // SCENE_H
