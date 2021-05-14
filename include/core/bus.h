#pragma once
#include "ppu.h"

namespace natsukashii::core
{
struct Bus
{
  Bus(Mem& mem, bool skip, std::string path1, std::string path2);
  byte read_byte(half addr);
  byte next_byte(half addr, half& pc);
  void write_byte(half addr, byte val);
  half read_half(half addr);
  half next_half(half addr, half& pc);
  void write_half(half addr, half val);
  void load_rom(std::string filename);
  void load_bootrom(std::string filename);
  void reset();
  Mem mem;
  Ppu ppu;
};

}  // namespace natsukashii::core