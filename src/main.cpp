#include <iostream>
#include <filesystem>
#include <chrono>
#include <SDL2/SDL.h>
#include "tinyfiledialogs.h"
#include "ini.h"
#include "cpu.h"
#include "mem.h"

using cl_hires = std::chrono::high_resolution_clock;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    std::string title = "Cowboy";
    SDL_Window* window = SDL_CreateWindow("Cowboy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 3, 144 * 3, SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

    Mem mem;
    Cpu cpu;
    
    SDL_Event event;
    bool quit = false;
    bool pause = false;
    std::filesystem::path rom;
    while(!quit) {
        auto start = cl_hires::now();
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: quit = true; break;
                case SDL_WINDOWEVENT:
                if(event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                    quit = true;
                break;
                case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_p:
                    if(pause && mem.rom_opened) {
                        cpu.step();
                    }
                    break;
                    case SDLK_BACKSPACE: {
                        mem.reset();
                        cpu.reset();
                        mINI::INIFile file("config.ini");
                        mINI::INIStructure ini;
                        file.read(ini);
                        std::string bootrom = ini["emulator"]["bootrom"];
                        if(bootrom.empty()) {
                            bootrom = tinyfd_openFileDialog("This is a one-time thing. You need to select a bootrom file and I'll remember it for you.",
                                    (std::filesystem::current_path().string() + "/").c_str(), 0, nullptr, "Valid GameBoy bootrom", 0);
                            ini["emulator"]["bootrom"] = bootrom;
                            file.write(ini);
                        }
                        mem.loadBootROM(bootrom);
                        char const* filter = "*.gb";
                        const char* rom_charstr = tinyfd_openFileDialog("Select a GameBoy rom",
                                    (std::filesystem::current_path().string() + "/").c_str(), 1, &filter, "Valid GameBoy rom", 0);

                        if(rom_charstr != nullptr) {
                            rom = rom_charstr;
                            mem.loadROM(rom.string());
                            cpu.mem = mem;
                        }
                    } break;
                    case SDLK_ESCAPE:
                    cpu.reset();
                    mem.reset();
                    rom.clear();
                    cpu.mem = mem;
                    break;
                    case SDLK_RETURN:
                    pause = !pause;
                    break;
                    case SDLK_RSHIFT:
                    cpu.reset();
                    mem.reset();
                    if(!rom.empty())
                        mem.loadROM(rom.string());
                    cpu.mem = mem;
                    break;
                }
                break;
            }
        }

        for(int i = 0; !pause && mem.rom_opened && i < 69905; i++) {
            cpu.step();
        }

        SDL_SetRenderDrawColor(renderer, 0,0,0,0);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        float frametime = std::chrono::duration<float, std::milli>(cl_hires::now() - start).count();
        char fps_frametime[32];
        snprintf(fps_frametime, 32, " | %.2f fps | %.2f ms", 1000 / frametime, frametime);
        SDL_SetWindowTitle(window, (mem.rom_opened) ? (title + " - \"" + rom.stem().string() + "\"" + fps_frametime).c_str()
                                                    : (title + " - Nothing playing" + fps_frametime).c_str());
        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}