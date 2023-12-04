#version 460 core // OpenGL 4.6

// input (location matches vertex shader "out")
layout (location = 0) in vec3 worldPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvCoord;
// output
out vec4 pixelColor;

layout (location = 16) uniform vec3 cameraWorldPos;

// define custom struct
struct Material {
    vec3 ambient; // first slot
    vec3 diffuse; // second slot
    vec3 specular; // third slot
    float shininess; // fourth slot
    float shininessStrength; // fifth slot
};

// Material needs 4 slots, similar to a matrix
layout (location = 17) uniform Material material;
uniform sampler2D diffuseTexture;

void main() {

    // create our "sun"
    vec3 lightColor = vec3(.992, .984, .827);
    vec3 lightWorldPos = vec3(0.0, 300.0, 0.0);
    vec3 lightDir = normalize(lightWorldPos - worldPos); // unit vector from light to fragment

    // ambient color (low light from scattered sunlight)
    float ambientStrength = 1.0; // ambient modifier
    vec3 ambientColor = lightColor * ambientStrength * material.ambient;

    // light color
    float diffuseStrength = dot(normal, lightDir); // calc intensity of light
    diffuseStrength = max(diffuseStrength, 0.0); // filter out negative intensity
    vec3 diffuseColor = lightColor * diffuseStrength * material.diffuse;

    // specular color
    vec3 cameraDir = normalize(cameraWorldPos - worldPos); // unit vector from camera to fragment
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularStrength = material.shininessStrength; // specular modifier
    specularStrength *= pow(max(dot(cameraDir, reflectDir), 0.0), material.shininess);
    vec3 specularColor = lightColor * specularStrength * material.specular;

    // final color
    vec3 color = texture(diffuseTexture, uvCoord).rgb; // sample texture
    // vec3 color = vec3(1.0f, 1.0f, 1.0f); // sample texture
    color = color * (ambientColor + diffuseColor + specularColor); // combine light colors

    // write to screen
    pixelColor = vec4(color, 1.0);
}