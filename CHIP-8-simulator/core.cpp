#include "Core.h"
#include <cstring>
#include <random>

namespace chip8 {
	// 构造函数：初始化并重置状态
	Chip8Cpu::Chip8Cpu() {
		reset();
	}

	// 重置所有寄存器、内存和状态
	void Chip8Cpu::reset() {
		memory_.fill(0);
		registers_.fill(0);
		stack_.fill(0);
		indexRegister_ = 0;
		pc_ = 0x200;
		stackPointer_ = 0;
		delayTimer_ = 0;
		soundTimer_ = 0;
		keypad_.fill(0);
		display_.fill(0);
		displayDirty_ = false;
		waitingForKey_ = false;
		waitingRegister_ = 0;

		// 将字体数据加载到内存常用位置 0x50
		std::memcpy(memory_.data() + 0x50, fontset_.data(), fontset_.size());
	}

	// 将 ROM 数据复制到内存的 0x200 区
	void Chip8Cpu::loadRom(const std::vector<uint8_t>& data) {
		reset();
		if (data.size() + 0x200 > memory_.size()) {
			// 超出内存大小则截断
			std::memcpy(memory_.data() + 0x200, data.data(), memory_.size() - 0x200);
		} else {
			std::memcpy(memory_.data() + 0x200, data.data(), data.size());
		}
	}

	// 从当前 PC 读取两个字节构成操作码
	uint16_t Chip8Cpu::fetchOpcode() {
		uint16_t hi = memory_[pc_];
		uint16_t lo = memory_[pc_ + 1];
		return (hi << 8) | lo;
	}

	// 更新键盘状态；如果正在等待按键且按下，则写入寄存器并继续
	void Chip8Cpu::setKey(uint8_t key, bool pressed) {
		if (key < keypad_.size()) {
			keypad_[key] = pressed;
			if (waitingForKey_ && pressed) {
				waitingForKey_ = false;
				registers_[waitingRegister_] = key;
			}
		}
	}

	// 执行一个 CPU 周期（取指、执行、更新定时器）
	void Chip8Cpu::step() {
		if (waitingForKey_) {
			// 等待按键时不执行指令，但仍更新定时器
			if (delayTimer_ > 0) --delayTimer_;
			if (soundTimer_ > 0) --soundTimer_;
			return;
		}

		uint16_t opcode = fetchOpcode();
		execute(opcode);

		// 在每次 step 结束时更新定时器（调用者应以 60Hz 频率调用以模拟真实定时器）
		if (delayTimer_ > 0) --delayTimer_;
		if (soundTimer_ > 0) --soundTimer_;
	}

