#pragma once

// External libraries
#include <glad/gl.h>
#include <SDL.h>
#include <SDL_opengl.h>
// C++ standard library
#include <vector>
#include <iostream>
// Project-local headers
#include "utils.hpp"
#include "window.hpp"
#include "pipeline.hpp"
#include "input.hpp"
#include "mesh.hpp"

struct App {
    int run() {
        while(bRunning) {
            // flush input from last frame
            input.flush();
            
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EventType::SDL_EVENT_QUIT) bRunning = false;
                window.handle_event(event); // handle window resize and such events
                input.handle_event(event); // handle keyboard/mouse events
            }
            

            handle_inputs();

            // clear screen, bind render pipeline and draw mesh to it
            glClear(GL_COLOR_BUFFER_BIT);
            pipeline.bind();
            mesh.draw();
            SDL_GL_SwapWindow(window.pWindow);
        }

        return 0;
    }

private:
    void handle_inputs() {
        // draw wireframe while holding f
        if (input.get_key_down(SDL_KeyCode::SDLK_f)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        float movementSpeed = 0.01f;
        if (input.get_key_down(SDL_KeyCode::SDLK_w)) mesh.transform.position.y += movementSpeed;
        if (input.get_key_down(SDL_KeyCode::SDLK_s)) mesh.transform.position.y -= movementSpeed;
        if (input.get_key_down(SDL_KeyCode::SDLK_d)) mesh.transform.position.x += movementSpeed;
        if (input.get_key_down(SDL_KeyCode::SDLK_a)) mesh.transform.position.x -= movementSpeed;

        float rotationSpeed = 0.01f;
        if (input.get_key_down(SDL_KeyCode::SDLK_q)) mesh.transform.rotation.z += rotationSpeed;
        if (input.get_key_down(SDL_KeyCode::SDLK_e)) mesh.transform.rotation.z -= rotationSpeed;
    }

private:
    Input input;
    Window window = Window(512, 512);
    Pipeline pipeline = Pipeline("default.vs", "default.fs");
    Mesh mesh;
    bool bRunning = true;
};