#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>

constexpr std::size_t ramSize = 4096;
constexpr std::size_t fontsSize = 80;
constexpr std::size_t numRegs = 16;
constexpr std::size_t stackSize = 16;
constexpr std::size_t numKeys = 16;
constexpr std::uint16_t startAddr = 0x200;
constexpr std::uint16_t endAddr = 0xfff;
constexpr std::size_t screenWidth = 64;
constexpr std::size_t screenHeight = 32;

// Font set
constexpr std::array<std::uint8_t, fontsSize> fonts = {
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

int main(int argc, char **argv) {
	// Define system components
	std::array<std::uint8_t, ramSize> ram = {};
	std::copy(std::begin(fonts), std::end(fonts), std::begin(ram));

	// Read ROM into memory
	std::string filename = "tetris.ch8";
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		std::cout << "Failed to open file.\n";
	} else {
		file.read(reinterpret_cast<char*>(ram.data() + startAddr), endAddr - startAddr);
	}

	// for (int i = 0; i < ram.size(); i++) {
	// 	std::cout << std::format("{:x} ", ram[i]);
	// }

	return 0;
}