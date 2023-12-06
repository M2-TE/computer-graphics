#pragma once

struct Material {
    void bind() {
        // bind each component to its location
        GLuint startLocation = 20;
        glUniform3f(startLocation++, ambient.r, ambient.g, ambient.b);
        glUniform3f(startLocation++, diffuse.r, diffuse.g, diffuse.b);
        glUniform3f(startLocation++, specular.r, specular.g, specular.b);
        glUniform1f(startLocation++, shininess);
        glUniform1f(startLocation++, shininessStrength);

        if (diffuseTexture < std::numeric_limits<GLuint>().max()) {
            glBindTextureUnit(0, diffuseTexture);
        }
    }

    glm::vec3 ambient = glm::vec3(0.1f); // first slot
    glm::vec3 diffuse = glm::vec3(1.0f); // second slot
    glm::vec3 specular = glm::vec3(0.0f); // third slot
    float shininess = 32.0f; // fourth slot
    float shininessStrength = 1.0f; // fifth slot
    
    // there can be multiple ambient/diffuse/specular textures at once
    // and they are combined with a blend factor
    // we ignore that for now and use one single texture
    GLuint diffuseTexture = std::numeric_limits<GLuint>().max();
};