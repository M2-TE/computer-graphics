#pragma once

#include <glm/gtc/matrix_transform.hpp> // https://glm.g-truc.net/0.9.2/api/a00245.html
#include <glm/gtx/euler_angles.hpp> // https://glm.g-truc.net/0.9.1/api/a00251.html
#include <glm/gtc/type_ptr.hpp> // allows use of glm::value_ptr to get raw pointer to data
#include <glm/gtc/quaternion.hpp> // https://glm.g-truc.net/0.9.0/api/a00135.html

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
        // using quaternions to prevent gimbal lock https://en.wikipedia.org/wiki/Gimbal_lock
        viewMatrix = glm::mat4_cast(glm::inverse(glm::quat(rotation)));
        viewMatrix = glm::translate(viewMatrix, -position);
        glUniformMatrix4fv(4, 1, false, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(8, 1, false, glm::value_ptr(projectionMatrix));
        glUniform3f(16, position.x, position.y, position.z);
    }

    glm::mat4x4 projectionMatrix;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f); // euler
};