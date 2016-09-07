#pragma once

#include <string>
#include <stdint.h>
#include <SDL.h>

static const uint32_t MEMORY_SIZE = 4096;
static const uint16_t START_ADDRESS = 0x200;

class Emulator
{
    public:
        Emulator();        
        bool run_loop(uint32_t *pixels, uint32_t width, uint32_t height);
        void load_file(std::string path);

    private:
        bool quit;

        uint8_t memory[4096];
        uint8_t V[16];                  // Registers
        uint16_t I;                     // Index register
        uint16_t pc;                    // Program counter

        uint16_t stack[16];
        uint16_t sp;                    // Stack pointer

        bool keys[16];                  // Key array
        uint8_t keyReg;                 // Register to write key to
        bool waiting_for_key;

        static const uint16_t screenBufSize = 64 * 32;
        uint8_t screenBuf[screenBufSize];     // Screen buffer

        // Timers
        uint8_t delayTimer;
        uint8_t soundTimer;

        void init();
        void copy_font();
        void run_tick(uint32_t *pixels, uint32_t width, uint32_t height);
        void run_instruction(uint16_t opcode);
        int16_t check_keys(SDL_Keycode code, bool keyState);

        void push_stack(uint16_t value);
        uint16_t pop_stack();

        void display_sprite(uint16_t num_bytes, uint8_t x, uint8_t y);
        void wait_key_press();
};
