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
#include <glm/gtc/type_ptr.hpp>

namespace ou {

struct SceneStates {
    Shader hdrShader{ "shaders/hdr.vert.glsl", "shaders/hdr.frag.glsl" };

    FrameBuffer hdrFrameBuffer, hdrResolveFrameBuffer;
    Texture hdrColorTexture, hdrResolveTexture;
    VertexArray hdrVao;

    std::chrono::system_clock::time_point lastFrameTime;
    std::chrono::system_clock::duration deltaTime;

    ECSEngine engine{};
};

glm::dvec2 Scene::mouseDelta() const
{
	return {};
}

void Scene::mouseClick()
{
}

void Scene::mouseMove(int, int)
{
}

void Scene::mouseEnter()
{
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

    m_s->hdrColorTexture = Texture(GL_TEXTURE_2D_MULTISAMPLE);
    m_s->hdrColorTexture.allocateMultisample2D(4, GL_RGBA16F, width, height, false);
    m_s->hdrFrameBuffer.bindTexture(GL_COLOR_ATTACHMENT0, m_s->hdrColorTexture);

    if (!m_s->hdrFrameBuffer.isComplete()) {
        std::cerr << "Error building framebuffer\n";
        throw std::runtime_error("Framebuffer is not complete");
    }

	m_s->hdrResolveTexture = Texture(GL_TEXTURE_2D);
	m_s->hdrResolveTexture.allocateStorage2D(1, GL_RGBA16F, width, height);
	m_s->hdrResolveFrameBuffer.bindTexture(GL_COLOR_ATTACHMENT0, m_s->hdrResolveTexture);

	if (!m_s->hdrResolveFrameBuffer.isComplete()) {
		std::cerr << "Error building hdr resolve framebuffer\n";
		throw std::runtime_error("Framebuffer is not complete");
	}
}

void Scene::render()
{
	SceneComponent& scene = m_s->engine.getOne<SceneComponent>();

    // Update time stuff
    auto now = std::chrono::system_clock::now();
    m_s->deltaTime = now - m_s->lastFrameTime;
    m_s->lastFrameTime = now;

    float fDeltaTime = std::chrono::duration<float>(m_s->deltaTime).count();

    // Clear screen & framebuffer
    glm::vec4 clearColor = glm::mix(glm::vec4(0, 0, 0.002f, 0), glm::vec4(0.002f, 0, 0.002f, 0), scene.elapsedTime / 100);
    FrameBuffer::defaultBuffer().clear(GL_COLOR, 0, glm::value_ptr(clearColor));
    m_s->hdrFrameBuffer.clear(GL_COLOR, 0, glm::value_ptr(clearColor));

    // Render stuff to framebuffer
    m_s->hdrFrameBuffer.use(GL_FRAMEBUFFER);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
    // render and update stuff
    m_s->engine.update(fDeltaTime);

    // apply HDR
	glBlitNamedFramebuffer(m_s->hdrFrameBuffer.id(), m_s->hdrResolveFrameBuffer.id(),
		0, 0, scene.windowWidth, scene.windowHeight,
		0, 0, scene.windowWidth, scene.windowHeight,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);

    FrameBuffer::defaultBuffer().use(GL_FRAMEBUFFER);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
	
    m_s->hdrVao.use();
    m_s->hdrShader.use();
    m_s->hdrResolveTexture.useAsTexture(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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
    playerPos.pos = { 0, -1 };

    ApparentVelComponent playerAppVel;
    playerAppVel.lastPos = playerPos.pos;
    playerAppVel.vel = {};

    HealthComponent playerHealth;
    playerHealth.health = 100;
    playerHealth.maxHealth = 100;
    m_s->engine.addEntity(Entity({ player, playerPos, playerAppVel, playerHealth, playerCollision }));

    InputComponent input;
    m_s->engine.addEntity(Entity({ input }));

    m_s->lastFrameTime = std::chrono::system_clock::now();
}

Scene::Scene(Scene&&) = default;
Scene& Scene::operator=(Scene&&) = default;
Scene::~Scene() = default;
}
