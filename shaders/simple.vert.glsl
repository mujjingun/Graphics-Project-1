#version 430 core

layout(location = 0) uniform mat4 u_ModelViewProjectionMatrix;

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec3 a_color;
out vec4 v_color;

void main(void) {
    v_color = vec4(a_color, 1.0f);
    gl_Position =  u_ModelViewProjectionMatrix * vec4(a_position, 0, 1);
}
