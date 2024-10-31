#version 460 core

// input
layout (location = 0) in vec4 in_pos;
layout (location = 1) in vec4 in_col;
// output
layout (location = 0) out vec4 out_col;
// uniforms
layout (location = 0) uniform mat4x4 model_transform;

void main() {
    gl_Position = model_transform * in_pos;
    out_col = in_col;
}