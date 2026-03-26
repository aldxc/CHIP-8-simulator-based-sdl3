# CHIP-8-simulator

简体中文说明

## 项目简介

`CHIP-8-simulator` 是一个使用 C++17 与 SDL3 实现的最小化 CHIP-8 模拟器示例工程。包含一个遵循 CHIP-8 行为的 CPU 内核 `Chip8Cpu` 与一个基于 SDL3 的平台封装 `Platform`（窗口、渲染、输入）。

本仓库适合作为学习、调试与扩展 CHIP-8 功能的基础实现。

## 主要组件

- `Chip8Cpu`：实现内存、寄存器、常用指令子集、显示缓冲与按键处理。
- `Platform`：封装 SDL3 初始化、窗口/渲染与键盘映射，负责把 `Chip8Cpu` 的显示缓冲输出到屏幕。
- `main.cpp`：示例主循环，演示 ROM 加载、事件处理、CPU 周期推进与渲染整合。

## 命名规则

为了代码一致性与可维护性，遵循以下约定：

- 类型/类/struct/enum 使用 PascalCase（例如 `Chip8Cpu`、`DisplayDriver`）。
- 方法与变量使用 camelCase（例如 `memoryBuffer`、`fetchOpcode()`）。
- 常量使用 `k` 前缀且为 PascalCase（例如 `kDisplayWidth`、`kFontSetSize`）。
- 私有成员使用尾缀下划线 `_`（例如 `registers_`、`pc_`）。
- 头文件使用 `#pragma once`，并避免在头文件中放置不必要的实现代码。

以上规则旨在保持代码风格一致。

## 构建依赖

- C++17 编译器
- SDL3 开发库（头文件与运行时库）
- CMake（推荐用于跨平台构建）

仓库根目录提供了 `CMakeLists.txt`，可用如下命令构建：

```
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

（在某些平台上需要指定 SDL3 的安装位置或使用包管理器安装 SDL3）

## 附带的 SDL3 库与示例 ROM

本仓库包含与 SDL3 相关的头文件/预编译库（例如 `include/SDL3` 下的头文件）。仓库中可能还包含若干示例 CHIP-8 ROM 文件用于测试（若存在，通常放在 `roms/` 或 `examples/roms/` 目录）。

- 如果要使用仓库内的 SDL3 运行时，CMake 配置会优先使用仓库提供的头文件/库（Windows 下常见）。
- 如果你在系统上已安装 SDL3，推荐使用系统安装版本来避免重复库，方法是修改或清理仓库内的本地库并让 CMake 查找系统 SDL3（查看 `CMakeLists.txt` 中的查找逻辑）。

## 运行说明

编译后生成可执行文件 `chip8-simulator`，运行时可以传入 ROM 路径作为第一个参数：

```
./chip8-simulator path/to/rom.ch8
```

默认行为：窗口像素放大倍数为 10（可在 `main.cpp` 中调整 `Platform::initialize` 的参数）。

## 键位映射（默认）

默认将键盘映射为常见 CHIP-8 布局：

- 1 2 3 4 -> CHIP-8: 1 2 3 C
- Q W E R -> CHIP-8: 4 5 6 D
- A S D F -> CHIP-8: 7 8 9 E
- Z X C V -> CHIP-8: A 0 B F

映射可在 `Platform::mapKey` 中自定义。

## 使用要点

- 将 ROM 文件读取为 `std::vector<uint8_t>` 并通过 `Chip8Cpu::loadRom` 加载（地址从 `0x200` 开始）。
- 在主循环中调用 `Chip8Cpu::step()` 执行 CPU 周期，且以约 60Hz 更新定时器（示例主循环已包含简单时序控制）。
- 渲染：当 `Chip8Cpu::isDisplayDirty()` 为真时调用 `Platform::render(cpu.getDisplay(), true)`，渲染后调用 `cpu.clearDisplayDirty()`。
- 按键事件通过 `Platform::pollEvents(running, cpu)` 传递到 `cpu.setKey(key, pressed)`。

## 扩展建议

- 添加声音支持（当 `soundTimer_` 大于 0 时播放音频）。
- 改进渲染（像素边缘、缩放过滤、硬件加速）。
- 完整实现所有 CHIP-8 指令及部分 Hires/延伸变体（SCHIP）。

## 许可

本项目采用 MIT 许可，详情见 `LICENSE` 文件。

---

如需我帮助：
- 把 README 翻译为英文并保留中文版，或
- 为 `main.cpp` 添加更多命令行选项（scale、cpu frequency、调试模式等），或
- 运行一次构建并修复可能的编译错误。
