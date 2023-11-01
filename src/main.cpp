#include <glad/gl.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>
#include <iostream>
#include "utils.hpp"
#include "shaders.hpp"

void do_stuff() {

    // direct NDC (normalized device coordinates)
    // from -1.0f to 1.0f in both x and y, depth set to 0
    std::vector<GLfloat> vertices = {
        -.5f, -.5f, .0f,
        .5f, -.5f, .0f,
        .0f, .5f, .0f
    };

    // glGen...() methods are now mostly replaced by glCreate...()
    // we will be using something called direct state access
    // outdated variant:
    // GLuint vbo; // simply a pointer
    // glGenBuffers(9, &vbo); // "create" buffer
    // glBindBuffer(GL_ARRAY_BUFFER, vbo); // bind to current
    // GLsizeiptr nBytes = vertices.size() * sizeof(GLfloat);
    // glBufferData(GL_ARRAY_BUFFER, nBytes, vertices.data(), GL_STATIC_DRAW); // write to buffer


    GLuint vao; // create our vertex array object
    glCreateVertexArrays(1, &vao);

    // OpenGL buffers will be our handle for GPU memory
    // we cannot interact with it directly like with cpu/system memory
    GLuint vbo; // simply a pointer/ID to our vertex buffer object
    glCreateBuffers(1, &vbo); // modern OpenGL (4.5+)
    GLsizeiptr nBytes = vertices.size() * sizeof(GLfloat);
    glNamedBufferStorage(vbo, nBytes, vertices.data(), 0);

    // we bind the vertex buffer and tell OpenGL how to interpret its data
    // this info is stored inside the vao
    glBindVertexArray(vao);

    GLuint i = 0; // location of "pos" in vertex shader
    glEnableVertexArrayAttrib(vao, i);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, 3 * sizeof(GLfloat));
    glVertexArrayAttribFormat(vao, i, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, i, 0);

    // creating our shader stages
    // very cursed for now
    const GLchar* vertexShaderString = 
        "#version 460 core\n"
        "layout (location = 0) in vec3 pos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
        "}\0";

    // and now we compile it at runtime
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderString, nullptr);
    glCompileShader(vertexShader);

    // check results
    GLint success;
    std::vector<GLchar> infoLog(512);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, infoLog.size(), nullptr, infoLog.data());
        std::cout << infoLog.data() << "\n";
    }

    // now we do the same for pixel/fragment shader
    const GLchar* fragmentShaderString = 
        "#version 460 core\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "   color = vec4(0.7f, 0.1f, 0.1f, 1.0f);\n"
        "}\0";
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderString, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, infoLog.size(), nullptr, infoLog.data());
        std::cout << infoLog.data() << "\n";
    }


    // to combine all shader stages, we create a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, infoLog.size(), nullptr, infoLog.data());
        std::cout << infoLog.data() << "\n";
    }
    glDeleteShader(vertexShader); // can safely delete after linking
    glDeleteShader(fragmentShader); // can safely delete after linking

    // now that everything is ready and working, use this shader program henceforth
    glViewport(0, 0, 500, 500); // set viewport size
    glClearColor(.5f, .5f, .5f, 1.0f); // default screen color
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    
    // clean up
    std::vector<GLuint> buffers = { vao, vbo };
    glDeleteBuffers(buffers.size(), buffers.data());
    glDeleteProgram(shaderProgram);
}

