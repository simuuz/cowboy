#pragma once
#include "debugwindow.h"

namespace natsukashii::core
{
using namespace natsukashii::frontend;
struct Core
{
  Core(bool skip, std::string bootrom_path);
  void Run(int key, int action);
  void Reset();
  void Pause();
  void Stop();
  void LoadROM(std::string path);
  Bus bus;
  Cpu cpu;
  bool debug = false;
  bool pause = false;
  bool running = false;
  bool init = false;
};
}  // namespace natsukashii::core