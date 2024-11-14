#pragma once
#include <glbinding/gl46core/gl.h>
using namespace gl46core;
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Camera {
    void set_perspective(float width, float height, float fov) {
        _projection_mat = glm::perspectiveFov(fov, width, height, nearPlane, farPlane);
    }
    void set_orthographic() {
        _projection_mat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    }

    // translate relative to camera direction
    void translate(float x, float y, float z) {
        _position += glm::quat(_rotation) * glm::vec3(x, y, z);
    }

    void bind() {
        glm::mat4x4 view_mat(1.0f);
        view_mat = glm::rotate(view_mat, - _rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        view_mat = glm::rotate(view_mat, - _rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        view_mat = glm::translate(view_mat, - _position);
        // upload to GPU
        glUniformMatrix4fv(4, 1, false, glm::value_ptr(view_mat));
        glUniformMatrix4fv(8, 1, false, glm::value_ptr(_projection_mat));
    }

    glm::mat4x4 _projection_mat;
    glm::vec3 _position;
    glm::vec3 _rotation; // euler _rotation
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
};