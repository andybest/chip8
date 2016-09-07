#include <iostream>
#include <SDL.h>

static const int SCREEN_WIDTH = 640;
static const int SCREEN_HEIGHT = 480;

SDL_Window *window = NULL;
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
            screenSurface = SDL_GetWindowSurface(window);
        }
    }

    return success;
}

void deinit()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char **argv)
{
    std::cout << "Hello, world!" << std::endl;

    init();

    bool done = false;
    SDL_Event e;

    while(!done)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                done = true;
            } else if(e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        done = true;
                        break;
                }
            }
        }
    }

    deinit();
    return 0;
}
