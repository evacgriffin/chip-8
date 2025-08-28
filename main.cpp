#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <stack>

void cleanupSDL(SDL_Window *display);

constexpr int scale = 10;
constexpr std::size_t screenWidth = 64 * scale;
constexpr std::size_t screenHeight = 32 * scale;
using DisplayArray = std::array<bool, screenWidth * screenHeight>;

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
	DisplayArray display = {};
	std::array<std::uint8_t, numRegs> vRegs = {};		// Do not use VF flags register!
	std::stack<std::uint16_t> stack = {};
	std::array<bool, numKeys> keys = {false};
	uint8_t dt = 0;
	uint8_t st = 0;
	uint16_t iReg = 0;
	uint16_t progCtr = startAddr;
	uint8_t stackPtr = 0;

public:
	Interpreter() {
		// Initialize RAM with fonts data
		std::copy(std::begin(fonts), std::end(fonts), std::begin(ram));
	}

	bool loadRom(std::string const& filename) {
		std::ifstream file(filename, std::ios::binary);
		if (!file.is_open()) {
			return false;
		} else {
			file.read(reinterpret_cast<char*>(ram.data() + startAddr), endAddr - startAddr);
			return true;
		}
	}

	DisplayArray& getDisplay() {
		return display;
	}

	void pressKey(int idx, bool pressed) {
		if (idx < 0 || idx >= keys.size()) {
			std::cout << "Key press error. Index out of bounds.\n";
			return;
		}

		keys[idx] = pressed;
	}

	void printRam() {
		std::cout << "RAM:\n";
		for (int i = 0; i < ram.size(); i++) {
			if (i % 16 == 0) {
				std::cout << '\n';
			}
			std::cout << std::format("{:02x} ", ram[i]);
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

	void tick() {
		uint16_t instruction = fetch();
		std::cout << std::format("\nCurrent instruction: {:x}\n", instruction);

		// Decode and execute the instruction
		execute(instruction);
	}

private:
	void tickTimers() {
		if (dt > 0) {
			dt--;
		}

		if (st > 0) {
			if (st == 1) {
				// Play beep
			}
			st--;
		}
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
				std::cout << "Clearing screen...\n";
				break;
			case 0xe:
				// Return from subroutine
				progCtr = stack.top();
				stack.pop();
				stackPtr--;
				std::cout << std::format("Returning from subroutine to {:x}\n", progCtr);
				break;
			}
			break;
		case 0x1:
			// Jump to NNN
			progCtr = instruction & 0x0fff;
			std::cout << std::format("Jumping to {:x}\n", progCtr);
			break;
		case 0x2:
			// Call subroutine
			stack.push(progCtr);
			stackPtr++;
			progCtr = instruction & 0x0fff;
			std::cout << std::format("Calling subroutine at {:x}\n", progCtr);
			break;
		case 0x3:
			if (vRegs[static_cast<int>(second)] == (instruction & 0x00ff)) {
				// Skip next instruction
				progCtr += 2;
				std::cout << std::format("Skipping next instruction due to vReg == NN. Continuing execution at {:x}\n", progCtr);
			}
			break;
		case 0x4:
			if (vRegs[static_cast<int>(second)] != (instruction & 0x00ff)) {
				// Skip next instruction
				progCtr += 2;
				std::cout << std::format("Skipping next instruction due to vReg != NN. Continuing execution at {:x}\n", progCtr);
			}
			break;
		case 0x5:
			if (vRegs[static_cast<int>(second)] == vRegs[static_cast<int>(third)]) {
				// Skip next instruction
				progCtr += 2;
				std::cout << std::format("Skipping next instruction due to vX == vY. Continuing execution at {:x}\n", progCtr);
			}
			break;
		case 0x6:
			// Set vReg
			vRegs[static_cast<int>(second)] = instruction & 0x00ff;
			std::cout << std::format("Setting V{} to {:x}\n", static_cast<int>(second), instruction & 0x00ff);
			break;
		case 0x7:
			// Add value to vReg
			vRegs[static_cast<int>(second)] += instruction & 0x00ff;
			std::cout << std::format("Adding {:x} to V{}\n", instruction & 0x00ff, static_cast<int>(second));
			break;
		case 0x8:
			break;
		case 0x9:
			if (vRegs[static_cast<int>(second)] != vRegs[static_cast<int>(third)]) {
				// Skip next instruction
				progCtr += 2;
				std::cout << std::format("Skipping next instruction due to vX != vY. Continuing execution at {:x}\n", progCtr);
			}
			break;
		case 0xa:
			// Set iReg
			iReg = instruction & 0x0fff;
			std::cout << std::format("Setting iReg to {:x}\n", instruction & 0x0fff);
			break;
		case 0xb:
			// Change program counter
			progCtr = vRegs[0] + (instruction & 0x0fff);
			std::cout << std::format("Setting program counter to {:x}\n", progCtr);
			break;
		case 0xc:
			// Randomize vReg
			std::cout << "Randomizing vReg...\n";
			break;
		case 0xd:
			// Display sprite
			std::cout << "Displaying sprite...\n";
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
	if (argc < 2) {
		std::cout << "Usage: path.exe filename\n";
		return 1;
	}
	
	std::string romName = argv[1];

	// Initialize SDL
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
		return 1;
	}

	// Create display
	SDL_Window *display = SDL_CreateWindow("Chip-8", screenWidth, screenHeight, 0);
	if (!display) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating display", display);
		cleanupSDL(display);
		return 1;
	}

	// Create interpreter
	Interpreter interp;
	if (!interp.loadRom(romName)) {
		std::cout << std::format("Could not open file named {}.\n", romName);
		return 1;
	}

	bool running = true;
	while (running) { // Should run around 700 instructions per second
		SDL_Event event{ 0 };
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				running = false;
				break;
			}
		}

		interp.tick();
		// // Fetch
		// uint16_t instruction = interp.fetch();

		// // Decode + execute
		// interp.execute(instruction);

		// // Debug
		// interp.dumpState();
		// // Get user input and execute debug options, e.g. "ram" to dump ram, "quit" etc.
		// std::cin.get();
	}

	cleanupSDL(display);
	return 0;
}

void cleanupSDL(SDL_Window *display) {
	SDL_DestroyWindow(display);
	SDL_Quit();
}