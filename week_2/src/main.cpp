#include "engine.hpp"

SDL_AppResult SDL_AppInit(void** engine_pp, int, char**) {
    *engine_pp = new Engine();
    return SDL_AppResult::SDL_APP_CONTINUE;
}
SDL_AppResult SDL_AppIterate(void* engine_p) {
    return static_cast<Engine*>(engine_p)->handle_sdl_frame();
}
SDL_AppResult SDL_AppEvent(void* engine_p, SDL_Event* event_p) {
    return static_cast<Engine*>(engine_p)->handle_sdl_event(*event_p);
}
void SDL_AppQuit(void* engine_p, SDL_AppResult) {
    delete static_cast<Engine*>(engine_p);
}