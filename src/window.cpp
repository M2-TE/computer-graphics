#include <glbinding/gl46core/gl.h>
#include <glbinding/glbinding.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>
#include <cassert>
#include "window.hpp"

Window::Window(int window_width, int window_height, int nSamples) : width(window_width), height(window_height) {
    if (SDL_InitSubSystem(SDL_InitFlags::SDL_INIT_VIDEO)) std::cout << SDL_GetError();

    // set opengl version
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_PROFILE_MASK, SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MAJOR_VERSION, 4); // adjust
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MINOR_VERSION, 6); // adjust
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_DOUBLEBUFFER, 1);
    if (nSamples > 1) {
        assert((nSamples & (nSamples - 1)) == 0); // check if nSamples is power of two
        SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_MULTISAMPLEBUFFERS, 1); // enable multisampling
        SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_MULTISAMPLESAMPLES, nSamples); // set number of samples per pixel
    }

    #ifndef NDEBUG
    // only enable explicit logging in debug mode
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_FLAGS, SDL_GLcontextFlag::SDL_GL_CONTEXT_DEBUG_FLAG);
    #endif

    // create OpenGL window
    // SDL_GL_MULTISAMPLEBUFFERS
    pWindow = SDL_CreateWindow("OpenGL Renderer", width, height, SDL_WindowFlags::SDL_WINDOW_OPENGL);
    if (pWindow == nullptr) std::cout << SDL_GetError();

    // create opengl context
    context = SDL_GL_CreateContext(pWindow);
    if (context == nullptr) std::cout << SDL_GetError();

    // set up glbinding loader (lazy loading)
    glbinding::initialize(SDL_GL_GetProcAddress, false);

    #ifndef NDEBUG
    // only enable explicit logging in debug mode
    // glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // glDebugMessageCallback(openglCallbackFunction, nullptr);
    // glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
    #endif

    glViewport(0, 0, width, height); // set viewport size
    glClearColor(.5f, .5f, .5f, 1.0f); // default screen color
    glEnable(GL_CULL_FACE); // cull backfaces
    glEnable(GL_DEPTH_TEST); // enable depth buffer and depth testing
    SDL_SetRelativeMouseMode(SDL_TRUE); // capture mouse for better camera controls
    SDL_GL_SetSwapInterval(1); // vsync

    if (nSamples > 1) {
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }

    // color blending via fragment shader's alpha value
    // glEnable(GL_BLEND); // color blending for proper transparency
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // define function for blending colors
}
Window::~Window() {
    SDL_Quit();
}

void Window::swap() {
    SDL_GL_SwapWindow(pWindow);
}