#pragma once
#include "ppu.h"

namespace natsukashii::core
{
struct Bus
{
  Bus(Mem& mem, bool skip, std::string path1, std::string path2, RenderWidget* renderer);
  byte ReadByte(half addr);
  byte NextByte(half addr, half& pc);
  void WriteByte(half addr, byte val);
  half ReadHalf(half addr);
  half NextHalf(half addr, half& pc);
  void WriteHalf(half addr, half val);
  void LoadRom(std::string filename);
  void LoadBootrom(std::string filename);
  void Reset();
  Mem mem;
  Ppu ppu;
};

}  // namespace natsukashii::core