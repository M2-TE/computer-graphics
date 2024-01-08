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
    vec3 fragToLight = worldPos - lightWorldPos;
    float closestDepth = texture(shadowMap, fragToLight).r; // depth as seen from light
    closestDepth *= lightFar;
    float currentDepth = length(fragToLight);
    float bias = 0.1;
    float shadow = currentDepth - bias > closestDepth ? 0.0 : 1.0;
    
    // percentage closer filter
    float samples = 4.0;
    float offset  = 0.1;
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float closestDepth = texture(shadowMap, fragToLight + vec3(x, y, z)).r; 
                closestDepth *= lightFar;   // undo mapping [0;1]
                if(currentDepth - bias < closestDepth) shadow += 1.0;
            }
        }
    }
    shadow /= (samples * samples * samples);

    // final color (blend/interpolate vertex color with texture color)
    vec4 sampledColor = texture(diffuseTexture, uvCoord);
    vec4 color = mix(vertCol, sampledColor, material.diffuseBlend);
    // combine with light colors
    color = vec4(color.rgb * (ambientColor + shadow * (diffuseColor + specularColor)), color.a);

    // write to screen
    pixelColor = color;
    // pixelColor = vec4(vec3(closestDepth / 50.0), 1.0);
}