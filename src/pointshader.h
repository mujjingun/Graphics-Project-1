#ifndef POINTSHADER_H
#define POINTSHADER_H

#include "graphics/shader.h"

namespace ou {

class PointShader : public Shader {
    PointShader();

public:
    static PointShader* self();
};
}

#endif // POINTSHADER_H
