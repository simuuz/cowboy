#pragma once
#include "cpu.h"

namespace natsukashii::core
{
struct Core
{
  Core(bool skip, std::string path1, std::string path2, RenderWidget* renderer);
  void Run();
  void Reset();
  Mem mem;
  Bus bus;
  Cpu cpu;
};
}  // namespace natsukashii::core