#pragma once
#include "transform.hpp"
#include "texture.hpp"
#include "mesh.hpp"

struct Model {
    void init() {
        _mesh.init();
        _has_texture = false;
    }
    void init(const char* texture_path) {
        init();
        _texture.init(texture_path);
        _has_texture = true;
    }
    void destroy() {
        if (_has_texture) _texture.destroy();
        _mesh.destroy();
    }

    void draw() {
        if (_has_texture) {
            _texture.bind();
            glUniform1ui(17, 1);
        }
        else {
            glUniform1ui(17, 0);
        }
        _transform.bind();
        _mesh.draw();
    }

    Mesh _mesh;
    Transform _transform;
    Texture _texture;
    bool _has_texture;
};