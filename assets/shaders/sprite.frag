#version 450 core

in vec2 v_uv;
in vec4 v_color;

layout(location = 0) out vec4 frag_color;

void main()
{
    // Placeholder: red ramps along +X within the tile, green along +Y.
    frag_color = vec4(v_uv.x, v_uv.y, 0.35, 1.0) * v_color;
}