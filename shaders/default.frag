#version 460 core

// interpolated input from vertex shader
layout (location = 0) in vec4 in_color;
layout (location = 1) in vec2 in_uv;
// final pixel/fragment color
layout (location = 0) out vec4 out_color;
// texture unit
layout (binding = 0) uniform sampler2D tex_diffuse;

void main() {
    // out_color = in_color;
    out_color = texture(tex_diffuse, in_uv);
}