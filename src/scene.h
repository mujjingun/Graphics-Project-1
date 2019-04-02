#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <unordered_map>
#include <chrono>
#include <vector>
#include <glm/glm.hpp>

#include "framebuffer.h"
#include "texture.h"
#include "renderbuffer.h"
#include "shader.h"
#include "vertexarray.h"

namespace ou {

class Background;
class Airplane;
class Enemy;

class Scene {
    FrameBuffer m_hdrFrameBuffer;
    Texture m_hdrColorTexture;
    RenderBuffer m_hdrDepthRenderBuffer;
    Shader m_hdrShader;
    VertexArray m_hdrVao;

    std::unique_ptr<Background> m_background;
    std::unique_ptr<Airplane> m_airplane;
    std::vector<std::unique_ptr<Enemy>> m_enemies;

    std::chrono::system_clock::time_point m_lastFrameTime;
    std::chrono::system_clock::duration m_deltaTime;

    glm::dvec2 m_mousePos;
    glm::dvec2 m_realMousePos;

    glm::dvec2 m_lastMousePos;
    glm::dvec2 m_smoothedMouseDelta;
    bool m_mousePosInvalidated = true;
    bool m_captureMouse = false;
    bool m_warpPointer = false;

    void mouseClick();
    void mouseMove(int x, int y);
    void mouseEnter();

    std::unordered_map<unsigned char, bool> m_keyStates;

    void keyDown(unsigned char key);
    void keyUp(unsigned char key);

    int m_windowWidth = -1, m_windowHeight = -1;

    void reshapeWindow(int width, int height);

    void realRender();

public:
    Scene();
    ~Scene();
    void render();

    int windowWidth() const;
    int windowHeight() const;
    bool isKeyPressed(unsigned char key) const;

    double deltaTime() const;

    glm::dvec2 mouseDelta() const;
    glm::mat4 viewProjMat() const;

    float aspectRatio() const;

    friend class Callbacks;
};
}

#endif // SCENE_H
