#pragma once
#include <string>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include "utils.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "cmrc_io.hpp"

// sample models:
// https://github.com/assimp/assimp-mdb
// model used here:
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

        // loading model from memory:
        // normally only feasible if model format is one single file (gltf, fbx, etc.)
        // we need to implement a custom virtual IO system for assimp
        // in order to load fragmented model formats (like .obj)
        importer.SetIOHandler(new CMRC_IOSystem());

        // load model
        const aiScene* pScene = importer.ReadFile(path, flags);
        if (pScene == nullptr) std::cerr << importer.GetErrorString() << '\n';
        if (pScene->mRootNode == nullptr  || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) std::cerr<<"err"<<std::endl;

        // prepare for data storage
        meshes.reserve(pScene->mNumMeshes);
        materials.resize(pScene->mNumMaterials);

        std::cout << meshes.capacity() << " meshes\n";
        std::cout << materials.size() << " materials\n";

        // create meshes
        for (int i = 0; i < pScene->mNumMeshes; i++) {
            aiMesh* pMesh = pScene->mMeshes[i];
            meshes.emplace_back(pMesh);
        }

        // create materials
        // https://assimp.sourceforge.net/lib_html/materials.html
        for (int i = 0; i < pScene->mNumMaterials; i++) {
            aiMaterial* pMaterial = pScene->mMaterials[i];

            if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE)) {
                // build path to texture resource
                aiString aiTexPath;
                pMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), aiTexPath);
                std::string texPath("../models/sponza/");
                texPath.append(aiTexPath.C_Str());

                // create texture
                GLuint texture;
                glCreateTextures(GL_TEXTURE_2D, 1, &texture);

                // load texture from disk using stbi
                int width, height, nChannels;
                stbi_uc* pImage = stbi_load(texPath.c_str(), &width, &height, &nChannels, 4);
                if (pImage == nullptr) std::cerr << "failed to load model texture" << std::endl;
                glTextureStorage2D(texture, 1, GL_RGBA8, width, height);
                glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
                stbi_image_free(pImage);

                // store texture into material
                materials[i].diffuseTexture = texture;
            }
        }
    }

    void draw() {

        transform.bind();
        for (int i = 0; i < meshes.size(); i++) {
            materials[meshes[i].materialIndex].bind();
            meshes[i].draw();
        }
    }


    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    Transform transform;
};