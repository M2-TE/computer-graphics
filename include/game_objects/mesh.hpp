#pragma once
#include "transform.hpp"
#include "material.hpp"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 norm;
    // glm::vec3 
    glm::vec2 uv;
};

struct Mesh {
    Mesh() {
        glCreateVertexArrays(1, &vao); // vertex array object
        glCreateBuffers(1, &vbo); // vertex buffer object
        glCreateBuffers(1, &ebo); // element buffer object (or: index buffer object)
    }
    Mesh(aiMesh* pMesh) : Mesh() {
        load_mesh(pMesh);
    }
    ~Mesh() {
        std::vector<GLuint> buffers = { vao, vbo, ebo };
        glDeleteBuffers(buffers.size(), buffers.data());
    }

    void generate_sphere() {
        // TODO
    }
    void generate_cube() {
        vertices = {

        };
        indices = {

        };
    }
    void load_mesh(aiMesh* pMesh) {
        // handle all vertices for this mesh
        vertices.reserve(pMesh->mNumVertices);
        for (int i = 0; i < pMesh->mNumVertices; i++) {
            Vertex vertex;

            // extract positions from mVertices
            vertex.pos.x = pMesh->mVertices[i].x;
            vertex.pos.y = pMesh->mVertices[i].y;
            vertex.pos.z = pMesh->mVertices[i].z;
            // extract normals from mNormals
            vertex.norm.x = pMesh->mNormals[i].x;
            vertex.norm.y = pMesh->mNormals[i].y;
            vertex.norm.z = pMesh->mNormals[i].z;
            // extract uv coords from 
            vertex.uv.x = pMesh->mTextureCoords[0][i].x;
            vertex.uv.y = pMesh->mTextureCoords[0][i].y;

            vertices.push_back(vertex);
        }

        // handle all indices, each face is 3 indices
        indices.reserve(pMesh->mNumFaces * 3);
        for (int i = 0; i < pMesh->mNumFaces; i++) {
            aiFace face = pMesh->mFaces[i];
            for (int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // simply assigned the material index for later
        materialIndex = pMesh->mMaterialIndex;

        // describe vertex buffer
        GLsizeiptr nBytes = vertices.size() * sizeof(Vertex);
        glNamedBufferStorage(vbo, nBytes, vertices.data(), 0);
        // describe index buffer
        nBytes = indices.size() * sizeof(GLuint);
        glNamedBufferStorage(ebo, nBytes, indices.data(), 0);

        // describe buffer contents
        glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(vao, ebo);
        // specify vertex format
        GLuint binding = 0;
        GLuint i;
        i = 0; // position
        glVertexArrayAttribFormat(vao, i, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vao, i, binding);
        glEnableVertexArrayAttrib(vao, i);
        i = 1; // normal
        glVertexArrayAttribFormat(vao, i, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
        glVertexArrayAttribBinding(vao, i, binding);
        glEnableVertexArrayAttrib(vao, i);
        i = 2; // uv coordinate
        glVertexArrayAttribFormat(vao, i, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat));
        glVertexArrayAttribBinding(vao, i, binding);
        glEnableVertexArrayAttrib(vao, i);
    }

    void draw() {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    }

public:
    unsigned int materialIndex;

private:
    GLuint vao; // vertex array object
    GLuint vbo; // vertex buffer object
    GLuint ebo; // element buffer object
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};