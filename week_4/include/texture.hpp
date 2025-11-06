#pragma once

struct Texture {
    void init(const std::string& path) {
        // base directory of the executable
        std::string base_path = SDL_GetBasePath();
        std::string full_path = base_path + "assets/textures/" + path;
        
        // load image from disk
        SDL_Surface* texture_raw = IMG_Load(full_path.c_str());
        if (texture_raw == nullptr) {
            std::println("IMG_Load(\"{}\"): {}", path, SDL_GetError());
            std::exit(1);
        }
        _width = texture_raw->w;
        _height = texture_raw->h;

        // create texture to store image in (texture is gpu buffer)
        glCreateTextures(GL_TEXTURE_2D, 1, &_texture);

        // figure out how many color channels it has (e.g. rgb vs rgba) and then upload to the GPU
        switch (texture_raw->format) {
            case SDL_PIXELFORMAT_RGBA32:
                glTextureStorage2D(_texture, 1, GL_SRGB8_ALPHA8, _width, _height);
                glTextureSubImage2D(_texture, 0, 0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, texture_raw->pixels);
                break;
            case SDL_PIXELFORMAT_RGB24:
                glTextureStorage2D(_texture, 1, GL_SRGB8, _width, _height);
                glTextureSubImage2D(_texture, 0, 0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, texture_raw->pixels);
                break;
            default:
                std::println("Unkown pixel format of loaded texture {}", path);
                exit(1);
        }

        // free image on cpu side
        SDL_DestroySurface(texture_raw);
    }
    void destroy() {
        glDeleteTextures(1, &_texture);
    }
    void bind() {
        glBindTextureUnit(0, _texture);
    }

    GLuint _texture;
    GLuint _width;
    GLuint _height;
};