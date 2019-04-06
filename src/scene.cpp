#include "scene.h"

#include "components.h"
#include "ecs/ecsengine.h"

#include "graphics/framebuffer.h"

// entity systems
#include "entitysystems/backgroundsystem.h"
#include "entitysystems/enemysystem.h"
#include "entitysystems/physicssystem.h"
#include "entitysystems/playersystem.h"
#include "entitysystems/projectilesystem.h"
#include "entitysystems/rendersystem.h"
#include "entitysystems/scenesystem.h"
#include "entitysystems/shrapnelsystem.h"

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

    ECSEngine engine{};
};

glm::dvec2 Scene::mouseDelta() const
{
    return m_s->smoothedMouseDelta;
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
    SceneComponent scene = m_s->engine.getOne<SceneComponent>();

    if (m_s->captureMouse) {
        glm::ivec2 delta(x - m_s->realMousePos.x, y - m_s->realMousePos.y);
        glm::ivec2 center(x - scene.windowWidth / 2, y - scene.windowHeight / 2);
        if (std::abs(center.x) > scene.windowWidth / 3 || std::abs(center.y) > scene.windowHeight / 3) {
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
    SceneComponent& scene = m_s->engine.getOne<SceneComponent>();
    if (width == scene.windowWidth && height == scene.windowHeight) {
        return;
    }

    scene.windowWidth = width;
    scene.windowHeight = height;

    // Resize viewport
    glViewport(0, 0, width, height);

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

    float fDeltaTime = std::chrono::duration<float>(m_s->deltaTime).count();

    // Clear screen & framebuffer
    float clearColor[] = { 0, 0, 0, 0 };
    FrameBuffer::defaultBuffer().clear(GL_COLOR, 0, clearColor);
    m_s->hdrFrameBuffer.clear(GL_COLOR, 0, clearColor);

    float clearDepth[] = { 1 };
    FrameBuffer::defaultBuffer().clear(GL_DEPTH, 0, clearDepth);
    m_s->hdrFrameBuffer.clear(GL_DEPTH, 0, clearDepth);

    // Render stuff to framebuffer
    m_s->hdrFrameBuffer.use(GL_FRAMEBUFFER);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_DEPTH_TEST);

    // render and update stuff
    m_s->engine.update(fDeltaTime);

    // apply HDR
    FrameBuffer::defaultBuffer().use(GL_FRAMEBUFFER);
    glDisable(GL_BLEND);
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

        float smoothing = 1 - glm::exp(-fDeltaTime * 15);
        m_s->smoothedMouseDelta = mouseDelta * double(smoothing);
        m_s->lastMousePos += m_s->smoothedMouseDelta;
    }

    SceneComponent scene = m_s->engine.getOne<SceneComponent>();
    if (m_s->captureMouse && m_s->warpPointer) {
        glutWarpPointer(scene.windowWidth / 2, scene.windowHeight / 2);
        m_s->realMousePos = { scene.windowWidth / 2, scene.windowHeight / 2 };
        m_s->warpPointer = false;
    }
}

Scene::Scene()
    : m_s(new SceneStates{})
{
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);

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
    scene.windowWidth = -1;
    scene.windowHeight = -1;
    scene.score = 0;
    m_s->engine.addEntity(Entity({ scene }));

    PlayerComponent player;
    player.dest = { 0, -1 };
    player.timeSinceBullet = -1;
    player.timeSinceHit = 10;
    player.timeSinceStreak = 0;

    CollidableComponent playerCollision;
    playerCollision.radius = 0.1f;

    PosComponent playerPos;
    playerPos.pos = { 0, -scene.aspectRatio };

    HealthComponent playerHealth;
    playerHealth.health = 100;
    playerHealth.maxHealth = 100;
    m_s->engine.addEntity(Entity({ player, playerPos, playerHealth, playerCollision }));

    InputComponent input;
    m_s->engine.addEntity(Entity({ input }));

    m_s->lastFrameTime = std::chrono::system_clock::now();
}

Scene::Scene(Scene&&) = default;
Scene& Scene::operator=(Scene&&) = default;
Scene::~Scene() = default;
}
