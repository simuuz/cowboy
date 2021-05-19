#pragma once
#include "cpu.h"

namespace natsukashii::core
{
struct Core
{
  Core(bool skip, std::string bootrom_path);
  void Run();
  void Reset();
  void Pause();
  void Stop();
  void LoadROM(std::string path);
  Mem mem;
  Bus bus;
  Cpu cpu;
private:
  bool canrun = false;
};
}  // namespace natsukashii::core