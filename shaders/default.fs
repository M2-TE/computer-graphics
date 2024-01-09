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
layout (location = 35) uniform float lightFar;

// texture samplers
layout (binding = 0) uniform sampler2D diffuseTexture;
layout (binding = 1) uniform samplerCube shadowMap;

vec3 calc_ambient() {
    float ambientStrength = 1.0; // ambient modifier
    return lightColor * ambientStrength * material.ambient;
}
vec3 calc_diffuse() {
    vec3 lightDir = normalize(lightWorldPos - worldPos); // unit vector from light to fragment
    float diffuseStrength = dot(normal, lightDir); // calc intensity of light
    diffuseStrength = max(diffuseStrength, 0.0); // filter out negative intensity
    return lightColor * diffuseStrength * material.diffuse;
}
vec3 calc_specular() {
    vec3 lightDir = normalize(lightWorldPos - worldPos); // unit vector from light to fragment
    vec3 cameraDir = normalize(cameraWorldPos - worldPos); // unit vector from camera to fragment
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularStrength = material.shininessStrength; // specular modifier
    specularStrength *= pow(max(dot(cameraDir, reflectDir), 0.0), material.shininess);
    return lightColor * specularStrength * material.specular;
}
float calc_shadow() {
    vec3 lightDir = normalize(lightWorldPos - worldPos); // unit vector from light to fragment
    vec3 fragToLight = worldPos - lightWorldPos;
    float bias = max(0.5 * (1.0 - dot(normal, lightDir)), 0.005);  
    // float closestDepth = texture(shadowMap, fragToLight).r; // depth as seen from light
    // closestDepth *= lightFar;
    // float currentDepth = length(fragToLight);
    // float shadow = currentDepth - bias > closestDepth ? 0.0 : 1.0;
    
    // percentage closer filter
    float samples = 4.0;
    float offset  = 0.1;
    float shadow = 0.0;
    for(float x = -offset; x < offset; x += offset / (samples * 0.5)) {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5)) {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5)) {
                float closestDepth = texture(shadowMap, fragToLight + vec3(x, y, z)).r; 
                closestDepth *= lightFar;
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

void main() {
    pixelColor = calc_light();
    // pixelColor = vec4(vec3(closestDepth / 50.0), 1.0);
}