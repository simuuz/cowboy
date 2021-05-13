#include <SDL2/SDL.h>
#include <chrono>
#include <filesystem>
#include "ini.h"
#include "cpu.h"
#include "tinyfiledialogs.h"

using clk = std::chrono::high_resolution_clock;

constexpr int DELAY = 16;
constexpr int CYCLES_PER_FRAME = 4194300 / DELAY;

int main(int argc, char* argv[])
{
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* window = SDL_CreateWindow("natsukashii", SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED, WIDTH * 3, HEIGHT * 3, SDL_WINDOW_SHOWN);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
  SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

  mINI::INIFile file("config.ini");
  mINI::INIStructure ini;
  file.read(ini);
  bool skip = ini["emulator"]["skip"] == "true";
  Cpu cpu(skip);

  SDL_Event event;
  bool quit = false;
  bool pause = false;

  std::filesystem::path rom;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

  while (!quit)
  {
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
            event.window.windowID == SDL_GetWindowID(window))
          quit = true;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDLK_p:
          if (pause && cpu.bus.mem.rom_opened)
          {
            cpu.step();
            cpu.bus.ppu.step(cpu.cycles);
          }
          break;
        case SDLK_BACKSPACE:
        {
          cpu.reset();
          cpu.bus.reset();
          std::string bootrom = ini["emulator"]["bootrom"];
          if (bootrom.empty())
          {
            bootrom =
                tinyfd_openFileDialog("This is a one-time thing. You need to select a bootrom file "
                                      "and I'll remember it for you.",
                                      (std::filesystem::current_path().string() + "/").c_str(), 0,
                                      nullptr, "Valid GameBoy bootrom", 0);
            ini["emulator"]["bootrom"] = bootrom;
            file.write(ini);
          }

          cpu.bus.mem.load_bootrom(bootrom);
          char const* filter = "*.gb";
          const char* rom_charstr = tinyfd_openFileDialog(
              "Select a GameBoy rom", (std::filesystem::current_path().string() + "/").c_str(), 1,
              &filter, "Valid GameBoy rom", 0);

          if (rom_charstr != nullptr)
          {
            rom = rom_charstr;
            cpu.bus.mem.load_rom(rom.string());
          }
        }
        break;
        case SDLK_ESCAPE:
          cpu.reset();
          cpu.bus.reset();
          rom.clear();
          break;
        case SDLK_RETURN:
          pause = !pause;
          break;
        case SDLK_RSHIFT:
          cpu.reset();
          cpu.bus.reset();
          if (!rom.empty())
            cpu.bus.mem.load_rom(rom.string());
          break;
        }
      }
    }

    auto start = clk::now();

    if (!pause && cpu.bus.mem.rom_opened)
    {
      while(cpu.total_cycles < CYCLES_PER_FRAME)
      {
        cpu.step();
        cpu.bus.ppu.step(cpu.cycles);
        cpu.handle_timers();
      }

      cpu.total_cycles -= CYCLES_PER_FRAME;
    }

    if(cpu.bus.ppu.render)
    {
      cpu.bus.ppu.render = false;
      SDL_UpdateTexture(texture, nullptr, cpu.bus.ppu.pixels, WIDTH * 3);
      SDL_RenderCopy(renderer, texture, nullptr, nullptr);
      SDL_RenderPresent(renderer);
    }

    if(!cpu.bus.mem.rom_opened)
    {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      SDL_RenderPresent(renderer);
    }

    SDL_Delay(DELAY);

    float frametime = std::chrono::duration<float, std::milli>(clk::now() - start).count();
    char fps_frametime[32];
    snprintf(fps_frametime, 32, " | %.2f fps | %.2f ms", 1000 / frametime, frametime);
    SDL_SetWindowTitle(
        window,
        (cpu.bus.mem.rom_opened) ?
            ("natsukashii" + std::string(" - \"") + rom.stem().string() + "\"" + fps_frametime)
                .c_str() :
            ("natsukashii" + std::string(" - Nothing playing") + fps_frametime).c_str());
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_DestroyTexture(texture);
  SDL_Quit();

  return 0;
}