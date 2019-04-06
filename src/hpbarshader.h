#ifndef HPBARSHADER_H
#define HPBARSHADER_H

#include "graphics/shader.h"

namespace ou {

class HpBarShader : public Shader {
    HpBarShader();
public:
    static HpBarShader* self();
};
}

#endif // HPBARSHADER_H
