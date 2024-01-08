#version 460 core // OpenGL 4.6

// input (location matches vertex shader "out")
layout (location = 0) in vec3 worldPos;

layout (location = 16) uniform vec3 cameraWorldPos;
layout (location = 17) uniform vec3 lightWorldPos;
layout (location = 18) uniform vec3 lightColor;

struct Material {
    vec3 ambient; // first slot
    vec3 diffuse; // second slot
    vec3 specular; // third slot
    float shininess; // etc
    float shininessStrength;
    float diffuseBlend;
};
layout (location = 20) uniform Material material;
layout (location = 26) uniform mat4 lightViewMatrix;
layout (location = 30) uniform mat4 lightPerspectiveMatrix;
layout (location = 34) uniform vec3 lightCameraWorldPos;

void main() {
    // do not write any color output
    float lightDistance = length(worldPos - lightWorldPos);
    lightDistance = lightDistance / 100.0;
    gl_FragDepth = lightDistance;
}