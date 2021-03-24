#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "tinyfiledialogs.h"
#include "config.h"
#include "natsukashii.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Cowboy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 3, 144 * 3, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

    ini_table_s* file = ini_table_create();

    char* bootrom_path = "bootrom.bin";

    bool skip = false;
    if(!ini_table_read_from_file(file, "config.ini")) {
        printf("Generating config.ini\n");
        ini_table_create_entry(file, "emulator", "bootrom", bootrom_path);
        ini_table_create_entry(file, "emulator", "skip", "false");
        ini_table_write_to_file(file, "config.ini");
    } else {
        ini_table_get_entry_as_bool(file, "emulator", "skip", &skip);
        bootrom_path = ini_table_get_entry(file, "emulator", "bootrom");
    }
    
    SDL_Event event;
    bool quit = false;
    bool pause = false;
    char* rom;
    emu_t emu;
    
    while(!quit) {
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
                    if(pause && emu.rom_opened) {
                        step_emu(&emu);
                    }
                    break;
                    case SDLK_BACKSPACE: {
                        reset_emu(&emu);
                        if(!load_bootrom(emu.bus.mem, bootrom_path)) {
                            bootrom_path = tinyfd_openFileDialog("This is a one-time thing. You need to select a bootrom file and I'll remember it for you.",
                                                                    NULL, 0, NULL, "Valid GameBoy bootrom", 0);
                            
                            load_bootrom(emu.bus.mem, bootrom_path);
                            ini_table_create_entry(file, "emulator", "bootrom", bootrom_path);
                            ini_table_write_to_file(file, "config.ini");
                        }
                        char const* filter = "*.gb";
                        const char* rom_charstr = tinyfd_openFileDialog("Select a GameBoy rom", NULL, 1, &filter, "Valid GameBoy rom", 0);

                        if(rom_charstr != NULL) {
                            rom = rom_charstr;
                            load_rom(emu.bus.mem, rom);
                        }
                    } break;
                    case SDLK_ESCAPE:
                    reset_emu(&emu);
                    rom = NULL;
                    break;
                    case SDLK_RETURN:
                    pause = !pause;
                    break;
                    case SDLK_RSHIFT:
                    reset_emu(&emu);
                    if(rom != NULL)
                        load_rom(emu.bus.mem, rom);
                    break;
                }
                break;
            }
        }

        if(!pause && emu.rom_opened) {
            run(&emu);
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}