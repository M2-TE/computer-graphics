#pragma once

struct Material {
    void bind() {
        // bind each component to its location
        glUniform3f(17, ambient.r, ambient.g, ambient.b);
        glUniform3f(18, diffuse.r, diffuse.g, diffuse.b);
        glUniform3f(19, specular.r, specular.g, specular.b);
        glUniform1f(20, shininess);

        // binding texture seperately for now
        if (diffuseTexture > 0) {
            glBindTextureUnit(0, diffuseTexture);
        }
        else {
            // unbinds texture instead of binding new one
            // this seems to not work on some machines, gotta figure out why
            glBindTextureUnit(0, 0);
        }
    }

    glm::vec3 ambient = glm::vec3(0.1f); // first slot
    glm::vec3 diffuse = glm::vec3(1.0f); // second slot
    glm::vec3 specular = glm::vec3(0.0f); // third slot
    float shininess = 16.0f; // fourth slot
    
    // there can be multiple ambient/diffuse/specular textures at once
    // and they are combined with a blend factor
    // we ignore that for now
    GLuint diffuseTexture;
};