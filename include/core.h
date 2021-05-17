#pragma once
#include "cpu.h"

namespace natsukashii::core
{
struct Core
{
  Core(bool skip, std::string path1, std::string path2);
  void Run();
  void Reset();
  void Pause();
  void Stop();
  bool run = true;
  Mem mem;
  Bus bus;
  Cpu cpu;
};
}  // namespace natsukashii::core