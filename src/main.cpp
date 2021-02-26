#include <iostream>
#include <chrono>
#include <SDL2/SDL.h>
#include "cpu.h"
#include "mem.h"

using cl_hires = std::chrono::high_resolution_clock;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    if(SDL_CreateWindowAndRenderer(160 * 3, 144 * 3, SDL_WINDOW_SHOWN, &window, &renderer)) {
        printf("SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    std::string title = "Cowboy";
    SDL_SetWindowTitle(window, title.c_str());

    if(argc < 3) {
        printf("Usage: %s <bootrom> <rom>\n", argv[0]);
        exit(1);
    }
    Mem mem(argv[1], argv[2]);
    Cpu emu;

    SDL_Event event;
    bool quit = false;
    while(!quit) {
        auto start = cl_hires::now();
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: quit = true; break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(1);
        float frametime = std::chrono::duration<float, std::milli>(cl_hires::now() - start).count();
        std::string frametimestr = std::to_string(frametime);
        std::string fps = std::to_string(1000 / frametime);
        SDL_SetWindowTitle(window, std::string(title + " | " + fps.erase(fps.find_first_of('.')) + " fps | " + frametimestr.erase(frametimestr.find_first_of('.')) + " ms").c_str());
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}