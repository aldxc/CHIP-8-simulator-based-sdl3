# Contributing to CHIP-8-simulator

感谢你对 CHIP-8-simulator 项目的贡献。本文件定义了项目的编码标准、代码审查与合并流程、分支与提交规范、测试要求以及常见开发流程。请在提出拉取请求(PR)或提交补丁前认真阅读。

## 项目概述

CHIP-8-simulator 是基于 C++14 和 SDL3 的 CHIP-8 虚拟机与模拟器实现。目标是可读、易维护、跨平台与可测试的代码库。

## 开发前提

- 使用 Visual Studio 2022（Windows）作为主要开发环境，但代码应保持可跨平台构建（Linux/macOS）
- 目标语言：C++14
- 使用 SDL3 作为图形与输入库
- 所有更改必须遵循本贡献指南和项目中的 .editorconfig 文件（如果存在）

## 分支与工作流

- 主分支: `main` - 始终保持可构建、可运行的状态。仅允许通过 PR 合并经过审查的更改。
- 功能分支: `feature/<描述>`，例如 `feature/rendering-scaling`。
- 修复分支: `fix/<issue-number>-<描述>`，例如 `fix/42-input-issue`。
- 发布分支: `release/<version>`（仅在需要准备发布时创建）。
- PR 标题应简洁，格式为: `<范围>: <简短描述>`（范围可选，例如 `emulator`, `ui`, `build`）。

## 提交消息规范

- 使用现在时态描述变更（例如: "修复字体渲染错误" 而非 "修复了..."）。
- 标题长度不超过 72 个字符；正文（如有）每行 72 字符。
- 若关联 issue，请在正文尾部添加 `Closes #<issue-number>`。

示例:

```
emulator: 修复 VBlank 定时器重置

修复描述的详细说明，为什么需要这个更改，影响范围。

Closes #12
```

## 代码风格与约定

所有源文件必须遵循项目的 .editorconfig（如果存在）和以下原则：

- 缩进使用 4 个空格，不使用制表符。
- 文件以 Unix 换行符 (LF) 结尾，并在文件末尾保留单个换行。
- 文件和目录命名使用小写、短横线分隔（例如: `chip8-core`、`display-driver`），C++ 类型与标识符遵循下列约定。

命名约定（C++）:
- 类型（类、struct、enum）：PascalCase，例如 `Chip8Cpu`、`DisplayDriver`。
- 变量、函数、参数：camelCase，例如 `memoryBuffer`、`fetchInstruction()`。
- 常量（constexpr 或 const）：k 前缀 + PascalCase，例如 `kDisplayWidth`、`kFontSetSize`。
- 私有成员变量使用后缀 `_`（下划线），例如 `registers_`。
- 命名空间使用小写并按功能分组，例如 `chip8::emulator`。

其他约定:
- 每个头文件必须包含 include guard（或使用 `#pragma once`）。
- 头文件 (.h/.hpp) 只暴露必要接口；实现应放在 .cpp 文件中。
- 遵循 RAII 原则，优先使用智能指针（`std::unique_ptr`、`std::shared_ptr`）和容器。
- 避免在头文件中定义非内联的全局变量。
- 在可能的地方使用 `constexpr` 和 `noexcept` 增强可读性和性能。

## 构建与依赖

- 首选使用 CMake 以便跨平台支持。项目中应包含 `CMakeLists.txt`（如果还未提供，请在 PR 中添加）。
- 在 Windows/Visual Studio 环境中：支持通过生成 .sln/.vcxproj（CMake 可生成）进行构建。
- 依赖管理：尽量使用系统或包管理器提供的 SDL3（例如 vcpkg、conan），并在 README 中记录如何获取依赖。
- 构建类型：Debug、Release。Release 用于发布。

## 测试要求

- 编写单元测试以覆盖核心逻辑（例如：CPU 解码、定时器、内存映射、按键处理和显示缓冲更新）。
- 推荐使用 Catch2、GoogleTest 或类似框架。单元测试应通过 CI 自动运行。
- 在 PR 中包含新增测试用例，且所有测试必须通过。

## 代码审查标准

- PR 必须包含描述更改意图的说明、如何测试以及相关截图或日志（若适用）。
- 审查者应检查：代码样式、正确性、性能、边界条件、资源管理、可测试性以及是否包含充分的测试。
- 遇到重大设计分歧时，请在 PR 中发起讨论并考虑编写 RFC 样式的草案。

## 性能与资源管理

- 显示渲染应尽量使用纹理与 GPU 加速路径（SDL3 API），避免每帧以像素调用绘制。
- 按 CHIP-8 行为模拟器的节拍器（时钟）以固定步长运行，渲染可以使用不同频率。
- 明确所有资源的生命周期（窗口、渲染器、纹理、音频设备等），并在适当位置释放。

## 调试与日志

- 使用可选的日志设施（例如基于宏或小型日志库），只在调试构建记录详细信息，Release 构建限制日志级别。
- 在关键路径（例如指令解码、内存越界、未定义指令）添加诊断日志，但默认情况下应保持安静。

## 跨平台注意事项

- SDL3 提供跨平台窗口/输入/音频支持，但路径、文件系统、字节序和行尾等方面需注意平台差异。
- 测试至少在 Windows 和 Linux 上验证基本功能。

## 安全与许可

- 保持第三方依赖合规并记录在 `LICENSE` 或 `THIRD-PARTY.md` 文件中。
- 避免将敏感信息或大二进制文件提交到仓库。

## CI / 持续集成

- 建议在 GitHub Actions 或其他 CI 上运行：
  - CMake 配置与构建（Debug/Release）
  - 单元测试
  - 静态分析（例如 clang-tidy、cppcheck）
  - 代码格式检查（基于 clang-format 或 .editorconfig 验证工具）

## 提交 PR 的检查清单

- [ ] 构建通过（Debug/Release）
- [ ] 所有单元测试通过
- [ ] 代码风格符合项目规范
- [ ] 包含/更新必要的文档（README、注释等）
- [ ] 变更经过至少一名审查者认可

## 联系与支持

在提交问题或 PR 时请提供重现步骤、日志与相关上下文。

---
感谢你的贡献！