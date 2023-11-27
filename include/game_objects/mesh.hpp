#pragma once
#include "transform.hpp"
#include "material.hpp"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 uv;
};

struct Mesh {
    Mesh() {
        glCreateVertexArrays(1, &vao); // vertex array object
        glCreateBuffers(1, &vbo); // vertex buffer object
        glCreateBuffers(1, &ebo); // element buffer object (or: index buffer object)
    }
    ~Mesh() {
        std::vector<GLuint> buffers = { vao, vbo, ebo };
        glDeleteBuffers(buffers.size(), buffers.data());
    }

    void generate_cube() {
        // cube
        float n = -0.5f; // for readability
        float p = 0.5f; // for readability
        vertices = {
            n, n, p, .5f, .0f, .0f, 1.0/3.0, 0.00f, 0.0f, 0.0f, 1.0f, // front
            p, n, p, .0f, .5f, .0f, 2.0/3.0, 0.00f, 0.0f, 0.0f, 1.0f,
            n, p, p, .0f, .0f, .5f, 1.0/3.0, 0.25f, 0.0f, 0.0f, 1.0f,
            p, p, p, .5f, .5f, .0f, 2.0/3.0, 0.25f, 0.0f, 0.0f, 1.0f,

            n, n, n, .5f, .0f, .0f, 1.0/3.0, 0.00f, 0.0f, 0.0f, -1.0f, // back
            p, n, n, .0f, .5f, .0f, 2.0/3.0, 0.00f, 0.0f, 0.0f, -1.0f,
            n, p, n, .0f, .0f, .5f, 1.0/3.0, 0.25f, 0.0f, 0.0f, -1.0f,
            p, p, n, .5f, .5f, .0f, 2.0/3.0, 0.25f, 0.0f, 0.0f, -1.0f,

            n, n, n, .5f, .0f, .0f, 1.0/3.0, 0.00f, -1.0f, 0.0f, 0.0f, // left
            n, n, p, .0f, .5f, .0f, 2.0/3.0, 0.00f, -1.0f, 0.0f, 0.0f,
            n, p, n, .0f, .0f, .5f, 1.0/3.0, 0.25f, -1.0f, 0.0f, 0.0f,
            n, p, p, .5f, .5f, .0f, 2.0/3.0, 0.25f, -1.0f, 0.0f, 0.0f,

            p, n, n, .5f, .0f, .0f, 1.0/3.0, 0.00f, 1.0f, 0.0f, 0.0f, // right
            p, n, p, .0f, .5f, .0f, 2.0/3.0, 0.00f, 1.0f, 0.0f, 0.0f,
            p, p, n, .0f, .0f, .5f, 1.0/3.0, 0.25f, 1.0f, 0.0f, 0.0f,
            p, p, p, .5f, .5f, .0f, 2.0/3.0, 0.25f, 1.0f, 0.0f, 0.0f,

            n, p, n, .5f, .0f, .0f, 1.0/3.0, 0.50f, 0.0f, 1.0f, 0.0f, // top
            n, p, p, .0f, .5f, .0f, 2.0/3.0, 0.50f, 0.0f, 1.0f, 0.0f,
            p, p, n, .0f, .0f, .5f, 1.0/3.0, 0.25f, 0.0f, 1.0f, 0.0f,
            p, p, p, .5f, .5f, .0f, 2.0/3.0, 0.25f, 0.0f, 1.0f, 0.0f,

            n, n, n, .5f, .0f, .0f, 1.0/3.0, 0.75f, 0.0f, -1.0f, 0.0f, // bottom
            n, n, p, .0f, .5f, .0f, 2.0/3.0, 0.75f, 0.0f, -1.0f, 0.0f,
            p, n, n, .0f, .0f, .5f, 1.0/3.0, 1.00f, 0.0f, -1.0f, 0.0f,
            p, n, p, .5f, .5f, .0f, 2.0/3.0, 1.00f, 0.0f, -1.0f, 0.0f,
        };
        indices = {
            0, 1, 3, 3, 2, 0, // front
            5, 4, 7, 7, 4, 6, // back
            8, 9, 11, 11, 10, 8, // left
            13, 12, 15, 15, 12, 14, // right
            16, 17, 19, 19, 18, 16, // top
            23, 21, 20, 23, 20, 22, // bottom
        };

        // OpenGL buffers will be our handle for GPU memory
        // we cannot interact with it directly like with cpu/system memory
        GLsizeiptr nBytes = vertices.size() * sizeof(GLfloat);
        glNamedBufferStorage(vbo, nBytes, vertices.data(), 0);

        nBytes = indices.size() * sizeof(GLuint);
        glNamedBufferStorage(ebo, nBytes, indices.data(), 0);

        // we need to tell OpenGL how to interpret vertex data
        // this info is stored inside the vao
        // specify buffers
        glVertexArrayVertexBuffer(vao, 0, vbo, 0, 8 * sizeof(GLfloat));
        glVertexArrayElementBuffer(vao, ebo);
        // specify vertex format
        GLuint binding = 0;
        GLuint i;
        i = 0; // position
        glVertexArrayAttribFormat(vao, i, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vao, i, binding);
        glEnableVertexArrayAttrib(vao, i);
        i = 1; // normal
        glVertexArrayAttribFormat(vao, i, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
        glVertexArrayAttribBinding(vao, i, binding);
        glEnableVertexArrayAttrib(vao, i);
        i = 2; // uv coordinate
        glVertexArrayAttribFormat(vao, i, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat));
        glVertexArrayAttribBinding(vao, i, binding);
        glEnableVertexArrayAttrib(vao, i);

        // load image
        int width, height, nChannels; // output for stbi_load_from_memory
        auto rawImage = load_image("images/grass.png");
        stbi_uc* pImage = stbi_load_from_memory(rawImage.first, rawImage.second, &width, &height, &nChannels, 3); // explicitly ask for 3 channels
        // create texture to store image in (texture is gpu buffer)
        GLuint texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(texture, 1, GL_RGB8, width, height);
        glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pImage);
        stbi_image_free(pImage);
        // sampler parameters
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT); // s is the u coordinate (width)
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT); // t is the v coordinate (height)
        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // interpolation mode when scaling image down
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // interpolation mode when scaling image up
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