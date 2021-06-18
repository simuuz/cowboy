#pragma once
#include "ppu.h"

namespace natsukashii::core
{
struct Bus
{
  Bus(bool skip, std::string bootrom_path);
  byte ReadByte(half addr);
  byte NextByte(half addr, half& pc);
  void WriteByte(uint64_t time, Scheduler* scheduler, half addr, byte val);
  half ReadHalf(half addr);
  half NextHalf(half addr, half& pc);
  void WriteHalf(uint64_t time, Scheduler* scheduler, half addr, half val);
  void LoadROM(std::string filename);
  void Reset();
  bool romopened = false;
  Mem mem;
  Ppu ppu;
};

}  // namespace natsukashii::core