#version 450 core
layout(location = 0) in vec2 v_uv;
layout(location = 1) in vec4 v_color;
layout(location = 2) flat in uint v_tex;

layout(location = 0) out vec4 frag_color;

void main() {
    // frag_color = v_color;
    frag_color = vec4(v_uv.x,v_uv.y,0,1);
}