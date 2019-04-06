#include "components.h"

namespace ou {

bool InputComponent::isKeyPressed(unsigned char key) const
{
    auto it = keyStates.find(key);
    if (it != keyStates.end()) {
        return it->second;
    }
    return false;
}
}
