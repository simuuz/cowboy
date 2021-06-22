#include "bus.h"

namespace natsukashii::core
{
Bus::Bus(bool skip, std::string bootrom_path) : mem(skip, bootrom_path), ppu(skip), apu(skip)
{
}

void Bus::LoadROM(std::string path)
{
  this->mem.LoadROM(path);
  romopened = mem.rom_opened;
}

void Bus::Reset()
{
  ppu.Reset();
  mem.Reset();
}

u8 Bus::ReadByte(u16 addr)
{
  switch(addr) {
  case 0x8000 ... 0x9fff:
    return ppu.vram[addr & 0x1fff];
  case 0xfe00 ... 0xfe9f:
    return ppu.oam[addr & 0xff];
  case 0xff40 ... 0xff4b:
    return ppu.ReadIO(addr);
  case 0xff10 ... 0xff3f:
    return apu.ReadIO(addr);
  default:
    return mem.Read(addr);
  }
}

u8 Bus::NextByte(u16 addr, u16& pc, u8& cycles)
{
  cycles += 4;
  pc++;

  return ReadByte(addr);
}

u16 Bus::ReadHalf(u16 addr)
{
  return (ReadByte(addr + 1) << 8) | ReadByte(addr);
}

u16 Bus::NextHalf(u16 addr, u16& pc, u8& cycles)
{
  return (NextByte(addr + 1, pc, cycles) << 8) | NextByte(addr, pc, cycles);
}

void Bus::WriteByte(u16 addr, u8 val)
{
  switch(addr) {
  case 0x8000 ... 0x9fff:
    ppu.vram[addr & 0x1fff] = val;
    break;
  case 0xfe00 ... 0xfe9f:
    ppu.oam[addr & 0xff] = val;
    break;
  case 0xff40 ... 0xff4b:
    ppu.WriteIO(mem, addr, val, mem.io.intf);
    break;
  case 0xff10 ... 0xff3f:
    apu.WriteIO(addr, val);
    break;
  default:
    mem.Write(addr, val);
  }
}

void Bus::WriteHalf(u16 addr, u16 val)
{
  WriteByte(addr + 1, val >> 8);
  WriteByte(addr, val);
}

}  // namespace natsukashii::core