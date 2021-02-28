#include <iostream>
#include <filesystem>
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

    if(argc < 3) {
        printf("Usage: %s <bootrom> <rom>\n", argv[0]);
        exit(1);
    }

    std::filesystem::path rompath(argv[2]);
    std::string rom_stem = rompath.stem().string();

    Mem mem(argv[1], argv[2]);
    Cpu cpu(mem);

    std::string title = "Cowboy - \"" + rom_stem + "\"";
    SDL_SetWindowTitle(window, title.c_str());

    SDL_Event event;
    bool quit = false;
    while(!quit) {
        auto start = cl_hires::now();
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: quit = true; break;
            }
        }

        for(int i = 0; i < 69905; i++)
            cpu.step();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        float frametime = std::chrono::duration<float, std::milli>(cl_hires::now() - start).count();
        char fps_frametime[32];
        snprintf(fps_frametime, 32, " | %.2f fps | %.2f ms", 1000 / frametime, frametime);
        SDL_SetWindowTitle(window, std::string(title + fps_frametime).c_str());
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}