#include "simpleshader.h"

namespace ou {

SimpleShader::SimpleShader()
    : Shader("shaders/simple.vert.glsl", "shaders/simple.frag.glsl")
{
}

SimpleShader* SimpleShader::self()
{
    static SimpleShader me;
    return &me;
}
}
