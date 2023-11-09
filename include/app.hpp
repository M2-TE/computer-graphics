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
#include "timer.hpp"
#include "mesh.hpp"
#include "camera.hpp"

struct App {
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

            // clear screen, bind render pipeline and draw mesh to it
            glClear(GL_COLOR_BUFFER_BIT);
            pipeline.bind();
            camera.bind();
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

        // capture mouse for better camera controls
        if (input.get_key_pressed(SDL_KeyCode::SDLK_ESCAPE)) SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());

        // camera movement
        float movementSpeed = timer.get_delta();
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
    Window window = Window(1280, 720);
    Pipeline pipeline = Pipeline("shaders/default.vs", "shaders/default.fs");
    Camera camera = Camera(70, window.width, window.height, 0.1f, 10.0f);
    Mesh mesh;
    bool bRunning = true;
};