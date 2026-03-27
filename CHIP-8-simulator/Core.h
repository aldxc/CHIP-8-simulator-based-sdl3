#pragma once
#pragma once

#include <vector>
#include <array>
#include <cstdint>

namespace chip8 {

	static constexpr int kDisplayWidth = 64;
	static constexpr int kDisplayHeight = 32;

	class Chip8Cpu {
	private:
		static constexpr int kMemorySize = 4096;
		static constexpr int kRegisterNum = 16;
		static constexpr int kStackDepth = 16;
     static constexpr int kMaxKeyBoard = 16;

		std::array<uint8_t, kMemorySize> memory_;
		uint16_t pc_;
		std::array<uint8_t, kRegisterNum> registers_;
		uint16_t indexRegister_;
		std::array<uint16_t, kStackDepth> stack_;
		uint8_t stackPointer_;

        uint8_t delayTimer_;
		uint8_t soundTimer_;

		std::array<bool, kDisplayWidth * kDisplayHeight> display_;
		std::array<bool, kMaxKeyBoard> keypad_;

      uint16_t fetchOpcode();
		void execute(uint16_t opcode);

        bool displayDirty_;
		bool waitingForKey_;
		uint8_t waitingRegister_;

       // Built-in font set stored at 0x50.
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

		void reset();

		void loadRom(const std::vector<uint8_t>& data);

		void step();

		void setKey(uint8_t key, bool pressed);

		const std::array<bool, kDisplayWidth * kDisplayHeight>& getDisplay() const { return display_; }
		bool isDisplayDirty() const { return displayDirty_; }
		void clearDisplayDirty() { displayDirty_ = false; }
		bool isWaitingForKey() const { return waitingForKey_; }
	};

} // namespace chip8
