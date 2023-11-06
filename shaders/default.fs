#version 460 core // OpenGL 4.6

out vec4 color; // the output to write to the texture

void main() {

    // we simply write a preset color
    color = vec4(0.7f, 0.1f, 0.1f, 1.0f);
}