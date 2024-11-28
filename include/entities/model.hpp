#pragma once
#include "transform.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "mesh.hpp"

struct Model {
    void init(Mesh::Primitive primitive) {
        switch (primitive) {
            case Mesh::eCube: _mesh.init(); break;
            case Mesh::eSphere: _mesh.init(32, 32); break;
        }
        _material._texture_contribution = 0.0;
    }
    void init(const char* texture_path) {
        init(Mesh::eCube);
        _texture.init(texture_path);
        _material._texture_contribution = 1.0;
    }
    void destroy() {
        if (_material._texture_contribution > 0.0) _texture.destroy();
        _mesh.destroy();
    }

    void draw() {
        if (_material._texture_contribution > 0.0) _texture.bind();
        _material.bind();
        _transform.bind();
        _mesh.draw();
    }

    Mesh _mesh;
    Texture _texture;
    Transform _transform;
    Material _material;
};