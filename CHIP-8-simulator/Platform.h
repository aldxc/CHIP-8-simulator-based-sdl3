#pragma once
#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <SDL3/SDL.h>
#include "Core.h"
#include <array>

namespace chip8 {

// 平台抽象：负责窗口、渲染与输入处理
class Platform {
public:
    Platform();
    ~Platform();

    // 初始化 SDL、窗口和渲染器，scale 为像素放大倍数
    bool initialize(int scale = 10);

    // 关闭并释放资源
    void shutdown();

    // 处理 SDL 事件：修改 running 状态并将按键事件传递给 CPU
    void pollEvents(bool& running, Chip8Cpu& cpu);

    // 将芯片 8 的显示缓冲渲染到窗口，displayDirty 为 true 时才会更新纹理
    void render(const std::array<bool, kDisplayWidth * kDisplayHeight>& display, bool displayDirty);

    // 清空屏幕（同时标记为脏）
    void clear();

private:
    SDL_Window* window_ = nullptr; // 窗口句柄
    SDL_Renderer* renderer_ = nullptr; // 渲染器
    SDL_Texture* texture_ = nullptr; // 用于像素显示的纹理

    int windowWidth_ = 0; // 窗口宽度（像素）
    int windowHeight_ = 0; // 窗口高度（像素）
    int scale_ = 1; // 每个 CHIP-8 像素放大倍数

    // 将 SDL 键值映射为 CHIP-8 键（0x0 - 0xF），若未映射返回 -1
    int mapKey(SDL_Keycode key) const;
};

} // namespace chip8

#endif // PLATFORM_H_