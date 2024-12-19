#pragma once
#include <array>
#include <glbinding/gl46core/gl.h>
using namespace gl46core;
#include <SDL3/SDL_init.h>
#include <glm/glm.hpp>
#include <fmt/base.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include "time.hpp"
#include "window.hpp"
#include "input.hpp"
#include "pipeline.hpp"
#include "entities/camera.hpp"
#include "entities/model.hpp"
#include "entities/light.hpp"

struct Engine {
    void init() {
        Time::init();
        _window.init(1280, 720, "OpenGL Renderer", 4);
        _camera.set_perspective(1280, 720, 70);

        // create pipeline for textured objects
        _pipeline.init("../shaders/default.vert", "../shaders/default.frag");
        _pipeline_shadows.init("../shaders/shadows.vert", "../shaders/shadows.frag");
        _pipeline_shadows.create_framebuffer();

        // create light and its shadow map
        _lights[0].init({+1.0, +3.0, -0.5}, {.992, .984, .827}, 100);
        _lights[1].init({+3.0, +1.5, +4.0}, {.992, .984, .827}, 100);

        // create renderable models
        _models.emplace_back().init(Mesh::eCube, "../textures/grass.png");
        _models.back()._transform._position = glm::vec3(-3, 0, -5);
        _models.emplace_back().init(Mesh::eSphere);
        _models.back()._transform._position = glm::vec3(+3, 0, -5);
        // load the entire "sponza" scene
        _models.emplace_back().init("../models/sponza/sponza.obj");
        _models.back()._transform._scale = glm::vec3(.01, .01, .01);
        // create spheres to represent the lights
        for (auto& light: _lights) {
            _models.emplace_back().init(Mesh::eSphere);
            _models.back()._transform._position = light._position;
        }

        // initialize ImGui for UI rendering
        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForOpenGL(_window._window_p, _window._context);
        ImGui_ImplOpenGL3_Init();
    }
    void destroy() {
        // free OpenGL resources
        for (auto& light: _lights) light.destroy();
        for (auto& model: _models) model.destroy();
        _pipeline.destroy();
        _window.destroy();
        
        // shut down imgui backend
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }
    auto execute_event(SDL_Event* event_p) -> SDL_AppResult {
        // pass event over to imgui as well
        ImGui_ImplSDL3_ProcessEvent(event_p);
        // let input system process event
        Input::register_event(*event_p);
        switch (event_p->type) {
            case SDL_EventType::SDL_EVENT_QUIT: return SDL_AppResult::SDL_APP_SUCCESS;
            default: break;
        }
        return SDL_AppResult::SDL_APP_CONTINUE;   
    }
    void execute_input() {
        // camera movement
        float speed = 0.1f;
        if (Keys::down(SDLK_W)) _camera.translate(0, 0, -speed);
        if (Keys::down(SDLK_S)) _camera.translate(0, 0, +speed);
        if (Keys::down(SDLK_A)) _camera.translate(-speed, 0, 0);
        if (Keys::down(SDLK_D)) _camera.translate(+speed, 0, 0);
        if (Keys::down(SDLK_Q)) _camera.translate(0, -speed, 0);
        if (Keys::down(SDLK_E)) _camera.translate(0, +speed, 0);
        // camera rotation
        if (_mouse_captured) {
            float rotationSpeed = 0.005f;
            _camera._rotation.x -= rotationSpeed * Mouse::delta().second;
            _camera._rotation.y -= rotationSpeed * Mouse::delta().first;
        }

        // handle mouse capture
        if (_mouse_captured && Keys::pressed(SDLK_ESCAPE)) {
            _mouse_captured = false;
            SDL_SetWindowRelativeMouseMode(_window._window_p, _mouse_captured);
        }
        if (!_mouse_captured && Mouse::pressed(Mouse::ids::left)) {
            _mouse_captured = true;
            SDL_SetWindowRelativeMouseMode(_window._window_p, _mouse_captured);
        }

        // draw wireframe while holding spacebar
        if (Keys::down(SDLK_SPACE)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    void execute_frame() {
        // update timer for accurate Time::get_delta()
        Time::update();
        // start new imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // draw some ui
        ImGui::Begin("FPS window");
        ImGui::Text("%.1f fps, %.2f ms", ImGui::GetIO().Framerate, Time::get_delta() * 1000.0);
        ImGui::End();

        // handle all the inputs such as camera movement
        execute_input();
        _models[0]._transform._rotation += Time::get_delta();

        // draw shadows
        if (_shadows_dirty) {
            // do this for each light
            for (auto& light: _lights) {
                _pipeline_shadows.bind();
                glViewport(0, 0, light._shadow_width, light._shadow_height);
                // render into each cubemap face
                for (int face = 0; face < 6; face++) {
                    // bind the target shadow map and clear it
                    light.bind_write(_pipeline_shadows._framebuffer, face);
                    glClear(GL_DEPTH_BUFFER_BIT);
                    // draw the stuff
                    for (auto& model: _models) model.draw(false);
                }
            }
            _shadows_dirty = false;
        }

        // draw color
        {
            // bind pipeline
            _pipeline.bind();
            glViewport(0, 0, 1280, 720);
            // clear screen before drawing
            glClearColor(0.1, 0.1, 0.1, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // bind lights and their shadow maps
            for (int i = 0; i < _lights.size(); i++) {
                _lights[i].bind_read(i + 1, i * 3);
            }
            _camera.bind();
            // draw the stuff
            for (auto& model: _models) model.draw(false);
        }

        // present to the screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(_window._window_p);
        Input::flush();
    }

    Window _window;
    Camera _camera;
    Pipeline _pipeline;
    Pipeline _pipeline_shadows;
    std::array<Light, 2> _lights;
    std::vector<Model> _models;
    // other
    bool _shadows_dirty = true;
    bool _mouse_captured = false;
};