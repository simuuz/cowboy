#pragma once
#include "ppu.h"
#include "apu.h"

namespace natsukashii::core
{
struct Bus
{
  Bus(bool skip, std::string bootrom_path);
  u8 ReadByte(u16 addr);
  u8 NextByte(u16 addr, u16& pc, u64& cycles);
  void WriteByte(u16 addr, u8 val);
  u16 ReadHalf(u16 addr);
  u16 NextHalf(u16 addr, u16& pc, u64& cycles);
  void WriteHalf(u16 addr, u16 val);
  void LoadROM(std::string filename);
  void Reset();
  bool romopened = false;
  Mem mem;
  Ppu ppu;
  Apu apu;
};

}  // namespace natsukashii::core