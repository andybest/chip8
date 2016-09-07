#include "Emulator.h"
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <SDL.h>

static const uint8_t CHIP8_FONT[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
    0x20, 0x60, 0x20, 0x20, 0x70,   // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
    0xF0, 0x80, 0xF0, 0x80, 0x80};  // F

static const SDL_Keycode keyAssignments[] = {
    SDLK_x,     // 0
    SDLK_1,     // 1
    SDLK_2,     // 2
    SDLK_3,     // 3
    SDLK_q,     // 4
    SDLK_w,     // 5
    SDLK_e,     // 6
    SDLK_a,     // 7
    SDLK_s,     // 8
    SDLK_d,     // 9
    SDLK_z,     // A
    SDLK_c,     // B
    SDLK_4,     // C
    SDLK_r,     // D
    SDLK_f,     // E
    SDLK_v      // F
};

static const SDL_Keycode KEY_QUIT = SDLK_ESCAPE;

Emulator::Emulator()
{
    init();
}

void Emulator::init()
{
    quit = false;
    waiting_for_key = false;
    srand(time(NULL));
    copy_font();
}

void Emulator::copy_font()
{
    // Copy the font into memory
    std::memcpy(memory, CHIP8_FONT, 5 * 16);
}

void Emulator::load_file(std::string path)
{
    uint32_t maxSize = MEMORY_SIZE - START_ADDRESS;
}

bool Emulator::run_loop(uint32_t *pixels, uint32_t width, uint32_t height)
{
    run_tick(pixels, width, height);
    return quit;
}

void Emulator::run_tick(uint32_t *pixels, uint32_t width, uint32_t height)
{
    if(delayTimer > 0) {
        --delayTimer;
    }

    if(soundTimer > 0) {
        --soundTimer;
    }


    // Check events
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0)
    {
        if(e.type == SDL_QUIT)
            quit = true;
        else if(e.type == SDL_KEYDOWN) {
            SDL_Keycode keycode = e.key.keysym.sym;

            if(keycode == KEY_QUIT)
            {
                quit = true;
            } else {
                int16_t key = check_keys(keycode, true);
                if(key > 0 && waiting_for_key)
                {
                    waiting_for_key = false;
                    V[keyReg] = (uint8_t)key;
                }
            }
        } else if(e.type == SDL_KEYUP) {
            check_keys(e.key.keysym.sym, false);
        }
    }

    if(!waiting_for_key)
    {
        for(int i = 0; i < 256; ++i)
        {
            uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];

            ++pc;
            run_instruction(opcode);

            if(waiting_for_key)
                continue;
        }
    }

    // Copy screen buffer
    for(int x = 0; x < 64; ++x)
    {
        for(int y = 0; y < 32; ++y)
        {
            pixels[x + (y * width)] =
                (screenBuf[x + (y * 64)] > 0)? 0xFFFFFFFF: 0xFF000000;
        }
    }
}

int16_t Emulator::check_keys(SDL_Keycode code, bool keyState)
{
    for(int i = 0; i < 0xF; ++i)
    {
        if(code == keyAssignments[i]) {
            keys[i] = keyState;
            return i;
        }
    }

    return -1;
}

/*
  			00E0 - CLS
            00EE - RET
            0nnn - SYS addr
            1nnn - JP addr
            2nnn - CALL addr
            3xkk - SE Vx, byte
            4xkk - SNE Vx, byte
            5xy0 - SE Vx, Vy
            6xkk - LD Vx, byte
            7xkk - ADD Vx, byte
            8xy0 - LD Vx, Vy
            8xy1 - OR Vx, Vy
            8xy2 - AND Vx, Vy
            8xy3 - XOR Vx, Vy
            8xy4 - ADD Vx, Vy
            8xy5 - SUB Vx, Vy
            8xy6 - SHR Vx {, Vy}
            8xy7 - SUBN Vx, Vy
            8xyE - SHL Vx {, Vy}
            9xy0 - SNE Vx, Vy
            Annn - LD I, addr
            Bnnn - JP V0, addr
            Cxkk - RND Vx, byte
            Dxyn - DRW Vx, Vy, nibble
            Ex9E - SKP Vx
            ExA1 - SKNP Vx
            Fx07 - LD Vx, DT
            Fx0A - LD Vx, K
            Fx15 - LD DT, Vx
            Fx18 - LD ST, Vx
            Fx1E - ADD I, Vx
            Fx29 - LD F, Vx
            Fx33 - LD B, Vx
            Fx55 - LD [I], Vx
            Fx65 - LD Vx, [I]
*/

