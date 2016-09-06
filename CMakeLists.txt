project(chip8)

set(CMAKE_CXX_STANDARD 14)

set(chip8_SOURCES
    src/chip8.cpp)

add_executable(chip8 ${chip8_SOURCES})