	// 执行操作码的实现（实现常用子集）
	void Chip8Cpu::execute(uint16_t opcode) {
		// 默认 PC 增量为 2 字节
		uint16_t pcIncrement = 2;
		
        auto nnn = opcode & 0x0FFF;
		auto n = opcode & 0x000F;
		auto x = (opcode & 0x0F00) >> 8;
		auto y = (opcode & 0x00F0) >> 4;
		auto kk = opcode & 0x00FF;

		switch (opcode & 0xF000) {
		case 0x0000:
			switch (opcode) {
			case 0x00E0: // CLS: 清屏 -- 0x00E0
				display_.fill(false);
				displayDirty_ = true;
				break;
			case 0x00EE: // RET: 从子例程返回 -- 0x00EE
				if (stackPointer_ > 0) {
					--stackPointer_;
					pc_ = stack_[stackPointer_];
					pcIncrement = 0;
				}
				break;
			default:
				// 0NNN (SYS) 忽略
				break;
			}
			break;
		case 0x1000: // JP addr: 跳转
			pc_ = nnn;
			pcIncrement = 0;
			break;
		case 0x2000: // CALL addr: 调用子例程
			if (stackPointer_ < stack_.size()) {
				stack_[stackPointer_] = pc_ + 2;
				++stackPointer_;
				pc_ = nnn;
				pcIncrement = 0;
			}
			break;
		case 0x3000: // SE Vx, byte: 如果相等则跳过
			if (registers_[x] == kk) pcIncrement = 4;
			break;
		case 0x4000: // SNE Vx, byte: 如果不等则跳过
			if (registers_[x] != kk) pcIncrement = 4;
			break;
		case 0x5000: // SE Vx, Vy : 如果寄存器相等则跳过下一条指令
			if ((opcode & 0x000F) == 0) {
				if (registers_[x] == registers_[y]) pcIncrement = 4;
			}
			break;
		case 0x6000: // LD Vx, byte : 将立即数加载到寄存器 Vx
			registers_[x] = kk;
			break;
		case 0x7000: // ADD Vx, byte : 寄存器 Vx 加上立即数
			registers_[x] = uint8_t(registers_[x] + kk);
			break;
		case 0x8000:
			switch (opcode & 0x000F) {
			case 0x0: // LD Vx, Vy : 将寄存器 Vy 的值复制到 Vx
				registers_[x] = registers_[y];
				break;
			case 0x1: // OR Vx, Vy : Vx 与 Vy 进行或运算，并存入Vx
				registers_[x] |= registers_[y];
				break;
			case 0x2: // AND Vx, Vy : Vx 与 Vy 进行与运算，并存入Vx
				registers_[x] &= registers_[y];
				break;
			case 0x3: // XOR Vx, Vy : Vx 与 Vy 进行异或运算，并存入Vx
				registers_[x] ^= registers_[y];
				break;
			case 0x4: { // ADD Vx, Vy 并设置 VF 进位
				uint16_t sum = registers_[x] + registers_[y];
				registers_[0xF] = sum > 0xFF ? 1 : 0;
				registers_[x] = uint8_t(sum & 0xFF);
			}
				break;
			case 0x5: // SUB Vx, Vy : Vx 减 Vy，VF 存放借位（Vx > Vy 时为 1）
				registers_[0xF] = registers_[x] > registers_[y] ? 1 : 0;
				registers_[x] = uint8_t(registers_[x] - registers_[y]);
				break;
			case 0x6: // SHR Vx: Vx 右移一位，VF 存放最低位
				registers_[0xF] = registers_[x] & 0x1;
				registers_[x] >>= 1;
				break;
			case 0x7: // SUBN Vx, Vy: Vy 减 Vx，VF 存放借位（Vy > Vx 时为 1）
				registers_[0xF] = registers_[y] > registers_[x] ? 1 : 0;
				registers_[x] = uint8_t(registers_[y] - registers_[x]);
				break;
			case 0xE: // SHL Vx: Vx 左移一位，VF 存放最高位
				registers_[0xF] = (registers_[x] & 0x80) >> 7;
				registers_[x] <<= 1;
				break;
			}
			break;
		case 0x9000: // SNE Vx, Vy : 如果寄存器不等则跳过下一条指令
			if ((opcode & 0x000F) == 0) {
				if (registers_[x] != registers_[y]) pcIncrement = 4;
			}
			break;
		case 0xA000: // LD I, addr : 将地址加载到索引寄存器 I
			indexRegister_ = nnn;
			break;
		case 0xB000: // JP V0, addr : 跳转到地址 nnn + V0
			pc_ = nnn + registers_[0];
			pcIncrement = 0;
			break;
		case 0xC000: { // RND Vx, byte : 生成随机数并与 kk 与运算后存入 Vx
				static std::mt19937 rng((unsigned)std::random_device{}());
				std::uniform_int_distribution<int> dist(0, 255);
				registers_[x] = uint8_t(dist(rng) & kk);
			}
			break;
		case 0xD000: { // DRW Vx, Vy, nibble：绘制n行精灵并设置 VF（碰撞）
				uint8_t vx = registers_[x] % kDisplayWidth;
				uint8_t vy = registers_[y] % kDisplayHeight;
				uint8_t height = n;
				registers_[0xF] = 0;
				for (uint8_t row = 0; row < height; ++row) {
					uint8_t sprite = memory_[indexRegister_ + row];
					for (uint8_t col = 0; col < 8; ++col) {
						if ((sprite & (0x80 >> col)) != 0) {
							int px = (vx + col) % kDisplayWidth;
							int py = (vy + row) % kDisplayHeight;
							int idx = py * kDisplayWidth + px;
							if (display_[idx]) {
								registers_[0xF] = 1;
							}
							display_[idx] = !display_[idx];
						}
					}
				}
				displayDirty_ = true;
			}
			break;
		case 0xE000:
			switch (opcode & 0x00FF) {
			case 0x9E: // SKP Vx : 如果按键 Vx 被按下则跳过下一条指令
				if (registers_[x] < keypad_.size() && keypad_[registers_[x]]) pcIncrement = 4;
				break;
			case 0xA1: // SKNP Vx : 如果按键 Vx 未被按下则跳过下一条指令
				if (registers_[x] < keypad_.size() && !keypad_[registers_[x]]) pcIncrement = 4;
				break;
			}
			break;
		case 0xF000:
			switch (opcode & 0x00FF) {
			case 0x07: // LD Vx, DT : 将延迟定时器值存入 Vx
				registers_[x] = delayTimer_;
				break;
			case 0x0A: // LD Vx, K : 等待按键按下，按键值存入 Vx
				waitingForKey_ = true;
				waitingRegister_ = x;
				// 暂停 PC 增量
				pcIncrement = 0;
				break;
			case 0x15: // LD DT, Vx : 将 Vx 值存入延迟定时器
				delayTimer_ = registers_[x];
				break;
			case 0x18: // LD ST, Vx : 将 Vx 值存入声音定时器
				soundTimer_ = registers_[x];
				break;
			case 0x1E: // ADD I, Vx : 索引寄存器 I 加上 Vx
				indexRegister_ = uint16_t(indexRegister_ + registers_[x]);
				break;
			case 0x29: // LD F, Vx: 设置 I 指向 Vx 对应字符的字模地址
				indexRegister_ = 0x50 + registers_[x] * 5;
				break;
			case 0x33: { // LD B, Vx (BCD) : 将 Vx 的值转换为 BCD 存入 I 指向的三个内存单元
				uint8_t value = registers_[x];
				memory_[indexRegister_ + 0] = value / 100;
				memory_[indexRegister_ + 1] = (value / 10) % 10;
				memory_[indexRegister_ + 2] = value % 10;
			}
				break;
			case 0x55: // LD [I], V0..Vx : 将 V0 到 Vx 存入 I 指向的内存
				for (uint8_t i = 0; i <= x; ++i) memory_[indexRegister_ + i] = registers_[i];
				break;
			case 0x65: // LD V0..Vx, [I] : 从 I 指向的内存加载到 V0 到 Vx
				for (uint8_t i = 0; i <= x; ++i) registers_[i] = memory_[indexRegister_ + i];
				break;
			}
			break;
		default:
			// 未实现/保留的指令
			break;
		}

		if (pcIncrement) pc_ += pcIncrement;
	}

} // namespace chip8


