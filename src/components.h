#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <unordered_map>

namespace ou {

struct SceneComponent {
    float aspectRatio;
    int windowWidth, windowHeight;
    float elapsedTime;
    int score;
};

struct HealthComponent {
    int maxHealth;
    int health;
};

struct ProjectileComponent {
    enum class Type {
        Bullet,
        Missile
    } type;
	float elapsedTime;
	int count;
};

struct PosComponent {
    glm::vec2 pos;
};

struct VelComponent {
    glm::vec2 vel;
};

struct ApparentVelComponent {
    glm::vec2 lastPos;
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
    float timeSinceStreak;
    float timeSinceBullet;
    float timeSinceHit;
    bool isDead = false;
    float timeSinceDead;
    glm::vec3 lastStreakColor = glm::vec3(0, 0, 1);
    glm::vec3 nextStreakColor = glm::vec3(0, 0, 1);
    float streakColorElapsed = 1;
    float timeSinceMissile = 0;
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
	ProjectileComponent::Type type;
};

struct CollidableComponent {
    float radius;
};

struct ShrapnelComponent {
    EntityType type;
    float scale;
    float elapsedTime;
};

struct StreakComponent {
    glm::vec3 color;
};

struct GravityComponent {};

}
#endif // COMPONENTS_H
