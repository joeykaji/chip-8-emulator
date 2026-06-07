#include "platform.hpp"
#include <stdexcept>
#include <algorithm>

Platform::Platform(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight){
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        throw std::runtime_error(SDL_GetError());

    window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if(!window) throw std::runtime_error(SDL_GetError());

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) throw std::runtime_error(SDL_GetError());

    // CHIP-8 pixels are ARGB8888 — each uint32_t is one pixel

    texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_ARGB8888,  // back to ARGB
    SDL_TEXTUREACCESS_STREAMING,
    64, 32);
    if(!texture) throw std::runtime_error(SDL_GetError());

    lastTimerTick = SDL_GetTicks();
}

Platform::~Platform(){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::update(const uint32_t* buffer){
    SDL_UpdateTexture(texture, nullptr, buffer, 64 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Platform::processInput(uint8_t* keys){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        if(e.type == SDL_QUIT) return false;
        if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) return false;

        bool pressed = (e.type == SDL_KEYDOWN);
        if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
            // CHIP-8 keypad:   mapped to keyboard:
            // 1 2 3 C           1 2 3 4
            // 4 5 6 D           Q W E R
            // 7 8 9 E           A S D F
            // A 0 B F           Z X C V
            switch(e.key.keysym.sym){
                case SDLK_1: keys[0x1] = pressed; break;
                case SDLK_2: keys[0x2] = pressed; break;
                case SDLK_3: keys[0x3] = pressed; break;
                case SDLK_4: keys[0xC] = pressed; break;
                case SDLK_q: keys[0x4] = pressed; break;
                case SDLK_w: keys[0x5] = pressed; break;
                case SDLK_e: keys[0x6] = pressed; break;
                case SDLK_r: keys[0xD] = pressed; break;
                case SDLK_a: keys[0x7] = pressed; break;
                case SDLK_s: keys[0x8] = pressed; break;
                case SDLK_d: keys[0x9] = pressed; break;
                case SDLK_f: keys[0xE] = pressed; break;
                case SDLK_z: keys[0xA] = pressed; break;
                case SDLK_x: keys[0x0] = pressed; break;
                case SDLK_c: keys[0xB] = pressed; break;
                case SDLK_v: keys[0xF] = pressed; break;
            }
        }
    }
    return true;
}

void Platform::tickTimers(uint8_t& delay_timer, uint8_t& sound_timer){
    // decrement timers at 60Hz independently of CPU cycle rate
    uint32_t now = SDL_GetTicks();
    uint32_t elapsed = now - lastTimerTick;
    uint32_t ticks = elapsed / (1000 / 60); // how many 60Hz ticks have passed

    if(ticks > 0){
        lastTimerTick += ticks * (1000 / 60);
        if(delay_timer > 0)
            delay_timer -= std::min((uint8_t)ticks, delay_timer);
        if(sound_timer > 0)
            sound_timer -= std::min((uint8_t)ticks, sound_timer);
    }
}
