# CHIP-8 Simulator
# CHIP-8 Simulator

这是一个用 `C++17` 和 `SDL3` 写的简单 `CHIP-8` 模拟器。代码结构比较直接，适合拿来学习、调试，或者继续扩展功能。

## 项目结构

- `Core.h` / `core.cpp`：`Chip8Cpu`，负责内存、寄存器、指令执行、显示缓冲和按键状态。
- `Platform.h` / `platform.cpp`：`Platform`，负责 SDL 初始化、窗口渲染和键盘映射。
- `main.cpp`：程序入口，负责加载 ROM、处理主循环、输入和渲染。
- `CMakeLists.txt`：构建配置。

## 当前实现

- ROM 从命令行参数读取。
- ROM 会加载到 `0x200` 起始地址。
- 主循环里每帧执行固定次数的 CPU 周期。
- 画面按 `60 FPS` 刷新。
- 按下 `Esc` 可以退出程序。

## 键位映射

默认键盘布局如下：

```text
1 2 3 4    -> 1 2 3 C
Q W E R    -> 4 5 6 D
A S D F    -> 7 8 9 E
Z X C V    -> A 0 B F
```

如果想调整映射，可以修改 `Platform::mapKey`。

## 构建

### 依赖

- 支持 `C++17` 的编译器
- `SDL3`
- `CMake 3.16+`

### 命令

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

项目会优先尝试使用 `SDL3` 的 CMake 包；如果找不到，再回退到 `pkg-config`。

## 运行

```bash
./chip8-simulator path/to/rom.ch8
```

Windows 下也是一样，把 ROM 路径作为启动参数传进去即可。

## 代码说明

- `Platform` 使用 `std::unique_ptr` 和自定义删除器管理 SDL 资源。
- 显示分辨率固定为 `64 x 32`。
- 默认缩放倍数是 `10`，窗口大小对应 `640 x 320`。
- 只有显示缓冲变化时才会触发渲染更新。

## 后续可扩展方向

- 声音输出
- 更完整的指令兼容性
- 命令行参数，例如缩放倍数、CPU 频率
- 调试功能，比如寄存器和内存查看

## 许可

项目使用 `MIT` 许可证，见 `LICENSE`。
