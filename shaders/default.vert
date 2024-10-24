#version 460 core

// input
layout (location = 0) in vec4 in_pos;
// output
// -> none yet
// uniforms
layout (location = 0) uniform vec4 vert_offset;

void main() {
    gl_Position = in_pos + vert_offset;
}