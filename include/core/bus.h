#pragma once
#include "ppu.h"

namespace natsukashii::core
{
class Bus
{
public:
  Bus(Mem& mem, bool skip, std::string path1, std::string path2);
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