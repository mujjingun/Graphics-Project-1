#ifndef SIMPLESHADER_H
#define SIMPLESHADER_H

#include "shader.h"

namespace ou {

class SimpleShader : public Shader {
    SimpleShader();

public:
    static SimpleShader* self();
};
}
#endif // SIMPLESHADER_H
