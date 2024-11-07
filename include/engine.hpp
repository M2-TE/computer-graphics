#pragma once
#include <SDL3/SDL_init.h>
#include <glm/glm.hpp>
#include <fmt/base.h>
#include "window.hpp"
#include "input.hpp"
#include "mesh.hpp"
#include "pipeline.hpp"
#include "transform.hpp"
#include "transform.hpp"

struct Engine {
    void init() {
        _window.init(720, 720, "OpenGL Renderer");
        _pipeline.init();
        _mesh.init();
    }
    void destroy() {
        _window.destroy();
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
        // bind and draw mesh
        _mesh.draw();
        // present to the screen
        SDL_GL_SwapWindow(_window._window_p);
    }

    Window _window;
    Pipeline _pipeline;
    Transform _transform;
    Mesh _mesh;
};