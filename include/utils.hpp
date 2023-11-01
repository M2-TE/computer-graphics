#pragma once

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