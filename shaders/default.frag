#version 460 core

// interpolated input from vertex shader
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_norm;
layout (location = 2) in vec2 in_uv;
// final pixel/fragment color
layout (location = 0) out vec4 out_color;
// texture unit
layout (binding = 0) uniform sampler2D tex_diffuse;
// uniform buffers
layout (location = 12) uniform vec3 camera_pos;
layout (location = 13) uniform float specular = 1.0;
layout (location = 14) uniform float specular_shininess = 16.0;

void main() {
    // create "sun"
    vec3 light_pos = vec3(1.0, 2.0, -3.5);
    vec3 light_col = vec3(.992, .984, .827);
    vec3 light_dir = normalize(light_pos - in_pos); // unit vector from light to fragment/pixel

    // ambient color (low scattered indirect light)
    float ambient_strength = 0.1;
    vec3 ambient_col = light_col * ambient_strength;

    // direct light color
    float diffuse_strength = dot(in_norm, light_dir);
    diffuse_strength = max(diffuse_strength, 0.0); // filter out "negative" strength
    vec3 diffuse_col = light_col * diffuse_strength;

    // specular color (reflected directly to camera)
    vec3 camera_dir = normalize(camera_pos - in_pos); // unit vector from camera to fragment/pixel
    vec3 reflected_dir = reflect(-light_dir, in_norm);
    float specular_strength = dot(camera_dir, reflected_dir);
    specular_strength = max(specular_strength, 0.0); // no negative specular light
    specular_strength = pow(specular_strength, specular_shininess);
    specular_strength = specular_strength * specular;
    vec3 specular_col = light_col * specular_strength;

    // final color
    vec3 color = texture(tex_diffuse, in_uv).rgb;
    color = color * (ambient_col + diffuse_col + specular_col); // combine light-affected colors
    out_color = vec4(color, 1.0);
}