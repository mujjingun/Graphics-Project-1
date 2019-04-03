#include "hpbarshader.h"

namespace ou {
HpBarShader::HpBarShader()
    : Shader("shaders/hpbar.vert.glsl", "shaders/hpbar.frag.glsl")
{
}

HpBarShader* HpBarShader::self()
{
    static HpBarShader me;
    return &me;
}
}
