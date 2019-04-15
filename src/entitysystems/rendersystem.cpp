#include "rendersystem.h"
#include "components.h"
#include "ecs/ecsengine.h"
#include "ecs/entity.h"
#include "models.h"
#include "pointshader.h"
#include "simpleshader.h"

#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>

namespace ou {

namespace {
    const int max_particles = 700;
}

RenderSystem::RenderSystem()
    : m_pointsBuf(max_particles)
{
}

void RenderSystem::update(ECSEngine& engine, float)
{
    SceneComponent const& scene = engine.getOne<SceneComponent>();
    Entity const& player = engine.getOneEnt<PlayerComponent>();
	InputComponent const& input = engine.getOne<InputComponent>();

    glm::mat4 projMat;
    const float realAspectRatio = scene.windowWidth / float(scene.windowHeight);
    if (scene.windowWidth * scene.aspectRatio > scene.windowHeight) {
        projMat = glm::ortho(-scene.aspectRatio * realAspectRatio,
            scene.aspectRatio * realAspectRatio,
            -scene.aspectRatio, scene.aspectRatio, -1000.0f, 1000.0f);
    } else {
        projMat = glm::ortho(-1.0f, 1.0f, -1 / realAspectRatio, 1 / realAspectRatio, -1000.0f, 1000.0f);
    }

    glm::mat4 viewMat = glm::mat4(1.0f);

    PlayerComponent const& playerComp = player.get<PlayerComponent>();
    if (playerComp.timeSinceHit < 0.5f) {
        float jitter = glm::sin(playerComp.timeSinceHit * 80);
        jitter *= (0.5f - playerComp.timeSinceHit) * 0.05f;
        viewMat = viewMat * glm::translate(glm::mat4(1), glm::vec3(jitter));
    }

    glm::mat4 viewProjMat = projMat * viewMat;

    // render background stars and projectiles
    std::vector<PointAttrib> attribs;

    // projectiles
    for (Entity& proj : engine.iterate<ProjectileComponent>()) {
        PosComponent pos = proj.get<PosComponent>();

        PointAttrib attrib;
        attrib.pos = pos.pos;

		ProjectileComponent const& comp = proj.get<ProjectileComponent>();

        switch (comp.type) {
        case ProjectileComponent::Type::Bullet:
            attrib.color = { 5, 5, 5 };
            attrib.size = 6;
            break;
        case ProjectileComponent::Type::Missile:
            attrib.color = glm::mix(glm::vec3(4, 0, 0), glm::vec3(0, 5, 0), comp.count / 3.0f);
            attrib.size = 12;
            break;
        }

        if (attribs.size() < max_particles) {
            attribs.push_back(attrib);
        }
    }

    // background stars
    for (Entity& ent : engine.iterate<StarComponent>()) {
        StarComponent star = ent.get<StarComponent>();
        PosComponent pos = ent.get<PosComponent>();

        PointAttrib attrib;
        attrib.pos = pos.pos;
        attrib.pos -= player.get<PosComponent>().pos * float(star.size) * 0.1f;
        attrib.color = star.color;
        attrib.size = float(star.size);

        if (attribs.size() < max_particles) {
            attribs.push_back(attrib);
        }
    }

    for (Entity& ent : engine.iterate<StreakComponent>()) {
        PosComponent pos = ent.get<PosComponent>();

        PointAttrib attrib;
        attrib.pos = pos.pos;
        attrib.color = ent.get<StreakComponent>().color;
        attrib.size = 5;

        if (attribs.size() < max_particles) {
            attribs.push_back(attrib);
        }
    }

    m_pointsBuf.vbo.updateData(attribs);

    PointShader::self()->setUniform(0, viewProjMat);

    m_pointsBuf.vao.use();
    PointShader::self()->use();
    glDrawArrays(GL_POINTS, 0, int(attribs.size()));

    // render shrapnels
    auto render = [](EntityType type, glm::mat4 mat) {
        if (type == EntityType::HOUSE) {
            HouseData::self()->render(mat * glm::scale(glm::mat4(1.0f), glm::vec3(1 / 18.0f)));
        } else if (type == EntityType::CAR) {
            CarData::self()->render(mat * glm::scale(glm::mat4(1.0f), glm::vec3(1 / 18.0f)));
        } else if (type == EntityType::COCKTAIL) {
            CocktailData::self()->render(mat * glm::scale(glm::mat4(1.0f), glm::vec3(1 / 18.0f)));
        } else if (type == EntityType::SQUID) {
            SquidData::self()->render(mat * glm::translate(glm::mat4(1.0), glm::vec3(-1))
                * glm::scale(glm::mat4(1.0), glm::vec3(1.f / 16)));
        } else if (type == EntityType::BALLOON) {
            BalloonData::self()->render(mat * glm::translate(glm::mat4(1.0), glm::vec3(-1))
                * glm::scale(glm::mat4(1.0), glm::vec3(1.f / 16)));
        } else if (type == EntityType::PLAYER) {
            PlaneData::self()->render(mat * glm::scale(glm::mat4(1.0f), glm::vec3(1 / 18.0f)));
        }
    };

    for (Entity& shrapnel : engine.iterate<ShrapnelComponent>()) {
        ShrapnelComponent comp = shrapnel.get<ShrapnelComponent>();
        float angle = shrapnel.get<AngleComponent>().angle;
        glm::vec2 pos = shrapnel.get<PosComponent>().pos;

        glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos, 0))
            * glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(0, 0, 1))
            * glm::scale(glm::mat4(1.0), glm::vec3(comp.scale));

        render(comp.type, viewProjMat * modelMat);
    }

    // render enemies
    for (Entity& enemy : engine.iterate<EnemyComponent>()) {
        EnemyComponent comp = enemy.get<EnemyComponent>();
        CollidableComponent collide = enemy.get<CollidableComponent>();

        float damageScale = 1 + glm::max(0.0f, 0.1f - comp.timeSinceHit) * 5.f;

        glm::vec2 pos = enemy.get<PosComponent>().pos;
        pos.x -= player.get<PosComponent>().pos.x * 0.05f;

        float angle = enemy.get<AngleComponent>().angle;

        glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos, 0));
        float scale;
        switch (comp.type) {
        case EntityType::HOUSE:
            scale = glm::sin(scene.elapsedTime * 10) * 0.5f + 1.0f;
            modelMat = modelMat
                * glm::translate(glm::mat4(1.0), glm::vec3(0, -.15f, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(1, scale, 1))
                * glm::translate(glm::mat4(1.0), glm::vec3(0, +.15f, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(collide.radius * damageScale));
            break;
        case EntityType::CAR:
            scale = glm::sin(scene.elapsedTime * 10) * 0.5f + 1.0f;
            modelMat = modelMat
                * glm::scale(glm::mat4(1.0), glm::vec3(scale, 1, 1))
                * glm::scale(glm::mat4(1.0), glm::vec3(collide.radius * damageScale));
            break;
        case EntityType::COCKTAIL:
            modelMat = modelMat
                * glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(0, 0, 1))
                * glm::scale(glm::mat4(1.0), glm::vec3(collide.radius * damageScale));
            break;
        case EntityType::SQUID:
            scale = glm::sin(scene.elapsedTime * 20) * 0.3f + 1.0f;
            modelMat = modelMat
                * glm::translate(glm::mat4(1.0), glm::vec3(0, -.15f, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(1, scale, 1))
                * glm::translate(glm::mat4(1.0), glm::vec3(0, +.15f, 0))
                * glm::scale(glm::mat4(1.0), glm::vec3(collide.radius * damageScale));
            break;
        case EntityType::BALLOON:
            modelMat = modelMat
                * glm::scale(glm::mat4(1.0), glm::vec3(collide.radius * damageScale));
            break;
        default:
            break;
        }

        render(comp.type, viewProjMat * modelMat);

        HealthComponent health = enemy.get<HealthComponent>();
        glm::mat4 hpBarModelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos + glm::vec2(0, collide.radius), 0.0f))
            * glm::scale(glm::mat4(1.0), glm::vec3(0.1f, 0.01f, 0.0f));
        m_hpBar.render(viewProjMat * hpBarModelMat, float(health.health) / health.maxHealth);
    }

    // render player
    if (!playerComp.isDead) {
        float scaleFactor = (glm::sin(scene.elapsedTime * 20) * 0.1f + 1.0f) * 0.08f;
        glm::vec2 pos = player.get<PosComponent>().pos;
        float offset = glm::pow(scene.elapsedTime < 1 ? 1.0f - float(scene.elapsedTime) : 0.0f, 2.0f);
        pos.y -= offset;

		glm::mat4 skewMat(1.0);
		if (input.isKeyPressed('d')) {
			skewMat[0][1] = glm::tan(glm::radians(-10.0f));
		}
		if (input.isKeyPressed('a')) {
			skewMat[0][1] = glm::tan(glm::radians(10.0f));
		}

        glm::mat4 modelMat = glm::translate(glm::mat4(1.0), glm::vec3(pos, 0.0f))
			* skewMat
            * glm::scale(glm::mat4(1.0), glm::vec3(scaleFactor))
            * glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0, 0, 1));
		
        render(EntityType::PLAYER, viewProjMat * modelMat);
    }

    // render health bar
    glm::mat4 hpBarProjMat = glm::ortho(-1.0f, 1.0f,
        -scene.aspectRatio, scene.aspectRatio, -1000.0f, 1000.0f);

    float hpBarScale = 0.02f;
    if (playerComp.timeSinceHit < 0.5f) {
        hpBarScale += (0.5f - playerComp.timeSinceHit) * 0.4f;
    }

    glm::mat4 hpBarModelMat = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -scene.aspectRatio, 0.0f))
        * glm::scale(glm::mat4(1.0), glm::vec3(1.f, hpBarScale, 0.0f));

    HealthComponent playerHealth = player.get<HealthComponent>();
    float playerHealthRatio = float(playerHealth.health) / playerHealth.maxHealth;
    m_hpBar.render(hpBarProjMat * hpBarModelMat, playerHealthRatio);

    // update title bar
    std::ostringstream oss;
    oss << "Sogang CSE4170 20171634 -- Score " << scene.score;

    if (playerComp.isDead) {
        oss << ", Game over";
    }

    glutSetWindowTitle(oss.str().c_str());
}
}
