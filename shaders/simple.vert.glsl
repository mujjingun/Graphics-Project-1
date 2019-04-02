#version 430 core

layout(location = 0) uniform mat4 u_ModelViewProjectionMatrix;
layout(location = 1) uniform vec3 u_color;

layout(location = 0) in vec4 a_position;
out vec4 v_color;

void main(void) {
    v_color = vec4(u_color, 1.0f);
    gl_Position =  u_ModelViewProjectionMatrix * a_position;
}
