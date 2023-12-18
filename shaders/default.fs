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

// texture samplers
layout (binding = 0) uniform sampler2D diffuseTexture;
layout (binding = 1) uniform sampler2D shadowMap;

void main() {
    // create our "sun"
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

    // shadow color
    // calculate shadow coord for current fragment (could do this in vertex shader)
    vec4 shadowCoord = lightPerspectiveMatrix * lightViewMatrix * vec4(worldPos, 1.0);
    // perspective divide (depth is stored in shadowCoord.w)
    // orthographic projection would not need this
    // this is done automatically for gl_Position in vertex shader
    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
    // convert from range [-1, 1] to [0, 1]
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z; // theoretical depth of current fragment
    // calculate shadow
    // if the theoretical depth is larger than the depth as seen from the light, its in shadow
    float minBias = 0.005;
    float maxBias = 0.05;
    float bias = max(maxBias * (1.0 - dot(normal, lightDir)), minBias); // higher bias on higher viewing angles
    // float closestDepth = texture(shadowMap, projCoords.xy).r; // depth as seen from light
    // float shadow = currentDepth - bias > closestDepth ? 0.0 : 1.0;

    // percentage closer filter (PCF)
    float shadow = 0.0;
    float shadowMapSize = 1024.0;
    float texelSize = 1.0 / shadowMapSize;
    // sample a 3x3 area of pixels
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 0.0 : 1.0;
        }
    }
    // get average shadow value
    shadow /= 9.0;


    // final color (blend/interpolate vertex color with texture color)
    vec4 sampledColor = texture(diffuseTexture, uvCoord);
    vec4 color = mix(vertCol, sampledColor, material.diffuseBlend);
    // combine with light colors
    color = vec4(color.rgb * (ambientColor + shadow * (diffuseColor + specularColor)), color.a);

    // write to screen
    pixelColor = color;
}