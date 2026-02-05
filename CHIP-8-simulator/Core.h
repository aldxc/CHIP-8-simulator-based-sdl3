#pragma once
#include <vector>
#include <array>
#include <cstdint>

namespace chip8 {

	static constexpr int kDisplayWidth = 64;
	static constexpr int kDisplayHeight = 32;

	// Chip8 CPU 核心类
	class Chip8Cpu {
	private:
		static constexpr int kMemorySize = 4096;
		static constexpr int kRegisterNum = 16;
		static constexpr int kStackDepth = 16;
		static constexpr int kMaxKeyBoard = 16;

		std::array<uint8_t, kMemorySize> memory_; // 内存缓冲区
		uint16_t pc_; // 程序计数器
		std::array<uint8_t, kRegisterNum> registers_; // 8 位寄存器 V0-VF
		uint16_t indexRegister_; // 索引寄存器 I
		std::array<uint16_t, kStackDepth> stack_; // 调用栈
		uint8_t stackPointer_; // 栈指针

		// 定时器（以 60Hz 频率递减）
		uint8_t delayTimer_;
		uint8_t soundTimer_;

		std::array<bool, kDisplayWidth * kDisplayHeight> display_; // 显示缓冲（像素开/关）
		std::array<bool, kMaxKeyBoard> keypad_; // 键盘状态映射

		// 指令执行相关
		uint16_t fetchOpcode(); // 从内存取指
		void execute(uint16_t opcode); // 执行指令

		// 状态标志
		bool displayDirty_; // 显示是否被修改
		bool waitingForKey_; // 是否正在等待按键
		uint8_t waitingRegister_; // 等待按键时要写入的寄存器索引

		// 字体数据（每个字模 5 字节，共 16 个数字）
		static constexpr std::array<uint8_t, 80> fontset_ = {{
			0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70,
			0xF0, 0x10, 0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0,
			0x90, 0x90, 0xF0, 0x10, 0x10, 0xF0, 0x80, 0xF0, 0x10, 0xF0,
			0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0, 0x10, 0x20, 0x40, 0x40,
			0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0, 0x10, 0xF0,
			0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0,
			0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0,
			0xF0, 0x80, 0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80
		}};

	public:
		Chip8Cpu();

		// 重置 CPU 状态
		void reset();

		// 将 ROM 数据加载到内存（从 0x200 开始）
		void loadRom(const std::vector<uint8_t>& data);

		// 执行一个仿真周期（取指->执行->更新定时器）
		void step();

		// 键盘输入
		void setKey(uint8_t key, bool pressed);

		// 显示访问器
		const std::array<bool, kDisplayWidth * kDisplayHeight>& getDisplay() const { return display_; }
		bool isDisplayDirty() const { return displayDirty_; }
		void clearDisplayDirty() { displayDirty_ = false; }
		bool isWaitingForKey() const { return waitingForKey_; }
	};

} // namespace chip8
