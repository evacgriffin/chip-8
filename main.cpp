#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>

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
	std::array<std::uint8_t, numRegs> regs = {};		// Do not use VF flags register!
	std::array<std::uint16_t, stackSize> stack = {};
	std::array<bool, numKeys> keys = {false};
	uint8_t delayTimer = 0;
	uint8_t soundTimer = 0;
	uint16_t indexReg = 0;
	uint16_t programCtr = startAddr;
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
		for (int i = 0; i < ram.size(); i++) {
			std::cout << std::format("{:x} ", ram[i]);
		}
	}
};

int main(int argc, char **argv) {
	Interpreter interp;
	interp.loadRom("tetris.ch8");
	interp.printRam();

	return 0;
}