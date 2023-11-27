#pragma once
#include <string>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "utils.hpp"
#include "mesh.hpp"
#include "material.hpp"

// sample models:
// https://github.com/assimp/assimp-mdb
// https://github.com/jimmiebergmann/Sponza
struct Model {
    Model(std::string path) {
        Assimp::Importer importer;

        // flags that allow some automatic post processing of model
        unsigned int flags = 0; // https://assimp.sourceforge.net/lib_html/postprocess_8h.html
        flags |= aiProcess_Triangulate; // triangulate all faces if not already triangulated
        flags |= aiProcess_GenNormals; // generate normals if they dont exist
        flags |= aiProcess_GenUVCoords; // generate uv coords if they dont exist
        flags |= aiProcess_FlipUVs; // OpenGL prefers flipped y axis
        // flags |= aiProcess_SplitLargeMeshes; // split into multiple meshes if model is too large
        // flags |= aiProcess_OptimizeMeshes; // merge multiple meshes into a single one

        /// version A: loading model from memory
        /// only really feasible if model format is one single file (gltf, fbx, etc.)
        std::size_t extOffset = path.find_last_of("."); // we often have to explicitly tell it the file extensions
        std::string extension = path.substr(extOffset);
        auto modelData = load_model(path); // retrieve model from memory
        const aiScene *scene = importer.ReadFileFromMemory(modelData.first, modelData.second, flags, extension.c_str());
        /// version B: loading model from disk
        /// this needs to be used for weird formats like .obj that are not just one file
        // const aiScene *scene = importer.ReadFile("../models/dino.fbx", flags);
        if (scene == nullptr) {
            std::cout << importer.GetErrorString() << '\n';
        }
        
        // prepare for data storage
        meshes.reserve(scene->mNumMeshes);
        textures.reserve(scene->mNumTextures);
        materials.reserve(scene->mNumMaterials);

        // helpful debug outputs
        std::cout << meshes.capacity() << " meshes\n";
        std::cout << textures.capacity() << " textures\n";
        std::cout << materials.capacity() << " materials\n";

        meshes.emplace_back();

    }
    ~Model() {
        glDeleteTextures(textures.size(), textures.data());
    }

    void draw() {

        // glBindTextureUnit(0, texture);
    }

    std::vector<Mesh> meshes;
    std::vector<GLuint> textures;
    std::vector<Material> materials;
    Transform transform;
};