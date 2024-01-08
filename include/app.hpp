#pragma once
// External libraries
#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>
#include <SDL.h>
// C++ standard library
#include <vector>
#include <iostream>
// Project-local headers
using namespace gl;
#include "utils.hpp"
#include "window.hpp"
#include "pipeline.hpp"
#include "input.hpp"
#include "timer.hpp"
#include "game_objects/model.hpp"
#include "game_objects/light.hpp"
#include "game_objects/camera.hpp"

struct App {
    App() {
        // create shadow cubemap
        GLuint& depthMap = shadowPipeline.framebufferTexture;
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &depthMap);
        glTextureStorage2D(depthMap, 1, GL_DEPTH_COMPONENT32F, shadowWidth, shadowHeight);
        // set wrapping/magnification behavior
        glTextureParameteri(depthMap, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(depthMap, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(depthMap, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTextureParameteri(depthMap, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(depthMap, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // create frame buffer for shadow mapping pipeline
        glCreateFramebuffers(1, &shadowPipeline.framebuffer);
        // attach texture to frame buffer (only draw to depth, no color output!)
        glNamedFramebufferReadBuffer(shadowPipeline.framebuffer, GL_NONE);
        glNamedFramebufferDrawBuffer(shadowPipeline.framebuffer, GL_NONE);
        
        // prepare shadow cameras
        shadowProjection = glm::perspectiveFov(glm::radians(90.0f), (float)shadowWidth, (float)shadowHeight, 1.0f, lightFar);
        shadowViews[0] = glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // right
        shadowViews[1] = glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // left
        shadowViews[2] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // top
        shadowViews[3] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)); // bottom
        shadowViews[4] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // back
        shadowViews[5] = glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // front
    }

    int run() {
        while(bRunning) {
            input.flush(); // flush input from last frame
            timer.update(); // update delta time
            // std::cout << timer.get_fps() << "fps\n";
            
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EventType::SDL_EVENT_QUIT) bRunning = false;
                window.handle_event(event); // handle window resize and such events
                input.handle_event(event); // handle keyboard/mouse events
            }
            handle_inputs();

            // first pass: render shadow map
            glViewport(0, 0, shadowWidth, shadowHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, shadowPipeline.framebuffer);
            shadowPipeline.bind();
            // render each cubemap face (omnidirectional shadows)
            for (int face = 0; face < 6; face++) {
                glNamedFramebufferTextureLayer(shadowPipeline.framebuffer, GL_DEPTH_ATTACHMENT, shadowPipeline.framebufferTexture, 0, face);
                glClear(GL_DEPTH_BUFFER_BIT);
                // bind resources to pipeline
                glUniformMatrix4fv(4, 1, false, glm::value_ptr(shadowViews[face]));
                glUniformMatrix4fv(8, 1, false, glm::value_ptr(shadowProjection));
                glUniform3f(17, lightPos.x, lightPos.y, lightPos.z);
                glUniform1f(35, lightFar);
                // draw models
                light.draw();
                model.draw();
            }

            // second pass: render color map
            glViewport(0, 0, window.width, window.height);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            colorPipeline.bind();
            // bind resources to pipeline
            glUniform3f(17, lightPos.x, lightPos.y, lightPos.z);
            glUniform1f(35, lightFar);
            glBindTextureUnit(1, shadowPipeline.framebufferTexture);
            camera.bind();
            light.bind();
            // draw models
            light.draw();
            model.draw();
            
            // present drawn frame to the screen
            window.swap();
        }

        return 0;
    }

private:
    void handle_inputs() {
        // draw wireframe while holding f
        if (input.get_key_down(SDL_KeyCode::SDLK_f)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // capture mouse for better camera controls
        if (input.get_key_pressed(SDL_KeyCode::SDLK_ESCAPE)) SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());

        // camera movement
        float movementSpeed = timer.get_delta() * 2.0f; // scale speed relative to framerate
        if (input.get_key_down(SDL_KeyCode::SDLK_LSHIFT)) movementSpeed *= 3.0f; // sprint button
        if (input.get_key_down(SDL_KeyCode::SDLK_s)) camera.translate(0.0f, 0.0f, movementSpeed);
        if (input.get_key_down(SDL_KeyCode::SDLK_w)) camera.translate(0.0f, 0.0f, -movementSpeed);
        if (input.get_key_down(SDL_KeyCode::SDLK_e)) camera.translate(0.0f, movementSpeed, 0.0f);
        if (input.get_key_down(SDL_KeyCode::SDLK_q)) camera.translate(0.0f, -movementSpeed, 0.0f);
        if (input.get_key_down(SDL_KeyCode::SDLK_d)) camera.translate(movementSpeed, 0.0f, 0.0f);
        if (input.get_key_down(SDL_KeyCode::SDLK_a)) camera.translate(-movementSpeed, 0.0f, 0.0f);

        // camera rotation
        float rotationSpeed = 0.001f;
        camera.rotation.x -= rotationSpeed * input.get_mouse_delta().second;
        camera.rotation.y -= rotationSpeed * input.get_mouse_delta().first;
    }

private:
    Input input;
    Timer timer;
    Window window = Window(1280, 720, 4);
    Pipeline colorPipeline = Pipeline("shaders/default.vs", "shaders/default.fs");
    Pipeline shadowPipeline = Pipeline("shaders/shadowmapping.vs", "shaders/shadowmapping.fs");
    glm::vec3 lightPos = {1, 1, 0};
    Camera camera = Camera(lightPos, {0, 0, 0}, window.width, window.height);
    Model model = Model({0, 0, 0}, {0, 0, 0}, {.01, .01, .01}, "models/sponza/sponza.obj");
    Light light = Light(lightPos, {0, 0, 0}, {1, 1, 1});
    bool bRunning = true;
    // temporary:
    int shadowWidth = 512;
    int shadowHeight = 512;
    float lightFar = 20.0f;
    GLuint shadowCubemap;
    std::array<glm::mat4x4, 6> shadowViews;
    glm::mat4x4 shadowProjection;
};