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
    Model(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, std::string path)
    : transform(pos, rot, scale) {
        Assimp::Importer importer;

        // flags that allow some automatic post processing of model
        unsigned int flags = 0; // https://assimp.sourceforge.net/lib_html/postprocess_8h.html
        flags |= aiProcess_Triangulate; // triangulate all faces if not already triangulated
        flags |= aiProcess_GenNormals; // generate normals if they dont exist
        flags |= aiProcess_FlipUVs; // OpenGL prefers flipped y axis
        flags |= aiProcess_PreTransformVertices; // simplifies model load
        
        // load model
        importer.SetIOHandler(new CMRC_IOSystem()); // custom virtual IO system
        const aiScene* pScene = importer.ReadFile(path, flags);
        if (pScene == nullptr) std::cerr << importer.GetErrorString() << '\n';
        else std::cout << "Loaded model: " << path << std::endl;

        // we need to figure out the path the root of a model for formats such as .obj
        size_t sepIndex = path.find_last_of('/');
        modelRoot = path.substr(0, sepIndex + 1);


        // TESTING
        // path = std::string("../").append(path);
        // importer.SetIOHandler(nullptr);
        // const aiScene* pScene2 = importer.ReadFile(path, flags);
        // if (pScene2 == nullptr) std::cerr << importer.GetErrorString() << '\n';
        // else std::cout << "Loaded model: " << path << std::endl;
        // std::cout 
        //     << "Meshes: " << pScene2->mNumMeshes << '\n'
        //     << "Materials " << pScene2->mNumMaterials << '\n'
        //     << "Textures: " << pScene2->mNumTextures << '\n';

        // prepare for data storage
        meshes.reserve(pScene->mNumMeshes);
        textures.reserve(pScene->mNumTextures);
        materials.resize(pScene->mNumMaterials);

        std::cout
            << "Meshes: " << pScene->mNumMeshes << '\n'
            << "Materials " << pScene->mNumMaterials << '\n'
            << "Textures: " << pScene->mNumTextures << '\n';

        // create meshes
        for (int i = 0; i < pScene->mNumMeshes; i++) {
            aiMesh* pMesh = pScene->mMeshes[i];
            meshes.emplace_back(pMesh);
        }

        // create textures (if embedded into model, such as .fbx)
        for (int i = 0; i < pScene->mNumTextures; i++) {
            aiTexture* pTexture = pScene->mTextures[i];
            GLuint texture;
            
            // check if embedded texture is compressed
            if (pTexture->mHeight == 0) {
                texture = load_texture(pTexture);
            }
            else std::cerr << "uncompressed embedded images not handled yet\n";
        }

        // create materials
        // https://assimp.sourceforge.net/lib_html/materials.html
        for (int i = 0; i < pScene->mNumMaterials; i++) {
            aiMaterial* pMaterial = pScene->mMaterials[i];
            Material& material = materials[i];
            aiColor3D color;

            // Ambient Color
            pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
            material.ambient.r = color.r;
            material.ambient.g = color.g;
            material.ambient.b = color.b;
            // Diffuse Color
            pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            material.diffuse.r = color.r;
            material.diffuse.g = color.g;
            material.diffuse.b = color.b;
            // Specular Color
            pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
            material.specular.r = color.r;
            material.specular.g = color.g;
            material.specular.b = color.b;
            // Shininess and its strength
            pMaterial->Get(AI_MATKEY_SHININESS, material.shininess);
            pMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, material.shininessStrength);

            // load diffuse texture
            if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE)) {
                material.diffuseTexture = get_texture(pMaterial, aiTextureType_DIFFUSE);
            }
        }

    }

    void draw() {
        transform.bind();
        for (int i = 0; i < meshes.size(); i++) {
            Material& material = materials[meshes[i].materialIndex];
            material.bind();
            meshes[i].draw();
        }
    }

private:
    GLuint get_texture(aiMaterial* pMaterial, aiTextureType texType) {
        // build path to texture resource
        aiString aiTexPath;
        pMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), aiTexPath);

        // check if we previously loaded this texture
        auto textureMapIter = textures.find(aiTexPath.C_Str());
        if (textureMapIter != textures.end()) return textureMapIter->second;

        // load texture from memory using stbi
        std::string texPath(modelRoot);
        texPath.append(aiTexPath.C_Str());
        auto rawTex = load_model_resource(texPath);
        int width, height, nChannels;
        stbi_uc* pImage = stbi_load_from_memory(rawTex.first, rawTex.second, &width, &height, &nChannels, 4);
        if (pImage == nullptr) std::cerr << "failed to load model texture" << std::endl;

        // create texture
        GLuint texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(texture, 1, GL_RGBA8, width, height);
        glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
        stbi_image_free(pImage);

        // add texture to our lookup map
        textures[aiTexPath.C_Str()] = texture;
        return texture;
    }
    GLuint load_texture(aiTexture* pTexture) {
        // uncompress using stbi
        int width, height, nChannels;
        const stbi_uc* pCompressedTexture = reinterpret_cast<stbi_uc*>(pTexture->pcData); // cast to a type that stbi understands
        stbi_uc* pImage = stbi_load_from_memory(pCompressedTexture, pTexture->mWidth, &width, &height, &nChannels, 4);
        if (pImage == nullptr) std::cerr << "failed to load embedded model texture" << std::endl;

        // create texture
        GLuint texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(texture, 1, GL_RGBA8, width, height);
        glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pImage);
        stbi_image_free(pImage);

        // add texture to our lookup map
        textures[pTexture->mFilename.C_Str()] = texture;

        return texture;
    }

    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    std::unordered_map<std::string, GLuint> textures;
    std::string modelRoot;
public:
    Transform transform;
};