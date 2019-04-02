#include "pointshader.h"

ou::PointShader::PointShader()
    : Shader("shaders/star.vert.glsl", "shaders/star.frag.glsl")
{
}

ou::PointShader* ou::PointShader::self()
{
    static PointShader sself;
    return &sself;
}
