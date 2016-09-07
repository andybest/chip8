cmake_minimum_required(VERSION 3.0)
project(chip8)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 14)

# External dependencies

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

include_directories(src)

set(chip8_SOURCES
    src/Emulator.cpp
    src/chip8.cpp)

add_executable(chip8 ${chip8_SOURCES})
target_link_libraries(chip8 ${SDL2_LIBRARY})
