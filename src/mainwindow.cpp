#include "mainwindow.h"

namespace natsukashii::frontend
{
MainWindow::~MainWindow() {
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  NFD_Quit();
}

using KeySaveState = std::pair<SDL_KeyCode, int>;

constexpr std::array<KeySaveState, 10> savestate_buttons{
  std::make_pair(SDLK_0, 0), std::make_pair(SDLK_1, 1), std::make_pair(SDLK_2, 2), std::make_pair(SDLK_3, 3), std::make_pair(SDLK_4, 4),
  std::make_pair(SDLK_5, 5), std::make_pair(SDLK_6, 6), std::make_pair(SDLK_7, 7), std::make_pair(SDLK_8, 8), std::make_pair(SDLK_9, 9)
};

constexpr std::array<KeySaveState, 10> loadstate_buttons{
  std::make_pair(SDLK_F10, 0), std::make_pair(SDLK_F1, 1), std::make_pair(SDLK_F2, 2), std::make_pair(SDLK_F3, 3), std::make_pair(SDLK_F4, 4),
  std::make_pair(SDLK_F5,  5), std::make_pair(SDLK_F6, 6), std::make_pair(SDLK_F7, 7), std::make_pair(SDLK_F8, 8), std::make_pair(SDLK_F9, 9)
};

MainWindow::MainWindow(std::string title) : file("config.ini") {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * 3, HEIGHT * 3, SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

  if (!file.read(ini)) {
    ini["emulator"]["skip"] = "false";
    ini["emulator"]["bootrom"] = "bootrom.bin";
    file.generate(ini);
  }

  bool skip = ini["emulator"]["skip"] == "true";
  std::string bootrom = ini["emulator"]["bootrom"];
  core = std::make_unique<Core>(skip, bootrom);

  NFD_Init();
}

void MainWindow::OpenFile() {
  nfdchar_t *outpath;
  nfdfilteritem_t filteritem[2] = {{ "Game Boy roms", "gb" }, { "Game Boy Color roms", "gbc" }};
  nfdresult_t result = NFD_OpenDialog(&outpath, filteritem, 2, "roms/");
  if(result == NFD_OKAY) {
    core->LoadROM(std::string(outpath));
  }
}

void MainWindow::UpdateTexture() {
  SDL_UpdateTexture(texture, nullptr, core->bus.ppu.pixels, WIDTH * sizeof(ColorRGBA));
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}

void MainWindow::Run() {
  int i = 0;
  bool running = true;
  int key;
  while(running) {
    u32 frameStartTicks = SDL_GetTicks();
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type) {
      case SDL_QUIT: running = false; break;
      case SDL_KEYDOWN: {
        key = event.key.keysym.sym;

        for(int i = 0; i < 10; i++) {
          if(savestate_buttons[i].first == key) {
            core->SaveState(savestate_buttons[i].second);
          }

          if(loadstate_buttons[i].first == key) {
            core->LoadState(loadstate_buttons[i].second);
          }
        }

        switch(key) {
          case SDLK_o: OpenFile(); break;
          case SDLK_s: core->Stop(); break;
          case SDLK_r: core->Reset(); break;
          case SDLK_p: core->Pause(); break;
          case SDLK_q: running = false; core->Stop(); break;
        }
      } break;
      case SDL_KEYUP: key = 0; break;
    }

    core->Run(key);

    if(core->bus.ppu.render) {
      core->bus.ppu.render = false;
    }
    
    UpdateTexture();

    while ((SDL_GetTicks() - frameStartTicks) < (1000 / 60))
      SDL_Delay(1);
  }
}
} // natsukashii::frontend