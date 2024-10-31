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
        glDeleteBuffers(1, &_vertex_buffer_object);
        glDeleteBuffers(1, &_element_buffer_object);
        glDeleteVertexArrays(1, &_vertex_array_object);
    }
    // draw the mesh using previously bound pipeline
    void draw() {
        glBindVertexArray(_vertex_array_object);
        glDrawElements(GL_TRIANGLES, _index_count, GL_UNSIGNED_INT, nullptr);
    }

    GLuint _vertex_buffer_object;
    GLuint _element_buffer_object;
    GLuint _vertex_array_object;
    GLsizei _index_count;
};