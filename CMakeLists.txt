cmake_minimum_required(VERSION 3.15...4.0)

project(chip-8
	VERSION 1.0
    DESCRIPTION "Chip-8 Interpreter"
    LANGUAGES CXX)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)

add_executable(chip-8
	WIN32
	main.cpp)

target_link_libraries(chip-8 PRIVATE SDL3::SDL3)