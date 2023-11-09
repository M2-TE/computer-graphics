#pragma once

// simplifies the loading of shaders
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(shaders);
static inline const char* load_shader(const std::string& path) {
    auto fs = cmrc::shaders::get_filesystem();
    auto file = fs.open(path);
    return file.cbegin();
}

// simplifies loading of images
CMRC_DECLARE(images);
static inline std::pair<const unsigned char*, std::size_t> load_image(const std::string& path) {
    auto fs = cmrc::images::get_filesystem();
    auto file = fs.open(path);
    return { reinterpret_cast<const unsigned char*>(file.cbegin()), file.size() };
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