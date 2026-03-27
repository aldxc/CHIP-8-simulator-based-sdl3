#pragma once
#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <memory>
#include "Core.h"

namespace chip8 {

struct SDLWindowDeleter {
    void operator()(SDL_Window* window) const { SDL_DestroyWindow(window); }
};

struct SDLRendererDeleter {
    void operator()(SDL_Renderer* renderer) const { SDL_DestroyRenderer(renderer); }
};

struct SDLTextureDeleter {
    void operator()(SDL_Texture* texture) const { SDL_DestroyTexture(texture); }
};

class Platform {
public:
    Platform() = default;

    ~Platform();

    Platform(const Platform&) = delete;
    Platform& operator=(const Platform&) = delete;
    Platform(Platform&&) = delete;
    Platform& operator=(Platform&&) = delete;

    bool init(int scale = 10);

    void shutdown();

    void pollEvents(bool& running, Chip8Cpu& cpu);

    void render(const std::array<bool, kDisplayWidth * kDisplayHeight>& display, bool displayDirty);

    void clear();

private:
    std::unique_ptr<SDL_Window, SDLWindowDeleter> window_;
    std::unique_ptr<SDL_Renderer, SDLRendererDeleter> renderer_;
    std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture_;

    int windowWidth_ = 0;
    int windowHeight_ = 0;
    int scale_ = 1;

    int mapKey(SDL_Scancode key) const;
};

} // namespace chip8
