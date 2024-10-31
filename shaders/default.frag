#version 460 core

// interpolated color from vertices
layout (location = 0) in vec4 in_color;
// final pixel/fragment color
layout (location = 0) out vec4 out_color;

void main() {
    out_color = in_color;
}