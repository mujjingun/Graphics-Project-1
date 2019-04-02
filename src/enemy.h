#ifndef ENEMY_H
#define ENEMY_H

#include <glm/glm.hpp>

namespace ou {

class Enemy {
protected:
    glm::vec2 m_pos;

public:
    Enemy(glm::vec2 pos);
    virtual ~Enemy() = default;

    virtual void render() = 0;
};
}

#endif // ENEMY_H
