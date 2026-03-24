#include <iostream>
#include <fstream>
#include <vector>
#include <SDL3/SDL_main.h>
#include "Core.h"
#include "Platform.h"

// 从文件读取 ROM 数据
std::vector<uint8_t> loadRomFromFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        SDL_Log("无法打开 ROM 文件: ");
        SDL_Log(path);
        SDL_Log("\n");
        return {};
    }

    // 获取文件大小
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // 读取数据
    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);

    SDL_Log("已加载 ROM: ");
    SDL_Log(path);
    std::cout << "已加载 ROM: " << path << " (" << size << " 字节)" << std::endl;
    return data;
}

int SDL_main(int argc, char* argv[]) {
    // 检查参数
    //const char* romPath = (argc > 1) ? argv[1] : "Bowling [Gooitzen van der Wal].ch8";

    // 初始化平台
    chip8::Platform platform;
    if (!platform.init(10)) {  // 放大10倍: 640x320
        //std::cerr << "平台初始化失败" << std::endl;
        SDL_Log("平台初始化失败");
        return 1;
    }

    // 初始化 CPU
    chip8::Chip8Cpu cpu;

    // 加载 ROM
    std::vector<uint8_t> romData = loadRomFromFile("test/Pong (1 player).ch8");
    if (romData.empty()) {
        platform.shutdown();
        return 1;
    }
    cpu.loadRom(romData);

    // 主循环
    bool running = true;
    constexpr int kCyclePerFrame = 10;  // 每帧执行多少个 CPU 周期

    while (running) {
        // 处理事件
        platform.pollEvents(running, cpu);

        // 执行多个 CPU 周期
        for (int i = 0; i < kCyclePerFrame; ++i) {
            cpu.step();
        }

        // 渲染
        platform.render(cpu.getDisplay(), cpu.isDisplayDirty());

        // 延迟以控制帧率（60 FPS）
        SDL_Delay(1000 / 60);
    }

    // 清理
    platform.shutdown();

    return 0;
}
