#ifndef PLATFORM_H
#define PLATFORM_H
#include <SDL.h>
#include <cstdint>

class Platform {
public:
    Platform(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();

    void update(const uint32_t* buffer);
    bool processInput(uint8_t* keys);
    void tickTimers(uint8_t& delay_timer, uint8_t& sound_timer);

private:
    SDL_Window*   window  = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture*  texture  = nullptr;

    uint32_t lastTimerTick = 0;
};
#endif
