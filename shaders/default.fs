#version 460 core // OpenGL 4.6

// input (location matches vertex shader "out")
layout (location = 0) in vec3 worldPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 vertexColor;
layout (location = 3) in vec2 uvCoord;
// output
out vec4 pixelColor;

uniform sampler2D textureSampler;
layout (location = 16) uniform vec3 cameraWorldPos;

void main() {
    // create our "sun"
    vec3 lightColor = vec3(.992f, .984f, .827f);
    vec3 lightWorldPos = vec3(1.5f, 4.0f, 3.0f);
    vec3 lightDir = normalize(lightWorldPos - worldPos); // unit vector from light to fragment

    // ambient color (low light from scattered sunlight)
    float ambientStrength = 0.1f; // arbitrary setting
    vec3 ambientColor = lightColor * ambientStrength;

    // light color
    float diffuseStrength = dot(normal, lightDir); // calc intensity of light
    diffuseStrength = max(diffuseStrength, 0.0f); // filter out negative intensity
    vec3 diffuseColor = lightColor * diffuseStrength;

    // specular color
    vec3 cameraDir = normalize(cameraWorldPos - worldPos); // unit vector from camera to fragment
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularStrength = 0.5f; // initial strength (0.0f to 1.0f)
    float shininess = 32.0f; // controls size/intensity of specular highlight
    specularStrength *= pow(max(dot(cameraDir, reflectDir), 0.0), shininess);
    vec3 specularColor = lightColor * specularStrength;

    // final color
    vec3 color = texture(textureSampler, uvCoord).rgb; // sample texture
    color = color * (ambientColor + diffuseColor + specularColor); // combine light colors

    // write to screen
    pixelColor = vec4(color, 1.0f);
}