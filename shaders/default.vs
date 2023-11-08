#version 460 core // OpenGL 4.6

// input
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;
// output
out vec4 vertexColor;

layout (location = 0) uniform mat4 modelMatrix;         // locations:  0,  1,  2,  3
layout (location = 4) uniform mat4 viewMatrix;          // locations:  4,  5,  6,  7
layout (location = 8) uniform mat4 perspectiveMatrix;  // locations:  8,  9, 10, 11

void main() {

    // gl_Position is a predefined vertex shader output
    // think: out vec4 gl_Position;
    gl_Position = modelMatrix * vec4(pos.xyz, 1.0f);
    vertexColor = vec4(col.xyz, 1.0f);
}