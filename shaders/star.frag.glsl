#version 430 core

layout(location = 1) uniform vec2 u_ScreenSize;

in vec3 v_color;
in float v_size;

layout(location = 0) out vec4 final_color;

void main(void) {
    float alpha = 1 - length(gl_PointCoord.st * 2 - 1);
    alpha = smoothstep(0.0, 0.5, alpha);
    final_color = vec4(v_color, alpha);
}
