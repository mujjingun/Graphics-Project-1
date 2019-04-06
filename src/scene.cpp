#include "scene.h"

#include "components.h"
#include "ecsengine.h"
#include "framebuffer.h"

// entity systems
#include "backgroundsystem.h"
#include "enemysystem.h"
#include "physicssystem.h"
#include "playersystem.h"
#include "projectilesystem.h"
#include "rendersystem.h"
#include "scenesystem.h"
#include "shrapnelsystem.h"

#include <GL/glew.h>

#include <GL/freeglut.h>

#include <algorithm>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

namespace ou {

struct SceneStates {
    Shader hdrShader{ "shaders/hdr.vert.glsl", "shaders/hdr.frag.glsl" };

    FrameBuffer hdrFrameBuffer;
    Texture hdrColorTexture;
    RenderBuffer hdrDepthRenderBuffer;
    VertexArray hdrVao;

    std::chrono::system_clock::time_point lastFrameTime;
    std::chrono::system_clock::duration deltaTime;

    glm::dvec2 mousePos;
    glm::dvec2 realMousePos;

    glm::dvec2 lastMousePos;
    glm::dvec2 smoothedMouseDelta;
    bool mousePosInvalidated = true;
    bool captureMouse = false;
    bool warpPointer = false;

    int windowWidth = -1, windowHeight = -1;

    ECSEngine engine{};
};

int Scene::windowWidth() const
{
    return m_s->windowWidth;
}

int Scene::windowHeight() const
{
    return m_s->windowHeight;
}

double Scene::deltaTime() const
{
    return std::chrono::duration<double>(m_s->deltaTime).count();
}

glm::dvec2 Scene::mouseDelta() const
{
    return m_s->smoothedMouseDelta;
}

glm::mat4 Scene::viewProjMat() const
{
    SceneComponent scene = m_s->engine.getOne<SceneComponent>();

    glm::mat4 projMat;
    float realAspectRatio = windowWidth() / float(windowHeight());
    if (windowWidth() * scene.aspectRatio > windowHeight()) {
        projMat = glm::ortho(-scene.aspectRatio * realAspectRatio,
            scene.aspectRatio * realAspectRatio,
            -scene.aspectRatio, scene.aspectRatio, -1000.0f, 1000.0f);
    } else {
        projMat = glm::ortho(-1.0f, 1.0f, -1 / realAspectRatio, 1 / realAspectRatio, -1000.0f, 1000.0f);
    }

    glm::mat4 viewMat = glm::mat4(1.0f);

    return projMat * viewMat;
}

void Scene::mouseClick()
{
    if (!m_s->captureMouse) {
        m_s->captureMouse = true;
        glutSetCursor(GLUT_CURSOR_NONE);
        m_s->lastMousePos = { 0, 0 };
        m_s->mousePos = { 0, 0 };
        m_s->warpPointer = true;
    } else {
        m_s->captureMouse = false;
        glutSetCursor(GLUT_CURSOR_INHERIT);
        m_s->mousePosInvalidated = true;
    }
}

void Scene::mouseMove(int x, int y)
{
    if (m_s->captureMouse) {
        glm::ivec2 delta(x - m_s->realMousePos.x, y - m_s->realMousePos.y);
        glm::ivec2 center(x - m_s->windowWidth / 2, y - m_s->windowHeight / 2);
        if (std::abs(center.x) > m_s->windowWidth / 3 || std::abs(center.y) > m_s->windowHeight / 3) {
            m_s->warpPointer = true;
        }
        m_s->mousePos += delta;
    } else {
        m_s->mousePos = { x, y };
    }

    m_s->realMousePos = { x, y };
}

void Scene::mouseEnter()
{
    m_s->mousePosInvalidated = true;
}

void Scene::keyDown(unsigned char key)
{
    m_s->engine.getOne<InputComponent>().keyStates[key] = true;
}

void Scene::keyUp(unsigned char key)
{
    m_s->engine.getOne<InputComponent>().keyStates[key] = false;
}

void Scene::reshapeWindow(int width, int height)
{
    if (width == m_s->windowWidth && height == m_s->windowHeight) {
        return;
    }

    m_s->windowWidth = width;
    m_s->windowHeight = height;

    // Resize viewport
    glViewport(0, 0, width, height);

    // adjust projection matrix
    m_s->engine.getOne<SceneComponent>().viewProjMat = viewProjMat();

    // Build framebuffer
    m_s->hdrFrameBuffer = FrameBuffer();

    m_s->hdrColorTexture = Texture(GL_TEXTURE_2D);
    m_s->hdrColorTexture.setMinFilter(GL_NEAREST);
    m_s->hdrColorTexture.setMagFilter(GL_NEAREST);
    m_s->hdrColorTexture.allocateStorage2D(1, GL_RGBA16F, width, height);
    m_s->hdrFrameBuffer.bindTexture(GL_COLOR_ATTACHMENT0, m_s->hdrColorTexture);

    m_s->hdrDepthRenderBuffer = RenderBuffer();
    m_s->hdrDepthRenderBuffer.allocateStorage(GL_DEPTH24_STENCIL8, width, height);
    m_s->hdrFrameBuffer.bindRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, m_s->hdrDepthRenderBuffer);

