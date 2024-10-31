#pragma once
#include <glbinding/gl46core/gl.h>
using namespace gl46core;

struct Pipeline {
    // compile shaders and link shader program
    void init() {
        // TODO
    }
    // clean up shader program object
    void destroy() {
        glDeleteProgram(shader_program);
    }
    // bind the shader program (needs to be done before binding meshes or uniforms)
    void bind() {
        glUseProgram(shader_program);
    }
    GLuint shader_program;
};