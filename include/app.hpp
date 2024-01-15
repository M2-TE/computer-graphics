#pragma once
// External libraries
#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>
#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
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
#include "game_objects/lights/light_point.hpp"
#include "game_objects/camera.hpp"

struct App {
    App() {
        // create frame buffer for shadow mapping pipeline
        glCreateFramebuffers(1, &shadowPipeline.framebuffer);
        // attach texture to frame buffer (only draw to depth, no color output!)
        glNamedFramebufferReadBuffer(shadowPipeline.framebuffer, GL_NONE);
        glNamedFramebufferDrawBuffer(shadowPipeline.framebuffer, GL_NONE);
    }

    int run() {
        while(bRunning) {
            input.flush(); // flush input from last frame
            timer.update(); // update delta time
            
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EventType::SDL_EVENT_QUIT) bRunning = false;
                ImGui_ImplSDL3_ProcessEvent(&event);
                window.handle_event(event); // handle window resize and such events
                input.handle_event(event); // handle keyboard/mouse events
            }
            handle_inputs();
            imgui_begin();
            draw();
            draw_ui();
            imgui_end();

            // present drawn frame to the screen
            window.swap();
        }

        return 0;
    }

private:
    void imgui_begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }
    void imgui_end() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    void draw_ui() {
        // ImGui::ShowDemoWindow();
        ImGui::Begin("FPS_Overlay");
        ImGui::Text("FPS (acc): %.2f", timer.get_fps());
        ImGui::Text("FPS (est): %.2f", ImGui::GetIO().Framerate);
        bool bPressed = ImGui::Button("testbutton", { 200, 200});
        ImGui::End();
    }
    void draw() {
        // first pass: render shadow map
        light.adjust_viewport();
        glBindFramebuffer(GL_FRAMEBUFFER, shadowPipeline.framebuffer);
        shadowPipeline.bind();
        // render each cubemap face (omnidirectional shadows)
        for (int face = 0; face < 6; face++) {
            // set framebuffer texture and clear it
            glNamedFramebufferTextureLayer(shadowPipeline.framebuffer, GL_DEPTH_ATTACHMENT, light.shadowCubemap, 0, face);
            glClear(GL_DEPTH_BUFFER_BIT);
            // bind resources to pipeline
            light.bind_write(face);
            // draw models
            lightB.draw();
            model.draw();
        }
            for (int face = 0; face < 6; face++) {
            // set framebuffer texture and clear it
            glNamedFramebufferTextureLayer(shadowPipeline.framebuffer, GL_DEPTH_ATTACHMENT, lightB.shadowCubemap, 0, face);
            glClear(GL_DEPTH_BUFFER_BIT);
            // bind resources to pipeline
            lightB.bind_write(face);
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
        camera.bind();
        light.bind_read(0, 1);
        lightB.bind_read(1, 2);
        // draw models
        light.draw();
        lightB.draw();
        model.draw();
    }
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
    bool bRunning = true;
    // render resources
    Pipeline colorPipeline = Pipeline("shaders/default.vs", "shaders/default.fs");
    Pipeline shadowPipeline = Pipeline("shaders/shadowmapping.vs", "shaders/shadowmapping.fs");
    Model model = Model({0, 0, 0}, {0, 0, 0}, {.01, .01, .01}, "models/sponza/sponza.obj");
    Camera camera = Camera({1, 2, 1}, {0, 0, 0}, window.width, window.height);
    PointLight light = PointLight({1, 1, 0}, {0, 0, 0}, {1, 1, 1}, 30.0f);
    PointLight lightB = PointLight({4, 1, 0}, {0, 0, 0}, {1, 1, 1}, 30.0f);
};