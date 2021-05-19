#pragma once
#include "ppu.h"

namespace natsukashii::core
{
struct Bus
{
  Bus(Mem& mem, bool skip, std::string bootrom_path);
  byte ReadByte(half addr);
  byte NextByte(half addr, half& pc);
  void WriteByte(half addr, byte val);
  half ReadHalf(half addr);
  half NextHalf(half addr, half& pc);
  void WriteHalf(half addr, half val);
  void LoadROM(std::string filename);
  void LoadBootROM(std::string filename);
  void Reset();
  Mem mem;
  Ppu ppu;
};

}  // namespace natsukashii::core