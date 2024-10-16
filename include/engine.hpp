#pragma once
#include <SDL3/SDL_init.h>
#include <fmt/base.h>
#include "window.hpp"

struct Engine {
    void init() {
        _window.init(1280, 720, "OpenGL Renderer");

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
        // compile vertex shader
        const GLchar* vs_string = "\
            #version 460 core\n\
            layout (location = 0) in vec4 in_pos;\n\
            void main() {\n\
                gl_Position = in_pos;\n\
            }";
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vs_string, nullptr);
        glCompileShader(vertex_shader);
        // check results
        GLint success;
        std::vector<GLchar> info_log(512);
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex_shader, info_log.size(), nullptr, info_log.data());
            fmt::print("{}", info_log.data());
        }

        // now we do the same for pixel/fragment shader
        const GLchar* fs_string = "\
            #version 460 core\n\
            out vec4 out_color;\n\
            void main() {\n\
                out_color = vec4(0.0, 1.0, 0.0, 1.0);\n\
            }";
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fs_string, nullptr);
        glCompileShader(fragment_shader);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment_shader, info_log.size(), nullptr, info_log.data());
            fmt::print("{}", info_log.data());
        }

        // to combine all shader stages, we create a shader program
        GLuint shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);
        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader_program, info_log.size(), nullptr, info_log.data());
            fmt::print("{}", info_log.data());
        }
        // clean up shaders after they were compiled and linked
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        struct Vertex {
            float x;
            float y;
            float z;
            float w;
        };
        std::vector<Vertex> vertices = {
            { -.5, -.5, 0, 1 },
            { .5, -.5, 0, 1 },
            { 0, .5, 0, 1 },
        };
        // describe vertex buffer
        GLsizeiptr byte_count = vertices.size() * sizeof(Vertex);
        GLuint vertex_buffer_object;
        glCreateBuffers(1, &vertex_buffer_object);
        glNamedBufferStorage(vertex_buffer_object, byte_count, vertices.data(), BufferStorageMask::GL_NONE_BIT);
        // create vertex array buffer
        GLuint vertex_array_object;
        glCreateVertexArrays(1, &vertex_array_object);
        glVertexArrayVertexBuffer(vertex_array_object, 0, vertex_buffer_object, 0, sizeof(Vertex));
        // specify vertex format with vertex array buffer
        glVertexArrayAttribFormat(vertex_array_object, 0, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vertex_array_object, 0, 0);
        glEnableVertexArrayAttrib(vertex_array_object, 0);

        // draw
        glClearColor(0.1, 0.1, 0.1, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);
        glBindVertexArray(vertex_array_object);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
        SDL_GL_SwapWindow(_window._window_p);
    }

    Window _window;
};