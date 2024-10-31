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
        glDeleteProgram(_shader_program);
    }
    // bind the shader program (needs to be done before binding meshes or uniforms)
    void bind() {
        glUseProgram(_shader_program);
    }
    GLuint _shader_program;
};