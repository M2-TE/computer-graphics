#pragma once

struct Window {
    Window(int window_width, int window_height) : width(window_width), height(window_height) {
        if (SDL_InitSubSystem(SDL_InitFlags::SDL_INIT_VIDEO)) std::cout << SDL_GetError();

        // set opengl version
        SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_PROFILE_MASK, SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MAJOR_VERSION, 4); // adjust
        SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MINOR_VERSION, 6); // adjust
        SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_DOUBLEBUFFER, 1);

        #ifndef NDEBUG
        // only enable explicit logging in debug mode
        SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_FLAGS, SDL_GLcontextFlag::SDL_GL_CONTEXT_DEBUG_FLAG);
        #endif

        // create OpenGL window
        pWindow = SDL_CreateWindow("OpenGL Renderer", width, height, SDL_WindowFlags::SDL_WINDOW_OPENGL);
        if (pWindow == nullptr) std::cout << SDL_GetError();

        // create opengl context
        context = SDL_GL_CreateContext(pWindow);
        if (context == nullptr) std::cout << SDL_GetError();

        // set up GLAD loader
        int version = gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);
        std::cout << "OpenGL core " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << "\n";

        #ifndef NDEBUG
        // only enable explicit logging in debug mode
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
        #endif

        glViewport(0, 0, width, height); // set viewport size
        glClearColor(.5f, .5f, .5f, 1.0f); // default screen color
    }
    ~Window() {
        SDL_Quit();
    }

    void handle_event(SDL_Event& event) {
        // do whatever with the event
    }

    SDL_Window* pWindow;
    SDL_GLContext context;
    int width;
    int height;
};