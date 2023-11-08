#pragma once

#include <glm/gtc/matrix_transform.hpp> // https://glm.g-truc.net/0.9.2/api/a00245.html
#include <glm/gtx/euler_angles.hpp> // https://glm.g-truc.net/0.9.1/api/a00251.html
#include <glm/gtc/type_ptr.hpp> // allows use of glm::value_ptr to get raw pointer to data

struct Transform {
    Transform() {
        position = { 0.0f, 0.0f, 0.0f };
        rotation = { 0.0f, 0.0f, 0.0f };
        scale = { 1.0f, 1.0f, 1.0f };
    }

    void bind() {
        // apply scale first
        glm::mat4x4 modelMatrix(1.0f); // set matrix to identity

        // another way to apply rotation
        // modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        // modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        // modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= glm::yawPitchRoll(rotation.x, rotation.y, rotation.z);
        modelMatrix = glm::scale(modelMatrix, scale);

        glUniformMatrix4fv(0, 1, false, glm::value_ptr(modelMatrix));
    }

    glm::vec3 position;
    glm::vec3 rotation; // euler rotation
    glm::vec3 scale;
};