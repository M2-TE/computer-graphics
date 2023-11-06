#version 460 core // OpenGL 4.6

layout (location = 0) in vec3 pos; // layout of the input vertices

void main() {

    // gl_Position is the vertex shader output (predefined by GLSL)
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f);
}