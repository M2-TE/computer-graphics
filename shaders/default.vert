#version 460 core

// input
layout (location = 0) in vec4 in_pos;
layout (location = 1) in vec4 in_col;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in vec3 in_norm;
// output
layout (location = 0) out vec3 out_pos;
layout (location = 1) out vec3 out_norm;
layout (location = 2) out vec2 out_uv;
// uniforms
// mat4x4 takes up for locations, each location being 16 bytes
layout (location = 0) uniform mat4x4 model_transform;
layout (location = 4) uniform mat4x4 camera_transform;
layout (location = 8) uniform mat4x4 camera_perspective;

void main() {
    gl_Position = model_transform * in_pos;
    out_pos = gl_Position.xyz;
    gl_Position = camera_transform * gl_Position;
    gl_Position = camera_perspective * gl_Position;
    
    // only rotate the normal
    out_norm = normalize(mat3x3(model_transform) * in_norm);
    out_uv = in_uv;
}