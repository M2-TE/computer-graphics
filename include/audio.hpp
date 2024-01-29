#pragma once
#include <SDL3_mixer/SDL_mixer.h>

struct Audio {
    Audio() {
        // open audio device
        SDL_AudioSpec spec = {};
        if (Mix_OpenAudio(0, &spec)) std::cout << SDL_GetError();

        // load audio sample
        auto* sample = samples.emplace_back(Mix_LoadWAV("audio/Bass-Drum-1.wav"));
        if (sample == nullptr) std::cout << SDL_GetError();

        // load music track
        auto* music = tracks.emplace_back(Mix_LoadMUS("audio/doom.mp3"));
        if (music == nullptr) std::cout << SDL_GetError();
        Mix_PlayMusic(music, 0); // immediately play music
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

    std::vector<Mix_Chunk*> samples;
    std::vector<Mix_Music*> tracks;
};