#version 460 core // OpenGL 4.6

// input (location matches vertex shader "out")
layout (location = 0) in vec3 worldPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvCoord;
layout (location = 3) in vec4 vertCol;
// output
layout (location = 0) out vec4 pixelColor;

struct Camera {
    vec3 worldPos; // 16
};
struct Material {
    vec3 ambient; // 17
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float shininessStrength;
    float diffuseBlend;
};
struct Light {
    vec3 worldPos; // 23
    vec3 color;
    float radius;
};
// uniform constants
layout (location = 16) uniform Camera camera;
layout (location = 17) uniform Material material;
layout (location = 23) uniform Light light;

// texture samplers
layout (binding = 0) uniform sampler2D diffuseTexture;
layout (binding = 1) uniform samplerCube shadowMap;

vec3 calc_ambient() {
    float ambientStrength = 1.0; // ambient modifier
    return light.color * ambientStrength * material.ambient;
}
vec3 calc_diffuse() {
    vec3 lightDir = normalize(light.worldPos - worldPos); // unit vector from light to fragment
    float diffuseStrength = dot(normal, lightDir); // calc intensity of light
    diffuseStrength = max(diffuseStrength, 0.0); // filter out negative intensity
    return light.color * diffuseStrength * material.diffuse;
}
vec3 calc_specular() {
    vec3 lightDir = normalize(light.worldPos - worldPos); // unit vector from light to fragment
    vec3 cameraDir = normalize(camera.worldPos - worldPos); // unit vector from camera to fragment
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularStrength = material.shininessStrength; // specular modifier
    specularStrength *= pow(max(dot(cameraDir, reflectDir), 0.0), material.shininess);
    return light.color * specularStrength * material.specular;
}
float calc_shadow() {
    vec3 lightDir = normalize(light.worldPos - worldPos); // unit vector from light to fragment
    vec3 fragToLight = worldPos - light.worldPos;
    float bias = max(0.5 * (1.0 - dot(normal, lightDir)), 0.005);  
    float currentDepth = length(fragToLight);
    
    // percentage closer filter
    float samples = 4.0;
    float offset  = 0.1;
    float shadow = 0.0;
    for(float x = -offset; x < offset; x += offset / (samples * 0.5)) {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5)) {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5)) {
                float closestDepth = texture(shadowMap, fragToLight + vec3(x, y, z)).r; 
                closestDepth *= light.radius;
                if(currentDepth - bias < closestDepth) shadow += 1.0;
            }
        }
    }
    return shadow / (samples * samples * samples);
}
vec4 calc_light() {
    vec3 ambientColor = calc_ambient(); // indirect scattered light
    vec3 diffuseColor = calc_diffuse(); // direct light
    vec3 specularColor = calc_specular(); // specular highlights (from direct light)
    float shadow = calc_shadow(); // shadow influence

    // blend texture/vertex colors
    vec4 sampledColor = texture(diffuseTexture, uvCoord);
    vec4 color = mix(vertCol, sampledColor, material.diffuseBlend);

    // blend sampled color with light
    return vec4(color.rgb * (ambientColor + shadow * (diffuseColor + specularColor)), color.a);
}
vec4 calc_debug() {
    vec3 fragToLight = worldPos - light.worldPos;
    float closestDepth = texture(shadowMap, fragToLight).r; // depth as seen from light
    closestDepth *= light.radius;
    return vec4(vec3(closestDepth / light.radius), 1.0);
}

void main() {
    pixelColor = calc_light();
    // pixelColor = calc_debug();
}