#pragma once

#include <glm/gtc/matrix_transform.hpp> // https://glm.g-truc.net/0.9.2/api/a00245.html
#include <glm/gtx/euler_angles.hpp> // https://glm.g-truc.net/0.9.1/api/a00251.html
#include <glm/gtc/type_ptr.hpp> // allows use of glm::value_ptr to get raw pointer to data

struct Transform {
    void bind() {
        glm::mat4x4 modelMatrix(1.0f); // set matrix to identity

        // calculate model matrix
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= glm::yawPitchRoll(rotation.x, rotation.y, rotation.z);
        modelMatrix = glm::scale(modelMatrix, scale);

        // calculate normal matrix
        glm::mat4x4 normalMatrix(1.0f);
        normalMatrix = glm::transpose(glm::inverse(modelMatrix));

        glUniformMatrix4fv(0, 1, false, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(12, 1, false, glm::value_ptr(normalMatrix));
    }

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f); // euler
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
};