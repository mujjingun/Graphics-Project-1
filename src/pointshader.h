#ifndef POINTSHADER_H
#define POINTSHADER_H

#include "shader.h"

namespace ou {

class PointShader : public Shader {
    PointShader();

public:
    static PointShader* self();
};
}

#endif // POINTSHADER_H
