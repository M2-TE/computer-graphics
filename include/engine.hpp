#pragma once
#include <SDL3/SDL_init.h>
#include <glm/glm.hpp>
#include <fmt/base.h>
#include "window.hpp"
#include "input.hpp"
#include "mesh.hpp"
#include "pipeline.hpp"
#include "transform.hpp"
#include "texture.hpp"
#include "camera.hpp"

struct Engine {
    void init() {
        _window.init(1280, 720, "OpenGL Renderer");
        _pipeline.init("../shaders/default.vert", "../shaders/default.frag");
        _mesh.init();
        _transform._position.z -= 5.0f;
        _camera.set_perspective(1280, 720, 70);
        _texture.init("../textures/grass.png");
        SDL_SetWindowRelativeMouseMode(_window._window_p, true);
    }
    void destroy() {
        _window.destroy();
        _pipeline.destroy();
        _mesh.destroy();
        _texture.destroy();
    }
    auto execute_event(SDL_Event* event_p) -> SDL_AppResult {
        // let input system process event
        Input::register_event(*event_p);
        switch (event_p->type) {
            case SDL_EventType::SDL_EVENT_QUIT: return SDL_AppResult::SDL_APP_SUCCESS;
            default: break;
        }
        return SDL_AppResult::SDL_APP_CONTINUE;   
    }
    void execute_frame() {
        // clear screen before drawing
        glClearColor(0.1, 0.1, 0.1, 0.0); // theoretically only needs to be set once
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // bind pipeline and draw geometry
        _pipeline.bind();
        _transform._rotation += 0.01f;
        _transform.bind();
        // move camera
        float speed = 0.1f;
        if (Keys::down(SDLK_W)) _camera.translate(0, 0, -speed);
        if (Keys::down(SDLK_S)) _camera.translate(0, 0, +speed);
        if (Keys::down(SDLK_A)) _camera.translate(-speed, 0, 0);
        if (Keys::down(SDLK_D)) _camera.translate(+speed, 0, 0);
        if (Keys::down(SDLK_Q)) _camera.translate(0, -speed, 0);
        if (Keys::down(SDLK_E)) _camera.translate(0, +speed, 0);
        float rotationSpeed = 0.005f;
        _camera._rotation.x -= rotationSpeed * Mouse::delta().second;
        _camera._rotation.y -= rotationSpeed * Mouse::delta().first;
        _camera.bind();
        // bind and draw mesh
        _texture.bind();
        _mesh.draw();
        // present to the screen
        SDL_GL_SwapWindow(_window._window_p);
        Input::flush();
    }

    Window _window;
    Pipeline _pipeline;
    Camera _camera;
    Transform _transform;
    Texture _texture;
    Mesh _mesh;
};