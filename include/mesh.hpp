#pragma once

struct Mesh {
    Mesh() {
        // from -1.0f to 1.0f in both x and y, depth set to 0
        vertices = {
            .0f, .0f, .0f,
            .5f, .0f, .0f,
            .0f, .5f, .0f,
            .5f, .5f, .0f,
        };
        indices = {
            0, 2, 1,
            1, 2, 3
        };

        // TODOOOO REORDER THESE STEPS??

        // OpenGL buffers will be our handle for GPU memory
        // we cannot interact with it directly like with cpu/system memory
        vbo; // simply a pointer/ID to our vertex buffer object
        glCreateBuffers(1, &vbo); // modern OpenGL (4.5+)
        GLsizeiptr nBytes = vertices.size() * sizeof(GLfloat);
        glNamedBufferStorage(vbo, nBytes, vertices.data(), 0);

        ebo; // element buffer object (or: index buffer object)
        glCreateBuffers(1, &ebo);
        nBytes = indices.size() * sizeof(GLuint);
        glNamedBufferStorage(ebo, nBytes, indices.data(), 0);

        // we need to tell OpenGL how to interpret vertex data
        // this info is stored inside the vao
        vao; // create our vertex array object
        glCreateVertexArrays(1, &vao);
        GLuint i = 0; // location of "pos" in vertex shader
        // specify buffers
        glVertexArrayVertexBuffer(vao, 0, vbo, 0, 3 * sizeof(GLfloat));
        glVertexArrayElementBuffer(vao, ebo);
        // specify vertex format
        glVertexArrayAttribFormat(vao, i, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vao, i, 0);
        // enable the "pos" attribute
        glEnableVertexArrayAttrib(vao, i);
    }
    ~Mesh() {
        std::vector<GLuint> buffers = { vao, vbo, ebo };
        glDeleteBuffers(buffers.size(), buffers.data());
    }        

    void draw() {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    }

private:
    GLuint vao; // vertex array object
    GLuint vbo; // vertex buffer object
    GLuint ebo; // element buffer object
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
};