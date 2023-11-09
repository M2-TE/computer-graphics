#version 460 core // OpenGL 4.6

// input
in vec4 vertexColor;
in vec2 uvCoord;
// output
out vec4 pixelColor;

uniform sampler2D textureSampler;

void main() {

    // write to output texture in various ways

    // pixelColor = vec4(0.7f, 0.1f, 0.1f, 1.0f);
    // pixelColor = uniformColor;
    // pixelColor = vertexColor;
    pixelColor = texture(textureSampler, uvCoord) * vertexColor;
}