#pragma once
#include <iostream>
#include <fstream>
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
        // read vertex shader data
        std::ifstream vs_file("shaders/default.vs", std::ios::binary);
        vs_file.seekg(0, std::ios::end);
        std::streamsize vs_size = vs_file.tellg();
        vs_file.seekg(0, std::ios::beg);
        std::vector<GLchar> vs_string(vs_size);
        GLchar* vs_data = vs_string.data();
        vs_file.read(vs_data, vs_size);
        // compile vertex shader
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vs_data, nullptr);
        glCompileShader(vertex_shader);
        // check results
        GLint success;
        std::vector<GLchar> info_log(512);
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex_shader, info_log.size(), nullptr, info_log.data());
            fmt::print("{}", info_log.data());
        }

        // read fragment shader data
        std::ifstream fs_file("shaders/default.fs", std::ios::binary);
        fs_file.seekg(0, std::ios::end);
        std::streamsize fs_size = fs_file.tellg();
        fs_file.seekg(0, std::ios::beg);
        std::vector<GLchar> fs_string(fs_size);
        GLchar* fs_data = fs_string.data();
        fs_file.read(fs_data, fs_size);
        // compile fragment shader
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fs_data, nullptr);
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

        // create vertices
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
        GLsizeiptr vertex_byte_count = vertices.size() * sizeof(Vertex);
        GLuint vertex_buffer_object;
        glCreateBuffers(1, &vertex_buffer_object);
        // upload data to GPU buffer
        glNamedBufferStorage(vertex_buffer_object, vertex_byte_count, vertices.data(), BufferStorageMask::GL_NONE_BIT);

        // create indices
        std::vector<uint32_t> indices = { 0, 1, 2 };
        // describe index buffer (element buffer)
        GLsizeiptr element_byte_count = vertices.size() * sizeof(Vertex);
        GLuint element_buffer_object;
        glCreateBuffers(1, &element_buffer_object);
        // upload data to GPU buffer
        glNamedBufferStorage(element_buffer_object, element_byte_count, indices.data(), BufferStorageMask::GL_NONE_BIT);

        // create vertex array buffer
        GLuint vertex_array_object;
        glCreateVertexArrays(1, &vertex_array_object);
        // assign both vertex and index (element) buffers
        glVertexArrayVertexBuffer(vertex_array_object, 0, vertex_buffer_object, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(vertex_array_object, element_buffer_object);
        // specify vertex format with vertex array buffer
        glVertexArrayAttribFormat(vertex_array_object, 0, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vertex_array_object, 0, 0);
        glEnableVertexArrayAttrib(vertex_array_object, 0);

        // draw
        glClearColor(0.1, 0.1, 0.1, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);

        // bind vertex offset uniform to the currently active graphics pipeline (shader_program)
        glUniform4f(0, 0, 0.5, 0, 0);

        glBindVertexArray(vertex_array_object);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        SDL_GL_SwapWindow(_window._window_p);
    }

    Window _window;
};