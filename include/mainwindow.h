#pragma once
#include "core.h"
#include "ini.h"
#include "renderer.h"
#include <nfd.hpp>
#include <chrono>

namespace natsukashii::frontend
{
using namespace natsukashii::core;
using namespace natsukashii::renderer;

struct MainWindow
{
  MainWindow(unsigned int w, unsigned h, std::string title);
  ~MainWindow() { NFD_Quit(); }
  void Run();
  void OpenFile();
  void Reset();
  void Pause();
  void Stop();
  void MenuBar();
  bool running = false;
  sf::RenderWindow window;
  Renderer renderer;
  std::unique_ptr<Core> core;
};
} // natsukashii::frontend