void Emulator::run_instruction(uint16_t opcode)
{
    uint8_t s = (opcode >> 12) & 0xF;   // Most significant nibble
    uint8_t ls = opcode & 0xF;          // Least significant nibble
    uint8_t x = (opcode >> 8) & 0xF;
    uint8_t y = (opcode >> 4) & 0xF;
    uint8_t kk = opcode & 0xFF;
    uint8_t nnn = opcode & 0xFFF;
    uint8_t F = 0xF;

    if(opcode == 0x00E0) {                          // CLS
        for(int i = 0; i < screenBufSize; i++)
            screenBuf[i] = 0;
    } else if(opcode == 0x00EE) {                   // RET
        pc = pop_stack();
    } else if(s == 1) {                             // JP addr
        pc = nnn;
    } else if(s == 2) {                             // CALL addr
        push_stack(pc);
        pc = nnn;
    } else if(s == 3) {                             // SE Cx, byte
        if(V[x] == kk)
            pc += 2;
    } else if(s == 4) {                             // SNE Vx, byte
        if(V[x] != kk)
            pc += 2;
    } else if(s == 5) {                             // SE Vx, Vy
        if(V[x] == V[y])
            pc += 2;
    } else if(s == 6) {                             // LD Vx, byte
        V[x] = kk;
    } else if(s == 7) {                             // ADD Vx, byte
        V[x] += kk;
    } else if(s == 8 && ls == 0) {                  // LD Vx, Vy
        V[x] = V[y];
    } else if(s == 8 && ls == 1) {                  // OR Vx, Vy
        V[x] = V[x] | V[y];
    } else if(s == 8 && ls == 2) {                  // AND Vx, Vy
        V[x] = V[x] & V[y];
    } else if(s == 8 && ls == 3) {                  // XOR Vx, Vy
        V[x] = V[x] ^ V[y];
    } else if(s == 8 && ls == 4) {                  // ADD Vx, Vy
        uint16_t r = V[x] + V[y];
        V[F] = (r > 0xFF)? 1: 0;
        V[x] = r & 0xFF;
    } else if(s == 8 && ls == 5) {                  // SUB Vx, Vy
        V[F] = (V[x] > V[y])? 1: 0;
        V[x] = V[x] - V[y];
   } else if(s == 8 && ls == 6) {                   // SHR Vx {, Vy}
       V[F] = V[x] & 0x1;
       V[x] >>= 1;
   } else if(s == 8 && ls == 7) {                   // SUBN Vx, Vy
       V[F] = (V[y] > V[x])? 1: 0;
       V[x] = V[y] - V[x];
   } else if(s == 8 && ls == 0xE) {                 // SHL Vx {, Vy}
       V[F] = V[x] & 0x80;
       V[x] <<=1; 
   } else if(s == 9 && ls == 0) {                   // SNE Vx, Vy
       if(V[x] != V[y])
           pc += 2;
   } else if(s == 0xA) {                              // LD I, addr
       I = nnn;
   } else if(s == 0xB) {                              // JP V0, addr
       pc = V[0] + nnn;
   } else if(s == 0xC) {                              // RND Vx, byte
       V[x] = (rand() % 256) & kk;
   } else if(s == 0xD) {                              // DRW Vx, Vy, nibble
       display_sprite(I + ls, x, y);
   } else if(s == 0xE && kk == 0x9E) {                // SKP Vx
       if(keys[V[x] & 0xF])
           pc += 2;
   } else if(s == 0xE && kk == 0xA1) {                // SKNP Vx
       if(!keys[V[x] & 0xF])
           pc += 2;
   } else if(s == 0xF && kk == 0x07) {              // LD Vx, DT
       V[x] = delayTimer;
   } else if(s == 0xF && kk == 0x0A) {              // LD Vx, K
       keyReg = x;
       wait_key_press();
   } else if(s == 0xF && kk == 0x15) {              // LD DT, Vx
       delayTimer = V[x];
   } else if(s == 0xF && kk == 0x18) {              // LD ST, Vx
       soundTimer = V[x];
   } else if(s == 0xF && kk == 0x1E) {              // ADD I, Vx
       I = I + V[x];
   } else if(s == 0xF && kk == 0x29) {              // LD F, Vx
       I = (V[x] & 0xF) * 5;
   } else if(s == 0xF && kk == 0x33) {              // LD B, Vx
       uint8_t units = V[x] % 10;
       uint8_t tens = (V[x] % 100) - units;
       uint8_t hundreds = V[x] - tens - units;
       memory[I] = hundreds;
       memory[I + 1] = tens;
       memory[I + 2] = units;
   } else if(s == 0xF && kk == 0x55) {              // LD [I], Vx
       std::memcpy(memory + I, V, x + 1);
   } else if(s == 0xF && kk == 65) {                // LD Vx, [I]
       std::memcpy(V, memory + I, x + 1);
   }
}

void Emulator::push_stack(uint16_t val)
{
    if(sp == 15)
        sp = 0;
    else
        ++sp;

    stack[sp] = val;
}

uint16_t Emulator::pop_stack()
{
    uint16_t rv = stack[sp];
    
    if(sp == 0)
        sp = 15;
    else
        --sp;

    return rv;
}

void Emulator::display_sprite(uint16_t num_bytes, uint8_t sx, uint8_t sy)
{
    V[0xF] = 0;

    for(int y = 0; y < num_bytes; y++)
    {
        int idx = (I + y) % 4096;
        uint8_t sprite = memory[idx];
        uint16_t bufY = (y + sy) % 32;

        for(int x = 0; x < 8; x++)
        {
            uint8_t bit = (sprite >> (7 - x)) & 0x1;
            uint16_t bufX = (x + sx) % 64;

            uint16_t screenIdx = bufX + (bufY * 64);
            uint8_t oldVal = screenBuf[screenIdx];
            screenBuf[screenIdx] ^= bit;
            if(oldVal == 1 && screenBuf[screenIdx] == 0)
                V[0xF] = 1;
        }
    }
}

void Emulator::wait_key_press()
{
    if(!waiting_for_key)
    {
        waiting_for_key = true;
        return;
    }
}
