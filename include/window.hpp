#pragma once
#include <vector>
#include <string>
#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <fmt/base.h>

struct Window {
    void init(int width, int height, std::string name) {
        // SDL: init the SDL video subsystem before anything else
        bool res = SDL_InitSubSystem(SDL_INIT_VIDEO);
        if (!res) fmt::println("{}", SDL_GetError());
        
        // SDL: create window with vulkan support
        _window_p = SDL_CreateWindow(name.c_str(), width, height, SDL_WINDOW_VULKAN);
        if (_window_p == nullptr) fmt::println("{}", SDL_GetError());

        // SDL: let SDL tell us which vulkan instance extensions we need for the window
        uint32_t extensions_n;
        const char* const* extensions_p = SDL_Vulkan_GetInstanceExtensions(&extensions_n);
        if (extensions_p == nullptr) fmt::println("{}", SDL_GetError());

        // Vulkan: add validation layer for helpful debug output (warnings, error, etc)
        std::vector<const char*> layers;
        layers.push_back("VK_LAYER_KHRONOS_validation");

        // Vulkan: create vulkan instance and supply it with info about the application
        vk::ApplicationInfo info_app {
            .pApplicationName = name.c_str(),
            .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
            .pEngineName = "TBD",
            .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
            .apiVersion = vk::ApiVersion13
        };
        _instance = vk::createInstance({
            .pApplicationInfo = &info_app,
            .enabledLayerCount = (uint32_t)layers.size(),
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = extensions_n,
            .ppEnabledExtensionNames = extensions_p,
        });

        // SDL + Vulkan: create a render surface from the already created window
        res = SDL_Vulkan_CreateSurface(_window_p, _instance, nullptr, (VkSurfaceKHR*)&_surface);
    }
    void destroy() {
        SDL_Vulkan_DestroySurface((VkInstance)_instance, (VkSurfaceKHR)_surface, nullptr);
        _instance.destroy();
        SDL_Quit();
    }

    SDL_Window* _window_p;
    vk::Instance _instance;
    vk::SurfaceKHR _surface;
};