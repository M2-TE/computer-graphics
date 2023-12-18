#version 460 core // OpenGL 4.6

// input (location matches vertex shader "out")
layout (location = 0) in vec3 worldPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvCoord;
layout (location = 3) in vec4 vertCol;
// output
layout (location = 0) out vec4 pixelColor;

layout (location = 16) uniform vec3 cameraWorldPos;
layout (location = 17) uniform vec3 lightWorldPos;
layout (location = 18) uniform vec3 lightColor;

// define custom struct
struct Material {
    vec3 ambient; // first slot
    vec3 diffuse; // second slot
    vec3 specular; // third slot
    float shininess; // etc
    float shininessStrength;
    float diffuseBlend;
};

// Material needs multiple slots
layout (location = 20) uniform Material material;
layout (binding = 0) uniform sampler2D diffuseTexture;

void main() {
    // do not write any color output
}