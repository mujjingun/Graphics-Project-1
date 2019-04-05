#include "scene.h"

#include "airplane.h"
#include "backgroundsystem.h"
#include "components.h"
#include "ecsengine.h"
#include "enemy.h"
#include "enemysystem.h"
#include "framebuffer.h"
#include "hpbar.h"
#include "physicssystem.h"
#include "rendersystem.h"

#include <GL/glew.h>

#include <GL/freeglut.h>

#include <algorithm>
#include <iostream>
#include <random>

#include <glm/gtc/matrix_transform.hpp>

namespace ou {

struct SceneStates {
    Shader hdrShader{ "shaders/hdr.vert.glsl", "shaders/hdr.frag.glsl" };

    FrameBuffer hdrFrameBuffer;
    Texture hdrColorTexture;
    RenderBuffer hdrDepthRenderBuffer;
    VertexArray hdrVao;

    std::unique_ptr<Airplane> airplane;
    std::vector<std::unique_ptr<Enemy>> enemies;

    std::chrono::system_clock::time_point lastFrameTime;
    std::chrono::system_clock::duration deltaTime;

    glm::dvec2 mousePos;
    glm::dvec2 realMousePos;

    glm::dvec2 lastMousePos;
    glm::dvec2 smoothedMouseDelta;
    bool mousePosInvalidated = true;
    bool captureMouse = false;
    bool warpPointer = false;

    std::unordered_map<unsigned char, bool> keyStates;

    int windowWidth = -1, windowHeight = -1;

    std::mt19937 gen{ std::random_device{}() };
    float time = 0;
    float nextEnemyTime = 0;
    float damageTime = -10;

    HpBar hpbar;

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

bool Scene::isKeyPressed(unsigned char key) const
{
    auto it = m_s->keyStates.find(key);
    if (it != m_s->keyStates.end()) {
        return it->second;
    }
    return false;
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
    glm::mat4 projMat;
    float realAspectRatio = windowWidth() / float(windowHeight());
    if (windowWidth() * aspectRatio() > windowHeight()) {
        projMat = glm::ortho(-aspectRatio() * realAspectRatio,
            aspectRatio() * realAspectRatio,
            -aspectRatio(), aspectRatio(), -1000.0f, 1000.0f);
    } else {
        projMat = glm::ortho(-1.0f, 1.0f, -1 / realAspectRatio, 1 / realAspectRatio, -1000.0f, 1000.0f);
    }

    glm::mat4 viewMat = glm::mat4(1.0f);

    if (m_s->damageTime + 0.3f > m_s->time) {
        float mag = m_s->damageTime + 0.3f - m_s->time;
        float t = m_s->time - m_s->damageTime;
        float y = mag * glm::sin(t * 80) * 0.1f;
        viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(y, y, 0));
    }

