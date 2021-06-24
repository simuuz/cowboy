#pragma once
#include "cpu.h"
#include "bus.h"

namespace natsukashii::core
{
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
  bool pause = false;
  bool running = false;
  bool init = false;
};
}  // namespace natsukashii::core