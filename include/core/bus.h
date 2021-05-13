#pragma once
#include "ppu.h"

class Bus
{
public:
  Bus(bool skip);
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