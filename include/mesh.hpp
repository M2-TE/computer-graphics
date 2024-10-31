#pragma once
#include <glbinding/gl46core/gl.h>
using namespace gl46core;

struct Mesh {
    // create mesh data (vertices, indices/elements, vertex arrays)
    void init() {
        // TODO
    }
    // clean up mesh buffers
    void destroy() {
        glDeleteBuffers(1, &vertex_buffer_object);
        glDeleteBuffers(1, &element_buffer_object);
        glDeleteVertexArrays(1, &vertex_array_object);
    }
    // draw the mesh using previously bound pipeline
    void draw() {
        glBindVertexArray(vertex_array_object);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    }

    GLuint vertex_buffer_object;
    GLuint element_buffer_object;
    GLuint vertex_array_object;
    GLsizei index_count;
};