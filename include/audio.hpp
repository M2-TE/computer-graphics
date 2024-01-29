#pragma once
#include <SDL3_mixer/SDL_mixer.h>
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(audio);

struct Audio {
    Audio() {
        // open audio device
        // SDL_AudioSpec spec = {};
        if (Mix_OpenAudio(0, nullptr)) std::cout << SDL_GetError();

        // // load audio sample
        // auto* sample = samples.emplace_back(Mix_LoadWAV("audio/Bass-Drum-1.wav"));
        // if (sample == nullptr) std::cout << SDL_GetError();

        // // load music track
        // auto* music = tracks.emplace_back(Mix_LoadMUS("audio/doom.mp3"));
        // if (music == nullptr) std::cout << SDL_GetError();
        // Mix_PlayMusic(music, 0); // immediately play music
        // // Mix_PauseMusic();
        // // Mix_ResumeMusic();

        // load audio sample
        samples.emplace_back(load_chunk("audio/Bass-Drum-1.wav"));
        tracks.emplace_back(load_music("audio/doom.mp3"));
        Mix_PlayMusic(tracks[0], 0); // immediately play music
    }
    ~Audio() {
        for (auto* sample: samples) {
            Mix_FreeChunk(sample);
        }

        for (auto* track: tracks) {
            Mix_FreeMusic(track);
        }

        Mix_CloseAudio();
        Mix_Quit();
    }
    
    SDL_RWops* load_memory(std::string_view path) {
        auto fs = cmrc::audio::get_filesystem();
        std::string path_str = std::string(path);
        if (fs.exists(path_str)) {
            auto file = fs.open(path_str);
            SDL_RWops* pMemory = SDL_RWFromConstMem(file.cbegin(), file.size());
            if (pMemory == nullptr) std::cout << SDL_GetError();
            return pMemory;
        }
        else {
            std::cerr << "Unable to load audio: " << path << std::endl;
            return nullptr;
        }
    }
    Mix_Music* load_music(std::string_view path) {
        SDL_RWops* pMemory = load_memory(path);
        Mix_Music* pMusic = Mix_LoadMUS_RW(pMemory, false);
        if (pMusic == nullptr) std::cout << SDL_GetError();
        return pMusic;
    }
    Mix_Chunk* load_chunk(std::string_view path) {
        SDL_RWops* pMemory = load_memory(path);
        Mix_Chunk* pChunk = Mix_LoadWAV_RW(pMemory, false);
        if (pChunk == nullptr) std::cout << SDL_GetError();
        return pChunk;
    }

    std::vector<Mix_Chunk*> samples;
    std::vector<Mix_Music*> tracks;
};