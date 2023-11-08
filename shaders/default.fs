#version 460 core // OpenGL 4.6

// input
in vec4 vertexColor;
// output
out vec4 pixelColor;

layout (location = 14) uniform vec4 uniformColor; // bound to shader before rendering
layout (location = 15) uniform vec4 A[2]; // takes up 2 location slots
layout (location = 17) uniform vec4 B; // since 16 is already in use, allot slot 17

void main() {

    // write to output texture in various ways

    // pixelColor = vec4(0.7f, 0.1f, 0.1f, 1.0f);
    // pixelColor = uniformColor;
    pixelColor = vertexColor;
}