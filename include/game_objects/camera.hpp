#pragma once

#include <glm/gtc/matrix_transform.hpp> // https://glm.g-truc.net/0.9.2/api/a00245.html
#include <glm/gtx/euler_angles.hpp> // https://glm.g-truc.net/0.9.1/api/a00251.html
#include <glm/gtc/type_ptr.hpp> // allows use of glm::value_ptr to get raw pointer to data

struct Camera {
    Camera(float fov, float width, float height, float near, float far) {
        projectionMatrix = glm::perspectiveFov(fov, width, height, near, far);
        // projectionMatrix = glm::perspective(fov, width / height, 0.1f, 10.0f);
    }

    // translate relative to camera direction
    void translate(float x, float y, float z) {
        position += glm::quat(rotation) * glm::vec3(x, y, z);
    }

    void bind() {
        glm::mat4x4 viewMatrix(1.0f);
        
        // different order: first translate, then rotate!
        // order of rotation matters here (first y, then x)
        // final order of operations is reversed because of column-major matrices
        viewMatrix = glm::rotate(viewMatrix, -rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        viewMatrix = glm::rotate(viewMatrix, -rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        viewMatrix = glm::translate(viewMatrix, -position);

        glUniformMatrix4fv(4, 1, false, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(8, 1, false, glm::value_ptr(projectionMatrix));
        glUniform3f(16, position.x, position.y, position.z);
    }

    glm::mat4x4 projectionMatrix;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f); // euler rotation
};