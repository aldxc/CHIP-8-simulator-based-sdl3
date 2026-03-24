#include "Platform.h"
#include <vector>
#include <cstring>

namespace chip8 {

//Platform::Platform() = default;

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

    // 创建窗口
    window_ = SDL_CreateWindow("CHIP-8", windowWidth_, windowHeight_, 0);
    if (!window_) {
        SDL_Quit();
        return false;
    }

    // 创建渲染器
    renderer_ = SDL_CreateRenderer(window_, nullptr);
    //renderer_ = SDL_CreateRenderer(window_, -1, 0);
    if (!renderer_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        SDL_Quit();
        return false;
    }

    // 创建纹理，纹理大小为 CHIP-8 原始分辨率（64x32），渲染时拉伸
    texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, kDisplayWidth, kDisplayHeight);
    if (!texture_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        SDL_Quit();
        return false;
    }

    return true;
}

void Platform::shutdown() {
    if (texture_) {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

int Platform::mapKey(SDL_Scancode key) const {
    // 常见键位映射：
    // 键盘布局:
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
            // 处理退出快捷键 Esc
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

    // 像素缓冲（RGBA8888）
    std::vector<uint32_t> pixels(kDisplayWidth * kDisplayHeight);
    for (int i = 0; i < kDisplayWidth * kDisplayHeight; ++i) {
        if (display[i]) {
            // 白色像素
            pixels[i] = 0xFFFFFFFFu;
        } else {
            // 黑色像素
            pixels[i] = 0xFF000000u;
        }
    }

    // 更新纹理数据
    SDL_UpdateTexture(texture_, nullptr, pixels.data(), kDisplayWidth * sizeof(uint32_t));

    // 清除渲染目标并复制纹理（拉伸到窗口）
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    //SDL_FRect dstRect = { 0, 0, windowWidth_, windowHeight_ };
    SDL_RenderTexture(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}

void Platform::clear() {
    if (!renderer_)
        return;
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);
}

} // namespace chip8
