#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <stack>

constexpr std::size_t screenWidth = 64;
constexpr std::size_t screenHeight = 32;

class Interpreter {
	static constexpr std::size_t ramSize = 4096;
	static constexpr std::size_t fontsSize = 80;
	static constexpr std::size_t numRegs = 16;
	static constexpr std::size_t stackSize = 16;
	static constexpr std::size_t numKeys = 16;
	static constexpr std::uint16_t startAddr = 0x200;
	static constexpr std::uint16_t endAddr = 0xfff;
 
	static constexpr std::array<std::uint8_t, fontsSize> fonts = {
		0xf0, 0x90, 0x90, 0x90, 0xf0,   // 0
	    0x20, 0x60, 0x20, 0x20, 0x70,   // 1
	    0xf0, 0x10, 0xf0, 0x80, 0xf0,   // 2
	    0xf0, 0x10, 0xf0, 0x10, 0xf0,   // 3
	    0x90, 0x90, 0xf0, 0x10, 0x10,   // 4
	    0xf0, 0x80, 0xf0, 0x10, 0xf0,   // 5
	    0xf0, 0x80, 0xf0, 0x90, 0xf0,   // 6
	    0xf0, 0x10, 0x20, 0x40, 0x40,   // 7
	    0xf0, 0x90, 0xf0, 0x90, 0xf0,   // 8
	    0xf0, 0x90, 0xf0, 0x10, 0xf0,   // 9
	    0xf0, 0x90, 0xf0, 0x90, 0x90,   // A
	    0xe0, 0x90, 0xe0, 0x90, 0xe0,   // B
	    0xf0, 0x80, 0x80, 0x80, 0xf0,   // C
	    0xe0, 0x90, 0x90, 0x90, 0xe0,   // D
	    0xf0, 0x80, 0xf0, 0x80, 0xf0,   // E
	    0xf0, 0x80, 0xf0, 0x80, 0x80,   // F
	};

	std::array<std::uint8_t, ramSize> ram = {};
	std::array<std::uint8_t, numRegs> vRegs = {};		// Do not use VF flags register!
	std::stack<std::uint16_t> stack = {};
	std::array<bool, numKeys> keys = {false};
	uint8_t dt = 0;
	uint8_t st = 0;
	uint16_t iReg = 0;
	uint16_t progCtr = startAddr;
	uint8_t stackPtr = 0;

    // Buzzer for beep sound
    // Display

public:
	Interpreter() {
		// Initialize RAM with fonts data
		std::copy(std::begin(fonts), std::end(fonts), std::begin(ram));
	}

	bool loadRom(std::string filename) {
		std::ifstream file(filename, std::ios::binary);
		if (!file.is_open()) {
			std::cout << "Failed to open file.\n";
			return false;
		} else {
			file.read(reinterpret_cast<char*>(ram.data() + startAddr), endAddr - startAddr);
			return true;
		}
	}

	void printRam() {
		std::cout << "RAM:\n";
		for (int i = 0; i < ram.size(); i++) {
			std::cout << std::format("{:x} ", ram[i]);
		}
		std::cout << '\n';
	}

	void dumpState() {
		std::cout << std::format("VREGS: |");
		for (int i = 0; i < vRegs.size(); i++) {
			std::cout << std::format(" {:x} |", vRegs[i]);
		}
		std::cout << '\n';

		std::cout << std::format("Stack Ptr: {:x}\n", stackPtr);
		std::cout << std::format("STACK:");
		std::stack<uint16_t> temp = stack;
		if (temp.empty()) {
			std::cout << " empty";
		} else {
			while (!temp.empty()) {
				std::cout << std::format(" {:x} |", temp.top());
				temp.pop();
			}
		}
		std::cout << '\n';

		std::cout << std::format("KEYS: |");
		for (int i = 0; i < keys.size(); i++) {
			std::cout << std::format(" {:x} |", keys[i]);
		}
		std::cout << '\n';

		std::cout << std::format("Delay Timer: {:x}\n", dt);
		std::cout << std::format("Sound Timer: {:x}\n", st);
		std::cout << std::format("Index Register: {:x}\n", iReg);
		std::cout << std::format("Program Counter: {:x}\n", progCtr);

		printRam();
	}

	uint16_t fetch() {
		uint16_t instruction = (static_cast<uint16_t>(ram[progCtr]) << 8) | ram[progCtr + 1];
		progCtr += 2;
		return instruction;
	}

	void execute(uint16_t instruction) {
		// Decode
		uint16_t first = (instruction & 0xf000) >> 12;
		uint16_t second = (instruction & 0x0f00) >> 8;
		uint16_t third = (instruction & 0x00f0) >> 4;
		uint16_t fourth = instruction & 0x000f;
		std::cout << std::format("Decoded: {:x} | {:x} | {:x} | {:x}\n", first, second, third, fourth);

		// Execute:
		switch(first) {
		case 0x0:
			switch(fourth) {
			case 0x0:
				// Clear display
				break;
			case 0xe:
				// Return from subroutine
				//progCtr = stack.pop();
				//stackPtr--;
				break;
			}
			break;
		case 0x1:
			// Jump to NNN
			progCtr = (instruction & 0x0fff) << 4;
			std::cout << std::format("Jumping to {:x}\n", progCtr);
			break;
		case 0x2:
			break;
		case 0x3:
			break;
		case 0x4:
			break;
		case 0x5:
			break;
		case 0x6:
			break;
		case 0x7:
			break;
		case 0x8:
			break;
		case 0x9:
			break;
		case 0xa:
			break;
		case 0xb:
			break;
		case 0xc:
			break;
		case 0xd:
			break;
		case 0xe:
			break;
		case 0xf:
			break;
		default:
			std::cout << "Invalid instruction\n";
			break;
		}
	}
};

int main(int argc, char* argv[]) {
	Interpreter interp;
	interp.loadRom("tetris.ch8");

	interp.dumpState();

	uint16_t instruction = 0;
	instruction = interp.fetch();
	std::cout << std::format("\nCurrent instruction: {:x}\n", instruction);
	//interp.execute(instruction);

	interp.execute(0x1abc);

	while (true) { // Should run around 700 instructions per second
		break;
		// Fetch
		instruction = interp.fetch();
		// std::cout << std::format("\nCurrent instruction: {:x}\n", instruction);
		// if (instruction == 0) {
		// 	break;
		// }

		// Decode + execute
		interp.execute(instruction);
	}

	return 0;
}