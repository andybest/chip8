#pragma once

#include <stdint.h>
#include <SDL.h>

class Emulator
{
    public:
        Emulator();        
        void run_loop();

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
        void run_tick();
        void run_instruction(uint16_t opcode);
        int16_t check_keys(SDL_Keycode code, bool keyState);

        void push_stack(uint16_t value);
        uint16_t pop_stack();

        void display_sprite(uint16_t num_bytes, uint8_t x, uint8_t y);
        void wait_key_press();
};
