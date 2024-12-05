#pragma once
#include <fmt/base.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include "transform.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "mesh.hpp"

struct Model {
    void init(Mesh::Primitive primitive) {
        _meshes.emplace_back();
        switch (primitive) {
            case Mesh::eCube: _meshes.front().init(); break;
            case Mesh::eSphere: _meshes.front().init(32, 32); break;
        }
        _materials.emplace_back()._texture_contribution = 0.0;
    }
    void init(Mesh::Primitive primitive, const char* texture_path) {
        _meshes.emplace_back();
        switch (primitive) {
            case Mesh::eCube: _meshes.front().init(); break;
            case Mesh::eSphere: _meshes.front().init(32, 32); break;
        }
        _textures.emplace_back().init(texture_path);
        _materials.emplace_back()._texture_contribution = 1.0;
    }
    void init(std::string model_path) {
        Assimp::Importer importer;

        // flags that allow some automatic post processing of model
        unsigned int flags = 0; // https://assimp.sourceforge.net/lib_html/postprocess_8h.html
        flags |= aiProcess_Triangulate; // triangulate all faces if not already triangulated
        flags |= aiProcess_GenNormals; // generate normals if they dont exist
        flags |= aiProcess_FlipUVs; // OpenGL prefers flipped y axis
        flags |= aiProcess_PreTransformVertices; // simplifies model load

        // load the entire "scene" (may be multiple meshes, hence scene)
        const aiScene* scene_p = importer.ReadFile(model_path, flags);
        if (scene_p == nullptr) {
            fmt::println("{}", importer.GetErrorString());
            return;
        }
        fmt::println("Loading model: {}", model_path);

        // figure out path to the model root for stuff like .obj, which puts its assets into sub-folders
        size_t separator_index = model_path.find_last_of('/');
        std::string model_root = model_path.substr(0, separator_index + 1);
        fmt::println("Model root: {}", model_root);

        // create materials and textures
        _materials.resize(scene_p->mNumMaterials);
        _textures.resize(scene_p->mNumMaterials);
        for (uint32_t i = 0; i < scene_p->mNumMaterials; i++) {
            aiMaterial* material_p = scene_p->mMaterials[i];
            Material& material = _materials[i];

            // load basic material properties
            material_p->Get(AI_MATKEY_SHININESS, material._specular);
            material_p->Get(AI_MATKEY_SHININESS_STRENGTH, material._specular_shininess);

            // load lighting colors
            aiColor3D color;
            material_p->Get(AI_MATKEY_COLOR_AMBIENT, color);
            material_p->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            material_p->Get(AI_MATKEY_COLOR_SPECULAR, color);

            // see if this should use a texture
            if (material_p->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                material._texture_contribution = 1.0;
                // load the texture
                aiString texture_path;
                material_p->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_path);
                std::string full_texture_path = model_root;
                full_texture_path.append(texture_path.C_Str());
                _textures[i].init(full_texture_path.c_str());
            }
        }
        
        // create meshes
        _meshes.resize(scene_p->mNumMeshes);
        for (uint32_t i = 0; i < scene_p->mNumMeshes; i++) {
            aiMesh* mesh_p = scene_p->mMeshes[i];
            _meshes[i].init(mesh_p);
        }
    }
    void destroy() {
        for (auto texture: _textures) {
            texture.destroy();
        }
        for (auto mesh: _meshes) {
            mesh.destroy();
        }
    }

    void draw() {
        _transform.bind();
        for (uint32_t i = 0; i < _meshes.size(); i++) {
            uint32_t material_index = _meshes[i]._material_index;
            if (material_index < _textures.size()) {
                _textures[material_index].bind();
            }
            _materials[material_index].bind();
            _meshes[i].draw();
        }
    }

    std::vector<Mesh> _meshes;
    std::vector<Material> _materials;
    std::vector<Texture> _textures; // TODO: move into material
    Transform _transform;
};