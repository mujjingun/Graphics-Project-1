#version 430 core

layout(location = 0) uniform mat4 u_ModelViewProjectionMatrix;

layout(location = 0) in vec2 a_position;
layout(location = 1) in float a_size;
layout(location = 2) in vec3 a_color;
out vec3 v_color;
out float v_size;

void main(void) {
    v_color = a_color;
    v_size = a_size;
    gl_Position = u_ModelViewProjectionMatrix * vec4(a_position, 0, 1);
    gl_PointSize = a_size;
}
