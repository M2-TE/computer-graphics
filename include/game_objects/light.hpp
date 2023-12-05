#pragma once
#include "game_objects/model.hpp"

struct Light {
    Light(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) 
    : transform(pos, rot, scale) {
        mesh.generate_cube();
    }

    // bind light properties
    void bind() {
        glUniform3f(17, transform.position.x, transform.position.y, transform.position.z);
        glUniform3f(18, lightColor.x, lightColor.y, lightColor.z);
    }
    // draw the object that represents the light
    void draw() {
        mesh.draw();
    }

    Transform transform;
    glm::vec3 lightColor = glm::vec3(.992f, .984f, .827f);
private:
    Mesh mesh;
};