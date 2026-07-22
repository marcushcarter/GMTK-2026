#version 450 core
layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec4 a_color;
layout(location = 3) in uint a_tex;

layout(location = 0) uniform vec4 u_scale_offset;

layout(location = 0) out vec2 v_uv;
layout(location = 1) out vec4 v_color;
layout(location = 2) flat out uint v_tex;

void main() {
    v_uv = a_uv;
    v_color = a_color;
    v_tex = a_tex;
    gl_Position = vec4(a_pos * u_scale_offset.xy + u_scale_offset.zw, 0.0, 1.0);
}