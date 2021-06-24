#pragma once
#include "core.h"
#include <nfd.hpp>

namespace natsukashii::frontend
{
using namespace natsukashii::core;

constexpr float aspect_ratio_gb = (float)WIDTH / (float)HEIGHT;
struct MainWindow
{
  MainWindow(std::string title);
  ~MainWindow();
  void Run();
  void OpenFile();
  void UpdateTexture();
  bool running = true;
  mINI::INIFile file;
  mINI::INIStructure ini;
  SDL_Window* window = nullptr;
  SDL_Texture* texture = nullptr;
  SDL_Renderer* renderer = nullptr;
  std::unique_ptr<Core> core;
};
} // natsukashii::frontend