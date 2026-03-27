#include "Platform.h"
#include "Platform.h"

#include <vector>

namespace chip8 {

Platform::~Platform() {
    shutdown();
}

bool Platform::init(int scale) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        return false;
    }

    scale_ = scale > 0 ? scale : 1;
    windowWidth_ = kDisplayWidth * scale_;
    windowHeight_ = kDisplayHeight * scale_;

    window_.reset(SDL_CreateWindow("CHIP-8", windowWidth_, windowHeight_, 0));
    if (!window_) {
        SDL_Quit();
        return false;
    }

    renderer_.reset(SDL_CreateRenderer(window_.get(), nullptr));
    if (!renderer_) {
        window_.reset();
        SDL_Quit();
        return false;
    }

    texture_.reset(SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, kDisplayWidth, kDisplayHeight));
    if (!texture_) {
        renderer_.reset();
        window_.reset();
        SDL_Quit();
        return false;
    }

    return true;
}

void Platform::shutdown() {
    texture_.reset();
    renderer_.reset();
    window_.reset();
    SDL_Quit();
}

int Platform::mapKey(SDL_Scancode key) const {
    // Keyboard layout:
    // 1 2 3 4    -> CHIP-8 1 2 3 C
    // Q W E R    -> CHIP-8 4 5 6 D
    // A S D F    -> CHIP-8 7 8 9 E
    // Z X C V    -> CHIP-8 A 0 B F
    switch (key) {
    case SDL_SCANCODE_1: return 0x1;
    case SDL_SCANCODE_2: return 0x2;
    case SDL_SCANCODE_3: return 0x3;
    case SDL_SCANCODE_4: return 0xC;

    case SDL_SCANCODE_Q: return 0x4;
    case SDL_SCANCODE_W: return 0x5;
    case SDL_SCANCODE_E: return 0x6;
    case SDL_SCANCODE_R: return 0xD;

    case SDL_SCANCODE_A: return 0x7;
    case SDL_SCANCODE_S: return 0x8;
    case SDL_SCANCODE_D: return 0x9;
    case SDL_SCANCODE_F: return 0xE;

    case SDL_SCANCODE_Z: return 0xA;
    case SDL_SCANCODE_X: return 0x0;
    case SDL_SCANCODE_C: return 0xB;
    case SDL_SCANCODE_V: return 0xF;

    default:
        return -1;
    }
}

void Platform::pollEvents(bool& running, Chip8Cpu& cpu) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
            return;
        }
        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
            bool pressed = (event.type == SDL_EVENT_KEY_DOWN);

            SDL_Scancode key = event.key.scancode;
            int mapped = mapKey(key);
            if (mapped >= 0) {
                cpu.setKey(static_cast<uint8_t>(mapped), pressed);
            }
            if (key == SDL_SCANCODE_ESCAPE && pressed) {
                running = false;
                return;
            }
        }
    }
}

void Platform::render(const std::array<bool, kDisplayWidth * kDisplayHeight>& display, bool displayDirty) {
    if (!renderer_ || !texture_)
        return;

    if (!displayDirty)
        return;

    std::vector<uint32_t> pixels(kDisplayWidth * kDisplayHeight);
    for (int i = 0; i < kDisplayWidth * kDisplayHeight; ++i) {
        pixels[i] = display[i] ? 0xFFFFFFFFu : 0xFF000000u;
    }

    SDL_UpdateTexture(texture_.get(), nullptr, pixels.data(), kDisplayWidth * sizeof(uint32_t));

    SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer_.get());

    SDL_RenderTexture(renderer_.get(), texture_.get(), nullptr, nullptr);
    SDL_RenderPresent(renderer_.get());
}

void Platform::clear() {
    if (!renderer_)
        return;
    SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer_.get());
    SDL_RenderPresent(renderer_.get());
}

} // namespace chip8
