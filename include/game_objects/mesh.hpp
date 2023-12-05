#pragma once
#include "transform.hpp"
#include "material.hpp"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 st; // OpenGL-style uv coordinate
    glm::vec4 vertCol;
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

    void load_sphere(bool bInvertNormals = false) {
        // TODO
    }
    void load_cube(bool bInvertNormals = false) {
        float n = -0.5f; // for readability
        float p = 0.5f; // for readability
        vertices = {
            // pos, norm, uv/st, vertCol
            {{n, n, p}, {0, 0, +1}, {0, 0}, {1, 1, 1, 1}}, // front
            {{p, n, p}, {0, 0, +1}, {0, 0}, {1, 1, 1, 1}},
            {{n, p, p}, {0, 0, +1}, {0, 0}, {1, 1, 1, 1}},
            {{p, p, p}, {0, 0, +1}, {0, 0}, {1, 1, 1, 1}},
            {{n, n, n}, {0, 0, -1}, {0, 0}, {1, 1, 1, 1}}, // back
            {{p, n, n}, {0, 0, -1}, {0, 0}, {1, 1, 1, 1}},
            {{n, p, n}, {0, 0, -1}, {0, 0}, {1, 1, 1, 1}},
            {{p, p, n}, {0, 0, -1}, {0, 0}, {1, 1, 1, 1}},
            {{n, n, n}, {-1, 0, 0}, {0, 0}, {1, 1, 1, 1}}, // left
            {{n, n, p}, {-1, 0, 0}, {0, 0}, {1, 1, 1, 1}},
            {{n, p, n}, {-1, 0, 0}, {0, 0}, {1, 1, 1, 1}},
            {{n, p, p}, {-1, 0, 0}, {0, 0}, {1, 1, 1, 1}},
            {{p, n, n}, {+1, 0, 0}, {0, 0}, {1, 1, 1, 1}}, // right
            {{p, n, p}, {+1, 0, 0}, {0, 0}, {1, 1, 1, 1}},
            {{p, p, n}, {+1, 0, 0}, {0, 0}, {1, 1, 1, 1}},
            {{p, p, p}, {+1, 0, 0}, {0, 0}, {1, 1, 1, 1}},
            {{n, p, n}, {0, +1, 0}, {0, 0}, {1, 1, 1, 1}}, // top
            {{n, p, p}, {0, +1, 0}, {0, 0}, {1, 1, 1, 1}},
            {{p, p, n}, {0, +1, 0}, {0, 0}, {1, 1, 1, 1}},
            {{p, p, p}, {0, +1, 0}, {0, 0}, {1, 1, 1, 1}},
            {{n, n, n}, {0, -1, 0}, {0, 0}, {1, 1, 1, 1}}, // bottom
            {{n, n, p}, {0, -1, 0}, {0, 0}, {1, 1, 1, 1}},
            {{p, n, n}, {0, -1, 0}, {0, 0}, {1, 1, 1, 1}},
            {{p, n, p}, {0, -1, 0}, {0, 0}, {1, 1, 1, 1}},
        };
        indices = {
            0, 1, 3, 3, 2, 0, // front
            5, 4, 7, 7, 4, 6, // back
            8, 9, 11, 11, 10, 8, // left
            13, 12, 15, 15, 12, 14, // right
            16, 17, 19, 19, 18, 16, // top
            23, 21, 20, 23, 20, 22, // bottom
        };

        if (bInvertNormals) invert_normals();
        describe_layout();
    }
    void load_mesh(aiMesh* pMesh) {
        // handle all vertices for this mesh
        vertices.reserve(pMesh->mNumVertices);
        for (int i = 0; i < pMesh->mNumVertices; i++) {
            Vertex vertex;

            // extract positions
            vertex.pos.x = pMesh->mVertices[i].x;
            vertex.pos.y = pMesh->mVertices[i].y;
            vertex.pos.z = pMesh->mVertices[i].z;
            // extract normals
            vertex.norm.x = pMesh->mNormals[i].x;
            vertex.norm.y = pMesh->mNormals[i].y;
            vertex.norm.z = pMesh->mNormals[i].z;
            // extract uv/st coords
            vertex.st.s = pMesh->mTextureCoords[0][i].x;
            vertex.st.t = pMesh->mTextureCoords[0][i].y;
            // extract vertex colors (if present)
            if (pMesh->mColors[0] != nullptr) {
                vertex.vertCol.r = pMesh->mColors[0][i].r;
                vertex.vertCol.g = pMesh->mColors[0][i].g;
                vertex.vertCol.b = pMesh->mColors[0][i].b;
                vertex.vertCol.a = pMesh->mColors[0][i].a;
            }
            // set vertex color to negative if its not present
            else vertex.vertCol = glm::vec4(-1.0f);

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
        describe_layout();
    }

    void draw() {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    }

private:
    void describe_layout() {
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
        i = 3; // vertex color
        glVertexArrayAttribFormat(vao, i, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat));
        glVertexArrayAttribBinding(vao, i, binding);
        glEnableVertexArrayAttrib(vao, i);
    }
    void invert_normals() {
        std::for_each(vertices.begin(), vertices.end(), [](Vertex& vertex) {
            vertex.norm = vertex.norm * -1.0f;
        });
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