    return projMat * viewMat;
}

float Scene::aspectRatio() const
{
    return 1.6f;
}

std::vector<std::unique_ptr<Enemy>>& Scene::enemies()
{
    return m_s->enemies;
}

std::unique_ptr<Airplane>& Scene::airplane()
{
    return m_s->airplane;
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
    m_s->keyStates[key] = true;
}

void Scene::keyUp(unsigned char key)
{
    m_s->keyStates[key] = false;
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

Scene::Scene()
    : m_s(new SceneStates{})
{
    m_s->engine.addSystem(std::make_unique<BackgroundSystem>());
    m_s->engine.addSystem(std::make_unique<RenderSystem>());
    m_s->engine.addSystem(std::make_unique<PhysicsSystem>());
    m_s->engine.addSystem(std::make_unique<EnemySystem>());

    glm::mat4 projMat;
    float realAspectRatio = windowWidth() / float(windowHeight());
    if (windowWidth() * aspectRatio() > windowHeight()) {
        projMat = glm::ortho(-aspectRatio() * realAspectRatio,
            aspectRatio() * realAspectRatio,
            -aspectRatio(), aspectRatio(), -1000.0f, 1000.0f);
    } else {
        projMat = glm::ortho(-1.0f, 1.0f, -1 / realAspectRatio, 1 / realAspectRatio, -1000.0f, 1000.0f);
    }

    SceneComponent scene;
    scene.aspectRatio = 1.6f;
    scene.viewProjMat = projMat;
    m_s->engine.addEntity(Entity({ scene }));

    PlayerComponent player;
    PosComponent playerPos{ { 0, -1 } };
    m_s->engine.addEntity(Entity({ player, playerPos }));

    InputComponent input;
    m_s->engine.addEntity(Entity({ input }));

    // TODO: get rid of this
    m_s->airplane.reset(new Airplane(this));

    m_s->lastFrameTime = std::chrono::system_clock::now();

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

Scene::Scene(Scene&&) = default;
Scene& Scene::operator=(Scene&&) = default;
Scene::~Scene() = default;

void Scene::realRender()
{
    m_s->engine.update(float(deltaTime()));

    m_s->time += deltaTime();

    for (auto& enemy : m_s->enemies) {
        if (enemy->isSuccess()) {
            m_s->airplane->takeDamage();
            if (!m_s->airplane->isGameOver()) {
                m_s->damageTime = m_s->time;
            }
        }
    }

    m_s->enemies.erase(
        std::remove_if(m_s->enemies.begin(), m_s->enemies.end(),
            [&](std::unique_ptr<Enemy>& enemy) {
                return enemy->doRemove();
            }),
        m_s->enemies.end());

    std::uniform_real_distribution<> posDist(-1, 1);
    std::discrete_distribution<> typeDist({ 2, 1, 2, 1, 2 });
    std::discrete_distribution<> cntDist({ 1, 3, 4, 3, 2, 1 });
    Enemy::Type types[] = {
        Enemy::Type::CAR,
        Enemy::Type::HOUSE,
        Enemy::Type::COCKTAIL,
        Enemy::Type::SQUID,
        Enemy::Type::BALOON,
    };
    if (m_s->time > m_s->nextEnemyTime) {

        // make a new row of enemies
        int cnt = cntDist(m_s->gen);
        Enemy::Type type = types[typeDist(m_s->gen)];

        if (type == Enemy::Type::SQUID) {
            cnt = 1;
            m_s->nextEnemyTime += 2;
        }

        for (int i = 0; i < cnt; ++i) {
            float x;
            if (cnt == 1) {
                x = float(posDist(m_s->gen)) * 0.5f;
            } else {
                x = 2.f / (cnt - 1) * (float(posDist(m_s->gen)) * 0.2f + 1.f) * i - 1.f;
            }
            x += float(posDist(m_s->gen)) * 0.1f;
            x = glm::clamp(x, -1.f, 1.f);
            float fastness = m_s->time * 0.01f;
            m_s->enemies.push_back(std::make_unique<Enemy>(this, type, x, fastness));
        }

        m_s->nextEnemyTime += 2;
    }

    //m_s->background->render();
    for (auto& enemy : m_s->enemies) {
        enemy->render();
    }
    m_s->airplane->render();

    // render health bar
    glm::mat4 hpBarProjMat = glm::ortho(-1.0f, 1.0f, -aspectRatio(), aspectRatio(), -1000.0f, 1000.0f);

    float hpBarScale = 0.02f;
    if (m_s->damageTime + 0.3f > m_s->time) {
        hpBarScale += 0.3f * (m_s->damageTime + 0.3f - m_s->time);
    }
    glm::mat4 hpBarModelMat = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -aspectRatio(), 0.0f))
        * glm::scale(glm::mat4(1.0), glm::vec3(1.f, hpBarScale, 0.0f));
    m_s->hpbar.render(hpBarProjMat * hpBarModelMat, m_s->airplane->healthRatio());

    if (m_s->airplane->isGameOver()) {
        m_s->time = 0;
        m_s->damageTime = -10;
        m_s->nextEnemyTime = 0;
        m_s->airplane.reset(new Airplane(this));
        m_s->enemies.clear();
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
}
