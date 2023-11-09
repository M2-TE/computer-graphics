#pragma once

#include <stb_image.h>
#include "transform.hpp"

struct Mesh {
    Mesh() {
        // from -1.0f to 1.0f in both x and y, depth set to 0
        vertices = {
            .0f, .0f, .0f, .5f, .0f, .0f, 0.0f, 0.0f,
            .5f, .0f, .0f, .0f, .5f, .0f, 1.0f, 0.0f,
            .0f, .5f, .0f, .0f, .0f, .5f, 0.0f, 1.0f,
            .5f, .5f, .0f, .5f, .5f, .0f, 1.0f, 1.0f,
        };
        indices = {
            0, 2, 1,
            1, 2, 3
        };

        // OpenGL buffers will be our handle for GPU memory
        // we cannot interact with it directly like with cpu/system memory
        vbo; // simply a pointer/ID to our vertex buffer object
        glCreateBuffers(1, &vbo); // modern OpenGL (4.5+) ..Create.. instead of ..Gen..
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
        // specify buffers
        glVertexArrayVertexBuffer(vao, 0, vbo, 0, 8 * sizeof(GLfloat));
        glVertexArrayElementBuffer(vao, ebo);
        // specify vertex format
        GLuint binding = 0;
        GLuint i;
        i = 0; // position
        glVertexArrayAttribFormat(vao, i, 3, GL_FLOAT, GL_FALSE, 0); // position
        glVertexArrayAttribBinding(vao, i, binding); // position
        glEnableVertexArrayAttrib(vao, i);
        i = 1; // color
        glVertexArrayAttribFormat(vao, i, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat)); // color
        glVertexArrayAttribBinding(vao, i, binding); // color
        glEnableVertexArrayAttrib(vao, i);
        i = 2; // texture coordinate
        glVertexArrayAttribFormat(vao, i, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat)); // texture coordinate
        glVertexArrayAttribBinding(vao, i, binding); // color
        glEnableVertexArrayAttrib(vao, i);

        // load image
        int width, height, nChannels; // output for stbi_load_from_memory
        auto rawImage = load_image("images/bricks.jpg");
        stbi_uc* pImage = stbi_load_from_memory(rawImage.first, rawImage.second, &width, &height, &nChannels, 3); // explicitly ask for 3 channels
        // create texture to store image in (texture is gpu buffer)
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(texture, 1, GL_RGB8, width, height);
        glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pImage);
        stbi_image_free(pImage);
        // sampler parameters
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT); // s is the u coordinate (width)
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT); // t is the v coordinate (height)
        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // interpolation mode when scaling image down
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // interpolation mode when scaling image up
    }
    ~Mesh() {
        std::vector<GLuint> buffers = { vao, vbo, ebo };
        glDeleteBuffers(buffers.size(), buffers.data());
    }        

    void draw() {
        transform.bind();
        glBindVertexArray(vao);
        glBindTextureUnit(0, texture);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    }

public:
    Transform transform;

private:
    GLuint texture; // texture buffer
    GLuint vao; // vertex array object
    GLuint vbo; // vertex buffer object
    GLuint ebo; // element buffer object
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
};