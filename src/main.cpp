#include <SDL2/SDL.h>
#include <filesystem>
#include <chrono>
#include "natsukashii.h"
#include "tinyfiledialogs.h"
#include "ini.h"

using clk = std::chrono::high_resolution_clock;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("natsukashii", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 3, 144 * 3, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
    
    mINI::INIFile file("config.ini");
    mINI::INIStructure ini;
    file.read(ini);
    bool skip = ini["emulator"]["skip_bootrom"] == "true";

    Emu emu(skip);

    SDL_Event event;
    bool quit = false;
    bool pause = false;

    std::filesystem::path rom;

    while(!quit) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                quit = true;
                break;
                case SDL_WINDOWEVENT:
                if(event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                    quit = true;
                break;
                case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_p:
                    if(pause && emu.cpu.bus.mem.rom_opened) {
                        emu.step();
                    }
                    break;
                    case SDLK_BACKSPACE: {
                        emu.reset();
                        std::string bootrom = ini["emulator"]["bootrom"];
                        if(bootrom.empty()) {
                            bootrom = tinyfd_openFileDialog("This is a one-time thing. You need to select a bootrom file and I'll remember it for you.",
                                    (std::filesystem::current_path().string() + "/").c_str(), 0, nullptr, "Valid GameBoy bootrom", 0);
                            ini["emulator"]["bootrom"] = bootrom;
                            file.write(ini);
                        }

                        emu.cpu.bus.mem.load_bootrom(bootrom);
                        char const* filter = "*.gb";
                        const char* rom_charstr = tinyfd_openFileDialog("Select a GameBoy rom",
                                    (std::filesystem::current_path().string() + "/").c_str(), 1, &filter, "Valid GameBoy rom", 0);

                        if(rom_charstr != nullptr) {
                            rom = rom_charstr;
                            emu.cpu.bus.mem.load_rom(rom.string());
                        }
                    } break;
                    case SDLK_ESCAPE:
                    emu.reset();
                    rom.clear();
                    break;
                    case SDLK_RETURN:
                    pause = !pause;
                    break;
                    case SDLK_RSHIFT:
                    emu.reset();
                    if(!rom.empty())
                        emu.cpu.bus.mem.load_rom(rom);
                    break;
                }
            }
        }

        auto start = clk::now();

        if(!pause && emu.cpu.bus.mem.rom_opened) {
            emu.run();
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        float frametime = std::chrono::duration<float, std::milli>(clk::now() - start).count();
        char fps_frametime[32];
        snprintf(fps_frametime, 32, " | %.2f fps | %.2f ms", 1000 / frametime, frametime);
        SDL_SetWindowTitle(window, (emu.cpu.bus.mem.rom_opened) ? ("natsukashii" + std::string(" - \"") + rom.stem().string() + "\"" + fps_frametime).c_str()
                                                        : ("natsukashii" + std::string(" - Nothing playing") + fps_frametime).c_str());

        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}