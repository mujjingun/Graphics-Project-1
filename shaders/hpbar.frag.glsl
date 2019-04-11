#version 430 core

layout(location = 0) uniform mat4 u_ModelViewProjectionMatrix;
layout(location = 1) uniform float u_hpValue;

in vec2 v_pos;

layout(location = 0) out vec4 final_color;

void main(void) {
    if ((v_pos.x + 1) * .5 < u_hpValue) {
        final_color = mix(vec4(2, 0, 0, 1), vec4(0, 2, 0, 1), u_hpValue);
    }
    else {
        final_color = vec4(0.1, 0.1, 0.1, 1);
    }
}
