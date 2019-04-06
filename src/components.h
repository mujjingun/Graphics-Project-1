#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <unordered_map>

namespace ou {

struct SceneComponent {
    float aspectRatio;
    int windowWidth, windowHeight;
    float elapsedTime;
};

struct HealthComponent {
    int maxHealth;
    int health;
};

struct ProjectileComponent {
    enum class Type {
        Player,
    } type;
};

struct PosComponent {
    glm::vec2 pos;
};

struct VelComponent {
    glm::vec2 vel;
};

struct StarComponent {
    int size;
    glm::vec3 color;
};

struct AngleComponent {
    float angle;
};

struct AngleSpeedComponent {
    float angleSpeed;
};

struct InputComponent {
    std::unordered_map<unsigned char, bool> keyStates;

    bool isKeyPressed(unsigned char key) const;
};

struct PlayerComponent {
    glm::vec2 dest;
    float timeSinceBullet;
    float timeSinceHit;
    bool isDead = false;
    float timeSinceDead;
};

enum class EntityType {
    PLAYER,
    CAR,
    HOUSE,
    COCKTAIL,
    SQUID,
    BALLOON,
};

struct EnemyComponent {
    EntityType type;
    int shrapnelCount;
    float timeSinceHit;
};

struct CollisionComponent {
};

struct CollidableComponent {
    float radius;
};

struct ShrapnelComponent {
    EntityType type;
    float scale;
    float elapsedTime;
};

}
#endif // COMPONENTS_H
