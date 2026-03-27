#include <iostream>
#include <fstream>
#include <iostream>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Core.h"
#include "Platform.h"

std::vector<uint8_t> loadRomFromFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        SDL_Log("无法打开 ROM 文件: %s", path);
        return {};
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    if (!file) {
        SDL_Log("读取 ROM 失败: %s", path);
        return {};
    }

    SDL_Log("已加载 ROM: %s", path);
    std::cout << "已加载 ROM: " << path << " (" << size << " 字节)" << std::endl;
    return data;
}

int SDL_main(int argc, char* argv[]) {
    if (argc < 2) {
        SDL_Log("用法: chip8-simulator <path-to-rom>");
        return 1;
    }

    chip8::Platform platform;
    if (!platform.init(10)) {  // 放大10倍: 640x320
        SDL_Log("平台初始化失败");
        return 1;
    }

    chip8::Chip8Cpu cpu;

    // 加载 ROM
    std::vector<uint8_t> romData = loadRomFromFile("test/Pong (1 player).ch8");
    if (romData.empty()) {
        return 1;
    }

    cpu.loadRom(romData);

    bool running = true;
    constexpr int kCyclePerFrame = 10;

    while (running) {
        // 处理事件
        platform.pollEvents(running, cpu);

        // 执行多个 CPU 周期
        for (int i = 0; i < kCyclePerFrame; ++i) {
            cpu.step();
        }

        const bool displayDirty = cpu.isDisplayDirty();
        platform.render(cpu.getDisplay(), displayDirty);
        if (displayDirty) {
            cpu.clearDisplayDirty();
        }

        // 延迟以控制帧率（60 FPS）
        SDL_Delay(1000 / 60);
    }

    // 清理
    platform.shutdown();

    return 0;
}
