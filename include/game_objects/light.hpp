#pragma once
#include "game_objects/model.hpp"

struct Light {
    Light(float x, float y, float z) {
        transform.position = {x, y, z};
        transform.scale *= 1.0f;
    }

    void bind() {
        glUniform3f(17, transform.position.x, transform.position.y, transform.position.z);
        glUniform3f(18, lightColor.x, lightColor.y, lightColor.z);
    }
    void draw() {
        glUniform1ui(19, 1); // isGlowing
        model.draw();
        glUniform1ui(19, 0);
    }

private:
    // https://www.turbosquid.com/
    Model model = Model("models/duck.glb");
public:
    Transform& transform = model.transform;
    glm::vec3 lightColor = glm::vec3(.992f, .984f, .827f);
};