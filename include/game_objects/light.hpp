#pragma once
#include "game_objects/model.hpp"

struct Light {
    Light(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) 
    : transform(pos, rot, scale) {
        mesh.load_sphere(12, 12, true);
        // mesh.load_cube(true);
    }

    // bind light properties
    void bind() {
        glUniform3f(17, transform.position.x, transform.position.y, transform.position.z);
        glUniform3f(18, lightColor.x, lightColor.y, lightColor.z);
    }
    // draw the object that represents the light
    void draw() {
        transform.bind();
        mesh.draw();
    }

    Transform transform;
    glm::vec3 lightColor = glm::vec3(.992f, .984f, .827f);
private:
    Mesh mesh;
};