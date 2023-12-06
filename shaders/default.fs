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

    // final color (blend/interpolate vertex color with texture color)
    vec4 sampledColor = texture(diffuseTexture, uvCoord);
    vec4 color = mix(vertCol, sampledColor, material.diffuseBlend);
    // combine with light colors
    color = vec4(color.rgb * (ambientColor + diffuseColor + specularColor), color.a);

    // lazy (and very efficient) alpha blending
    // can glDisable(GL_BLENDING) for this
    // if (color.a < 0.5) {
    //     discard; // discard current fragment/pixel
    // }

    // see if there are any alpha values between 0.1 and 0.9
    // if (color.a > 0.1 && color.a < 0.9) {
    //     color.rgb = vec3(1, 1, 1);
    // }

    // write to screen
    pixelColor = color;
}