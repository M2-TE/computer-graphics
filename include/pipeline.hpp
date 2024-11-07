#pragma once
#include <fstream>
#include <fmt/base.h>
#include <glbinding/gl46core/gl.h>
using namespace gl46core;

struct Pipeline {
    // compile shaders and link shader program
    void init() {
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
        _shader_program = glCreateProgram();
        glAttachShader(_shader_program, vertex_shader);
        glAttachShader(_shader_program, fragment_shader);
        glLinkProgram(_shader_program);
        glGetProgramiv(_shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(_shader_program, info_log.size(), nullptr, info_log.data());
            fmt::print("{}", info_log.data());
        }
        // clean up shaders after they were compiled and linked
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
    // clean up shader program object
    void destroy() {
        glDeleteProgram(_shader_program);
    }
    // bind the shader program (needs to be done before binding meshes or uniforms)
    void bind() {
        glUseProgram(_shader_program);
    }
    GLuint _shader_program;
};