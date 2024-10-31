#pragma once
#include <glbinding/gl46core/gl.h>
using namespace gl46core;

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Transform {
    void bind() {
        // initialize to identity matrix
        glm::mat4x4 transform_matrix(1.0);
        // calculate transform/model matrix from transform components
        transform_matrix = glm::translate(transform_matrix, position);
        transform_matrix = glm::rotate(transform_matrix, rotation.x, glm::vec3(1, 0, 0));
        transform_matrix = glm::rotate(transform_matrix, rotation.y, glm::vec3(0, 1, 0));
        transform_matrix = glm::rotate(transform_matrix, rotation.z, glm::vec3(0, 0, 1));
        transform_matrix = glm::scale(transform_matrix, scale);

        glUniformMatrix4fv(0, 1, false, glm::value_ptr(transform_matrix));
    }

    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 rotation = glm::vec3(0, 0, 0); // euler angles
    glm::vec3 scale = glm::vec3(1, 1, 1);
};