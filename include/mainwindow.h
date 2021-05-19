#pragma once
#include "core.h"
#include "ini.h"
#include "renderer.h"

struct MainWindow
{
  MainWindow(QApplication& app);
  void OnOpenFile();
  std::unique_ptr<natsukashii::core::Core> core;
  void Reset();
  void Pause();
  void Stop();
  bool running = false;
  Renderer* renderer;
};