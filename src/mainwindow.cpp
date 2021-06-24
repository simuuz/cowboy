#include "mainwindow.h"

namespace natsukashii::frontend
{
MainWindow::~MainWindow()
{
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  NFD_Quit();
}

MainWindow::MainWindow(std::string title) : file("config.ini")
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * 3, HEIGHT * 3, SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

  if (!file.read(ini))
  {
    ini["emulator"]["skip"] = "false";
    ini["emulator"]["bootrom"] = "bootrom.bin";
    file.generate(ini);
  }

  bool skip = ini["emulator"]["skip"] == "true";
  std::string bootrom = ini["emulator"]["bootrom"];
  core = std::make_unique<Core>(skip, bootrom);

  NFD_Init();
}

void MainWindow::OpenFile()
{
  nfdchar_t *outpath;
  nfdfilteritem_t filteritem[2] = {{ "Game Boy roms", "gb" }, { "Game Boy Color roms", "gbc" }};
  nfdresult_t result = NFD_OpenDialog(&outpath, filteritem, 2, "roms/");
  if(result == NFD_OKAY)
  {
    core->LoadROM(std::string(outpath));
  }
}

void MainWindow::UpdateTexture()
{
  SDL_UpdateTexture(texture, nullptr, core->bus.ppu.pixels.data(), WIDTH * sizeof(ColorRGBA));
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}

void MainWindow::Run()
{
  int i = 0;
  bool running = true;

  while(running)
  {
    SDL_Event event;
    int key, action;
    SDL_PollEvent(&event);
    action = event.type;
    if(action == SDL_KEYDOWN || action == SDL_KEYUP)
      key = event.key.keysym.sym;
    
    switch(event.type) {
      case SDL_QUIT: running = false; break;
      case SDL_KEYDOWN:
        switch(key) {
          case SDLK_o: OpenFile(); break;
          case SDLK_s: core->Stop(); break;
          case SDLK_r: core->Reset(); break;
          case SDLK_p: core->Pause(); break;
          case SDLK_q: running = false; core->Stop(); break;
        }
        break;
    }

    core->Run(key, action);

    if(core->bus.ppu.render) {
      core->bus.ppu.render = false;
    }
    
    UpdateTexture();
  }
}
} // natsukashii::frontend