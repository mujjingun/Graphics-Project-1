#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>

struct SceneComponent {
    float aspectRatio;
    glm::mat4 viewProjMat;
};

struct HealthComponent {
    int maxHealth;
    int health;
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

struct InputComponent {
};

struct PlayerComponent {
};

#endif // COMPONENTS_H
