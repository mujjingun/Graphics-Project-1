#version 430 core

layout(location = 0) uniform mat4 u_ModelViewProjectionMatrix;
layout(location = 1) uniform float u_hpValue;

layout(location = 0) in vec2 a_position;
out vec2 v_pos;

void main(void) {
    v_pos = a_position;
    gl_Position = u_ModelViewProjectionMatrix * vec4(a_position, 0, 1);
}
