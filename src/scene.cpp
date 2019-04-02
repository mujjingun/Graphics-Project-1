#include "scene.h"

#include "airplane.h"
#include "framebuffer.h"

#include <GL/glew.h>

#include <GL/freeglut.h>

#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace ou {

int Scene::windowWidth() const
{
    return m_windowWidth;
}

int Scene::windowHeight() const
{
    return m_windowHeight;
}

bool Scene::isKeyPressed(unsigned char key) const
{
    auto it = m_keyStates.find(key);
    if (it != m_keyStates.end()) {
        return it->second;
    }
    return false;
}

double Scene::deltaTime() const
{
    return std::chrono::duration<double>(m_deltaTime).count();
}

glm::dvec2 Scene::mouseDelta() const
{
    return m_smoothedMouseDelta;
}

void Scene::mouseClick()
{
    if (!m_captureMouse) {
        m_captureMouse = true;
        glutSetCursor(GLUT_CURSOR_NONE);
        m_lastMousePos = { 0, 0 };
        m_mousePos = { 0, 0 };
        m_warpPointer = true;
    } else {
        m_captureMouse = false;
        glutSetCursor(GLUT_CURSOR_INHERIT);
        m_mousePosInvalidated = true;
    }
}

void Scene::mouseMove(int x, int y)
{
    if (m_captureMouse) {
        glm::ivec2 delta(x - m_realMousePos.x, y - m_realMousePos.y);
        glm::ivec2 center(x - m_windowWidth / 2, y - m_windowHeight / 2);
        if (std::abs(center.x) > m_windowWidth / 3 || std::abs(center.y) > m_windowHeight / 3) {
            m_warpPointer = true;
        }
        m_mousePos += delta;
    } else {
        m_mousePos = { x, y };
    }

    m_realMousePos = { x, y };
}

void Scene::mouseEnter()
{
    m_mousePosInvalidated = true;
}

void Scene::keyDown(unsigned char key)
{
    m_keyStates[key] = true;
}

void Scene::keyUp(unsigned char key)
{
    m_keyStates[key] = false;
}

void Scene::reshapeWindow(int width, int height)
{
    if (width == m_windowWidth && height == m_windowHeight) {
        return;
    }

    m_windowWidth = width;
    m_windowHeight = height;

    // Resize viewport
    glViewport(0, 0, width, height);

    // Build framebuffer
    m_hdrFrameBuffer = FrameBuffer();

    m_hdrColorTexture = Texture(GL_TEXTURE_2D);
    m_hdrColorTexture.setMinFilter(GL_NEAREST);
    m_hdrColorTexture.setMagFilter(GL_NEAREST);
    m_hdrColorTexture.allocateStorage2D(1, GL_RGBA16F, width, height);
    m_hdrFrameBuffer.bindTexture(GL_COLOR_ATTACHMENT0, m_hdrColorTexture);

    m_hdrDepthRenderBuffer = RenderBuffer();
    m_hdrDepthRenderBuffer.allocateStorage(GL_DEPTH24_STENCIL8, width, height);
    m_hdrFrameBuffer.bindRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, m_hdrDepthRenderBuffer);

    if (!m_hdrFrameBuffer.isComplete()) {
        std::cerr << "Error building framebuffer\n";
        throw std::runtime_error("Framebuffer is not complete");
    }
}

Scene::Scene()
    : m_hdrShader("shaders/hdr.vert.glsl", "shaders/hdr.frag.glsl")
    , m_airplane(new Airplane(this))
    , m_lastFrameTime(std::chrono::system_clock::now())
{
    //glEnable(GL_CULL_FACE);
}

Scene::~Scene() = default;

void Scene::render()
{
    // Update time stuff
    auto now = std::chrono::system_clock::now();
    m_deltaTime = now - m_lastFrameTime;
    m_lastFrameTime = now;

    // Clear screen & framebuffer
    float clearColor[] = { 0, 0, 0, 0 };
    FrameBuffer::defaultBuffer().clear(GL_COLOR, 0, clearColor);
    m_hdrFrameBuffer.clear(GL_COLOR, 0, clearColor);

    float clearDepth[] = { 1 };
    FrameBuffer::defaultBuffer().clear(GL_DEPTH, 0, clearDepth);
    m_hdrFrameBuffer.clear(GL_DEPTH, 0, clearDepth);

    // Render stuff to framebuffer
    m_hdrFrameBuffer.use(GL_FRAMEBUFFER);
    //glEnable(GL_DEPTH_TEST);

    // render stuff
    m_airplane->render();

    // apply HDR
    FrameBuffer::defaultBuffer().use(GL_FRAMEBUFFER);
    //glDisable(GL_DEPTH_TEST);

    m_hdrVao.use();
    m_hdrShader.use();
    m_hdrColorTexture.use(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Update mouse cursor stuff
    if (m_mousePosInvalidated) {
        m_lastMousePos = m_mousePos;
        m_smoothedMouseDelta = { 0, 0 };
        m_mousePosInvalidated = false;
    } else {
        glm::dvec2 mouseDelta = m_mousePos - m_lastMousePos;

        double smoothing = 1 - glm::exp(-deltaTime() * 15);
        m_smoothedMouseDelta = mouseDelta * smoothing;
        m_lastMousePos += m_smoothedMouseDelta;
    }

    if (m_captureMouse && m_warpPointer) {
        glutWarpPointer(m_windowWidth / 2, m_windowHeight / 2);
        m_realMousePos = { m_windowWidth / 2, m_windowHeight / 2 };
        m_warpPointer = false;
    }
}
}