    if (!m_s->hdrFrameBuffer.isComplete()) {
        std::cerr << "Error building framebuffer\n";
        throw std::runtime_error("Framebuffer is not complete");
    }
}

void Scene::render()
{
    // Update time stuff
    auto now = std::chrono::system_clock::now();
    m_s->deltaTime = now - m_s->lastFrameTime;
    m_s->lastFrameTime = now;

    // Clear screen & framebuffer
    float clearColor[] = { 0, 0, 0, 0 };
    FrameBuffer::defaultBuffer().clear(GL_COLOR, 0, clearColor);
    m_s->hdrFrameBuffer.clear(GL_COLOR, 0, clearColor);

    float clearDepth[] = { 1 };
    FrameBuffer::defaultBuffer().clear(GL_DEPTH, 0, clearDepth);
    m_s->hdrFrameBuffer.clear(GL_DEPTH, 0, clearDepth);

    // Render stuff to framebuffer
    m_s->hdrFrameBuffer.use(GL_FRAMEBUFFER);
    //glEnable(GL_DEPTH_TEST);

    // render stuff
    realRender();

    // apply HDR
    FrameBuffer::defaultBuffer().use(GL_FRAMEBUFFER);
    //glDisable(GL_DEPTH_TEST);

    m_s->hdrVao.use();
    m_s->hdrShader.use();
    m_s->hdrColorTexture.use(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Update mouse cursor stuff
    if (m_s->mousePosInvalidated) {
        m_s->lastMousePos = m_s->mousePos;
        m_s->smoothedMouseDelta = { 0, 0 };
        m_s->mousePosInvalidated = false;
    } else {
        glm::dvec2 mouseDelta = m_s->mousePos - m_s->lastMousePos;

        double smoothing = 1 - glm::exp(-deltaTime() * 15);
        m_s->smoothedMouseDelta = mouseDelta * smoothing;
        m_s->lastMousePos += m_s->smoothedMouseDelta;
    }

    if (m_s->captureMouse && m_s->warpPointer) {
        glutWarpPointer(m_s->windowWidth / 2, m_s->windowHeight / 2);
        m_s->realMousePos = { m_s->windowWidth / 2, m_s->windowHeight / 2 };
        m_s->warpPointer = false;
    }
}

Scene::Scene()
    : m_s(new SceneStates{})
{
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    m_s->engine.addSystem(std::make_unique<SceneSystem>());
    m_s->engine.addSystem(std::make_unique<BackgroundSystem>());
    m_s->engine.addSystem(std::make_unique<RenderSystem>());
    m_s->engine.addSystem(std::make_unique<PhysicsSystem>());
    m_s->engine.addSystem(std::make_unique<EnemySystem>());
    m_s->engine.addSystem(std::make_unique<PlayerSystem>());
    m_s->engine.addSystem(std::make_unique<ProjectileSystem>());
    m_s->engine.addSystem(std::make_unique<ShrapnelSystem>());

    SceneComponent scene;
    scene.aspectRatio = 1.6f;
    scene.elapsedTime = 0;
    m_s->engine.addEntity(Entity({ scene }));

    PlayerComponent player;
    player.dest = { 0, -1 };
    player.timeSinceBullet = -1;

    PosComponent playerPos;
    playerPos.pos = { 0, -scene.aspectRatio };

    HealthComponent playerHealth;
    playerHealth.health = 100;
    playerHealth.maxHealth = 100;
    m_s->engine.addEntity(Entity({ player, playerPos, playerHealth }));

    InputComponent input;
    m_s->engine.addEntity(Entity({ input }));

    m_s->lastFrameTime = std::chrono::system_clock::now();
}

void Scene::realRender()
{
    m_s->engine.update(float(deltaTime()));
}

Scene::Scene(Scene&&) = default;
Scene& Scene::operator=(Scene&&) = default;
Scene::~Scene() = default;
}
