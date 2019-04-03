#ifndef HPBAR_H
#define HPBAR_H

#include <glm/glm.hpp>

namespace ou {

class HpBar {
public:
    HpBar();
    void render(glm::mat4 mat, float value);
};
}

#endif // HPBAR_H
