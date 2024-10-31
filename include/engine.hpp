#pragma once
#include <iostream>
#include <fstream>
#include <SDL3/SDL_init.h>
#include <glm/glm.hpp>
#include <fmt/base.h>
#include "window.hpp"
#include "input.hpp"
#include "transform.hpp"

struct Engine {
    void init() {
        _window.init(720, 720, "OpenGL Renderer");

        // read vertex shader data
        std::ifstream vs_file("../shaders/default.vert", std::ios::binary);
        vs_file.seekg(0, std::ios::end);
        GLint vs_size = vs_file.tellg();
        vs_file.seekg(0, std::ios::beg);
        std::vector<GLchar> vs_string(vs_size);
        GLchar* vs_data = vs_string.data();
        vs_file.read(vs_data, vs_size);
        // compile vertex shader
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vs_data, &vs_size);
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
        std::ifstream fs_file("../shaders/default.frag", std::ios::binary);
        fs_file.seekg(0, std::ios::end);
        GLint fs_size = fs_file.tellg();
        fs_file.seekg(0, std::ios::beg);
        std::vector<GLchar> fs_string(fs_size);
        GLchar* fs_data = fs_string.data();
        fs_file.read(fs_data, fs_size);
        // compile fragment shader
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fs_data, &fs_size);
        glCompileShader(fragment_shader);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment_shader, info_log.size(), nullptr, info_log.data());
            fmt::print("{}", info_log.data());
        }

        // to combine all shader stages, we create a shader program
        shader_program = glCreateProgram();
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
            glm::vec4 position;
            glm::vec4 color;
        };
        float n = -0.5f; // for readability
        float p = 0.5f; // for readability
        std::vector<Vertex> vertices = {
            // pos, norm, uv/st, col
            {{n, n, p, 1}, {1, 0, 0, 1}}, // front
            {{p, n, p, 1}, {1, 0, 0, 1}},
            {{n, p, p, 1}, {1, 0, 0, 1}},
            {{p, p, p, 1}, {1, 0, 0, 1}},
            {{n, n, n, 1}, {1, 0, 0, 1}}, // back
            {{p, n, n, 1}, {1, 0, 0, 1}},
            {{n, p, n, 1}, {1, 0, 0, 1}},
            {{p, p, n, 1}, {1, 0, 0, 1}},
            {{n, n, n, 1}, {0, 1, 0, 1}}, // left
            {{n, n, p, 1}, {0, 1, 0, 1}},
            {{n, p, n, 1}, {0, 1, 0, 1}},
            {{n, p, p, 1}, {0, 1, 0, 1}},
            {{p, n, n, 1}, {0, 1, 0, 1}}, // right
            {{p, n, p, 1}, {0, 1, 0, 1}},
            {{p, p, n, 1}, {0, 1, 0, 1}},
            {{p, p, p, 1}, {0, 1, 0, 1}},
            {{n, p, n, 1}, {0, 0, 1, 1}}, // top
            {{n, p, p, 1}, {0, 0, 1, 1}},
            {{p, p, n, 1}, {0, 0, 1, 1}},
            {{p, p, p, 1}, {0, 0, 1, 1}},
            {{n, n, n, 1}, {0, 0, 1, 1}}, // bottom
            {{n, n, p, 1}, {0, 0, 1, 1}},
            {{p, n, n, 1}, {0, 0, 1, 1}},
            {{p, n, p, 1}, {0, 0, 1, 1}},
        };
        
        // describe vertex buffer
        GLsizeiptr vertex_byte_count = vertices.size() * sizeof(Vertex);
        glCreateBuffers(1, &vertex_buffer_object);
        // upload data to GPU buffer
        glNamedBufferStorage(vertex_buffer_object, vertex_byte_count, vertices.data(), BufferStorageMask::GL_NONE_BIT);

        // create indices
        std::vector<uint32_t> indices = {
            0, 1, 3, 3, 2, 0, // front
            5, 4, 7, 7, 4, 6, // back
            8, 9, 11, 11, 10, 8, // left
            13, 12, 15, 15, 12, 14, // right
            16, 17, 19, 19, 18, 16, // top
            23, 21, 20, 23, 20, 22, // bottom
        };
        index_count = indices.size();
        // describe index buffer (element buffer)
        GLsizeiptr element_byte_count = vertices.size() * sizeof(Vertex);
        glCreateBuffers(1, &element_buffer_object);
        // upload data to GPU buffer
        glNamedBufferStorage(element_buffer_object, element_byte_count, indices.data(), BufferStorageMask::GL_NONE_BIT);

        // create vertex array buffer
        glCreateVertexArrays(1, &vertex_array_object);
        // assign both vertex and index (element) buffers
        glVertexArrayVertexBuffer(vertex_array_object, 0, vertex_buffer_object, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(vertex_array_object, element_buffer_object);
        // specify vertex format with vertex array buffer
        // struct Vertex {
        //     glm::vec4 position; <---
        //     glm::vec4 color;
        // };
        // total size of 4 floats, starts at byte 0*GL_FLOAT
        glVertexArrayAttribFormat(vertex_array_object, 0, 4, GL_FLOAT, GL_FALSE, 0 * sizeof(GL_FLOAT));
        glVertexArrayAttribBinding(vertex_array_object, 0, 0);
        glEnableVertexArrayAttrib(vertex_array_object, 0);
        // struct Vertex {
        //     glm::vec4 position;
        //     glm::vec4 color; <---
        // };
        // total size of 4 floats, starts at byte 4*GL_FLOAT
        glVertexArrayAttribFormat(vertex_array_object, 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT));
        glVertexArrayAttribBinding(vertex_array_object, 1, 0);
        glEnableVertexArrayAttrib(vertex_array_object, 1);
    }
    void destroy() {
        _window.destroy();
        // clean up
        glDeleteBuffers(1, &vertex_buffer_object);
        glDeleteBuffers(1, &element_buffer_object);
        glDeleteVertexArrays(1, &vertex_array_object);
        glDeleteProgram(shader_program);
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
        // draw
        if (Keys::down('w')) _transform.position.y += 0.01f;
        if (Keys::down('s')) _transform.position.y -= 0.01f;
        if (Keys::down('d')) _transform.position.x += 0.01f;
        if (Keys::down('a')) _transform.position.x -= 0.01f;
        if (Keys::down('q')) _transform.rotation += 0.01f;
        if (Keys::down('e')) _transform.rotation -= 0.01f;
        if (Mouse::down(Mouse::ids::left)) _transform.scale += 0.01f;
        if (Mouse::down(Mouse::ids::right)) _transform.scale -= 0.01f;
        glClearColor(0.1, 0.1, 0.1, 0.0); // theoretically only needs to be set once
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_program);
        _transform.bind();
        glBindVertexArray(vertex_array_object);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
        SDL_GL_SwapWindow(_window._window_p);
    }

    Window _window;
    Transform _transform;
    GLuint shader_program;
    GLuint vertex_buffer_object;
    GLuint element_buffer_object;
    GLuint vertex_array_object;
    GLuint index_count;
};