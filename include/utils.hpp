#pragma once

// simplifies the loading of shaders
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(shaders);
static inline const char* load_shader(const std::string& path) {
    auto fs = cmrc::shaders::get_filesystem();
    auto file = fs.open(path);
    return file.cbegin();
}

// reports information about potential OpenGL API misuse
static void openglCallbackFunction(
    GLenum source, GLenum type, GLuint id, 
    GLenum severity,  GLsizei length,
    const GLchar* message, const void* userParam) {
    fprintf(stderr, "%s\n", message);
    if (severity==GL_DEBUG_SEVERITY_HIGH) {
        fprintf(stderr, "Aborting...\n");
        abort();
    }
}