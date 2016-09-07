#include <iostream>
#include <SDL.h>

#include "Emulator.h"

static const int SCREEN_WIDTH = 640;
static const int SCREEN_HEIGHT = 480;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Surface *screenSurface = NULL;

bool init()
{
    bool success = true;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Could not initialize SDL" << std::endl;
        success = false;
    } else {
        window = SDL_CreateWindow(
                "Chip8", 
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH,
                SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN);
        
        if(window == NULL)
        {
            std::cout << "Failed to create window. Error: " << SDL_GetError() << std::endl;
            success = false;
        } else {
            renderer = SDL_CreateRenderer(window, -1, 0);
            texture = SDL_CreateTexture(
                    renderer,
                   SDL_PIXELFORMAT_ARGB8888,
                   SDL_TEXTUREACCESS_STATIC,
                   SCREEN_WIDTH,
                   SCREEN_HEIGHT);

            screenSurface = SDL_GetWindowSurface(window);
        }
    }

    return success;
}

void deinit()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char **argv)
{
    std::cout << "Hello, world!" << std::endl;

    init();

    uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

    Emulator emu;
    while(!emu.run_loop(pixels, SCREEN_WIDTH, SCREEN_HEIGHT))
    {
        SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    deinit();
    return 0;
}
