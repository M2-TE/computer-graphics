#pragma once
#include <SDL3/SDL_init.h>
#include <fmt/base.h>
#include "window.hpp"

struct Engine {
    void init() {
        // Vulkan: dynamic dispatcher init 1/3
        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        // SDL: create window and query required instance extensions
        _window.init(1280, 720, "Vulkan Renderer");

        // Vulkan: dynamic dispatcher init 2/3
        VULKAN_HPP_DEFAULT_DISPATCHER.init(_window._instance);

    }
    void destroy() {
        _window.destroy();
    }
    auto execute_event(SDL_Event* event_p) -> SDL_AppResult { 
        switch (event_p->type) {
            case SDL_EventType::SDL_EVENT_QUIT: return SDL_AppResult::SDL_APP_SUCCESS;
            default: break;
        }
        return SDL_AppResult::SDL_APP_CONTINUE;   
    }
    void execute_frame() {
    }

    Window _window;
};