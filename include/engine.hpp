#pragma once
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

struct Engine {
    void init() {
        Time::init();
        _window.init(1280, 720, "OpenGL Renderer");
        _camera.set_perspective(1280, 720, 70);
        // create pipeline for textured objects
        _pipeline.init("../shaders/default.vert", "../shaders/default.frag");
        // create the two cubes
        _cube.init("../textures/grass.png");
        _cube._transform._position = glm::vec3(-3, 0, -5);
        _sphere.init(Mesh::eSphere);
        _sphere._transform._position = glm::vec3(+3, 0, -5);
        // initialize ImGui for UI rendering
        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForOpenGL(_window._window_p, _window._context);
        ImGui_ImplOpenGL3_Init();
    }
    void destroy() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        _pipeline.destroy();
        _window.destroy();
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

        // material slider for the colcube mat
        ImGui::Begin("colcube specular");
        ImGui::SliderFloat("specular", &_sphere._material._specular, 0.0f, 1.0f);
        ImGui::SliderFloat("specular shininess", &_sphere._material._specular_shininess, 1.0f, 256.0f);
        ImGui::End();

        // handle all the inputs such as camera movement
        execute_input();

        // clear screen before drawing
        glClearColor(0.1, 0.1, 0.1, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // bind pipeline and draw geometry
        _pipeline.bind();
        _camera.bind();
        _cube._transform._rotation += Time::get_delta();
        _cube.draw();
        _sphere.draw();
        // present to the screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(_window._window_p);
        Input::flush();
    }

    Window _window;
    Camera _camera;
    Pipeline _pipeline;
    Pipeline _pipeline_vertcol;
    Model _cube;
    Model _sphere;
    // other
    bool _mouse_captured = false;
};