void do_stuff_indexed() {
    // some setup for the render pipeline
    glViewport(0, 0, 500, 500); // set viewport size
    glClearColor(.5f, .5f, .5f, 1.0f); // default screen color
    glClear(GL_COLOR_BUFFER_BIT);

    // direct NDC (normalized device coordinates)
    // from -1.0f to 1.0f in both x and y, depth set to 0
    std::vector<GLfloat> vertices = {
        .0f, .0f, .0f,
        .5f, .0f, .0f,
        .0f, .5f, .0f,
        .5f, .5f, .0f,
    };
    std::vector<GLuint> indices = {
        0, 2, 1,
        1, 2, 3
    };

    // OpenGL buffers will be our handle for GPU memory
    // we cannot interact with it directly like with cpu/system memory
    GLuint vbo; // simply a pointer/ID to our vertex buffer object
    glCreateBuffers(1, &vbo); // modern OpenGL (4.5+)
    GLsizeiptr nBytes = vertices.size() * sizeof(GLfloat);
    glNamedBufferStorage(vbo, nBytes, vertices.data(), 0);

    GLuint ebo; // element buffer object (or: index buffer object)
    glCreateBuffers(1, &ebo);
    nBytes = indices.size() * sizeof(GLuint);
    glNamedBufferStorage(ebo, nBytes, indices.data(), 0);

    // we need to tell OpenGL how to interpret vertex data
    // this info is stored inside the vao
    GLuint vao; // create our vertex array object
    glCreateVertexArrays(1, &vao);
    GLuint i = 0; // location of "pos" in vertex shader
    // specify buffers
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, 3 * sizeof(GLfloat));
    glVertexArrayElementBuffer(vao, ebo);
    // specify vertex format
    glVertexArrayAttribFormat(vao, i, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, i, 0);
    // enable the "pos" attribute
    glEnableVertexArrayAttrib(vao, i);

    // compile shader at runtime
    const GLchar* vertexShaderString = read_shader("default.vs");
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderString, nullptr);
    glCompileShader(vertexShader);

    // check results
    GLint success;
    std::vector<GLchar> infoLog(512);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, infoLog.size(), nullptr, infoLog.data());
        std::cout << infoLog.data() << "\n";
    }

    // now we do the same for pixel/fragment shader
    const GLchar* fragmentShaderString = read_shader("default.fs");
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderString, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, infoLog.size(), nullptr, infoLog.data());
        std::cout << infoLog.data() << "\n";
    }


    // to combine all shader stages, we create a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, infoLog.size(), nullptr, infoLog.data());
        std::cout << infoLog.data() << "\n";
    }
    glDeleteShader(vertexShader); // can safely delete after linking
    glDeleteShader(fragmentShader); // can safely delete after linking

    // now that everything is ready and working, use this shader program henceforth
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

    
    // clean up
    std::vector<GLuint> buffers = { vao, vbo, ebo };
    glDeleteBuffers(buffers.size(), buffers.data());
    glDeleteProgram(shaderProgram);
}

int main() {
    if (SDL_InitSubSystem(SDL_InitFlags::SDL_INIT_VIDEO)) {
        SDL_Log(SDL_GetError(), "");
        SDL_Quit(); // clean up on failure
    }

    // set opengl version
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_PROFILE_MASK, SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MAJOR_VERSION, 4); // adjust
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MINOR_VERSION, 6); // adjust
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_DOUBLEBUFFER, 1);
    // SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_FLAGS, SDL_GLcontextFlag::SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // potentially required on MacOS

    #ifndef NDEBUG
    // only enable explicit logging in debug mode
    SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_FLAGS, SDL_GLcontextFlag::SDL_GL_CONTEXT_DEBUG_FLAG);
    #endif

    // create OpenGL window
    int width = 500;
    int height = 500;
    SDL_Window* pWindow = SDL_CreateWindow("OpenGL Renderer", width, height, SDL_WindowFlags::SDL_WINDOW_OPENGL);
    if (pWindow == nullptr) {
        std::cout << SDL_GetError();
        SDL_Quit();
        return 1;
    }

    // create opengl context
    SDL_GLContext context = SDL_GL_CreateContext(pWindow);
    if (context == nullptr) {
        std::cout << SDL_GetError();
        SDL_Quit();
        return 1;
    }

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

    bool bRunning = true;
    while(bRunning) {

        // handle all the window events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EventType::SDL_EVENT_QUIT) bRunning = false;
            if (event.type == SDL_EventType::SDL_EVENT_KEY_DOWN) {
                switch (event.key.keysym.sym) {
                    case SDL_KeyCode::SDLK_f:
                        std::cout << "f\n";
                    break;
                }
            }
        }
        
        // for debugging in wireframe mode:
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        do_stuff_indexed();
        SDL_GL_SwapWindow(pWindow);
    }

    SDL_Quit();
    return 0;
}