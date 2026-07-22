#version 450 core
layout(location = 0) in vec2 vUv;
layout(location = 0) out vec4 frag_color;
layout(binding = 0) uniform sampler2D u_src;

void main()
{
    frag_color = texture(u_src, vUv);
}