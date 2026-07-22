#version 450 core

layout(location = 0) in vec2 a_pos;    // world units
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec4 a_color;

layout(location = 0) uniform vec4 u_scale_offset;

out vec2 v_uv;
out vec4 v_color;

void main()
{
    v_uv = a_uv;
    v_color = a_color;
    gl_Position = vec4(a_pos * u_scale_offset.xy + u_scale_offset.zw, 0.0, 1.0);
}