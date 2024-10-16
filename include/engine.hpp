#pragma once
#include <vulkan/vulkan.hpp>
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

        // Vulkan: pick physical device, then create logical device from it
        std::vector<vk::PhysicalDevice> physical_devices = _window._instance.enumeratePhysicalDevices();
        _phys_device = physical_devices[0];
        float prio = 1.0f;
        vk::DeviceQueueCreateInfo info_queue {
            .queueFamilyIndex = 0,
            .queueCount = 1,
            .pQueuePriorities = &prio,
        };
        std::vector<const char*> extensions = {
            vk::KHRSwapchainExtensionName
        };
        vk::PhysicalDeviceVulkan11Features vk11_features{
            .pNext = nullptr,
        };
        vk::PhysicalDeviceVulkan12Features vk12_features{
            .pNext = &vk11_features,
        };
        vk::PhysicalDeviceVulkan13Features vk13_features{
            .pNext = &vk12_features,
        };
        vk::PhysicalDeviceFeatures2 features {
            .pNext = &vk13_features,
            .features {
            },
        };
        vk::DeviceCreateInfo info_device {
            .pNext = &features,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &info_queue,
            .enabledExtensionCount = (uint32_t)extensions.size(),
            .ppEnabledExtensionNames = extensions.data(),
        };
        _device = _phys_device.createDevice(info_device);

        // Vulkan: dynamic dispatcher init 3/3
        VULKAN_HPP_DEFAULT_DISPATCHER.init(_device);

        // Vulkan: get the all-rounder queue with family index 0
        _queue = _device.getQueue(0, 0);
    }
    void destroy() {
        _device.destroy();
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

    vk::PhysicalDevice _phys_device;
    vk::Device _device;
    vk::Queue _queue;
    Window _window;
};