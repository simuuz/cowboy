#include "bus.h"

namespace natsukashii::core
{
Bus::Bus(bool skip, std::string bootrom_path) : mem(skip, bootrom_path), ppu(skip)
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
  if (addr >= 0x8000 && addr <= 0x9fff)
    return ppu.vram[addr & 0x1fff];
  else if (addr >= 0xfe00 && addr <= 0xfe9f)
    return ppu.oam[addr & 0xff];
  else if (addr >= 0xff40 && addr <= 0xff4b)
    return ppu.ReadIO(addr);

  return mem.Read(addr);
}

u8 Bus::NextByte(u16 addr, u16& pc, u64& cycles)
{
  cycles+=4;
  pc++;
  if (addr >= 0x8000 && addr <= 0x9fff)
    return ppu.vram[addr & 0x1fff];
  else if (addr >= 0xfe00 && addr <= 0xfe9f)
    return ppu.oam[addr & 0xff];
  else if (addr >= 0xff40 && addr <= 0xff4b)
    return ppu.ReadIO(addr);

  return mem.Read(addr);
}

u16 Bus::ReadHalf(u16 addr)
{
  return (ReadByte(addr + 1) << 8) | ReadByte(addr);
}

u16 Bus::NextHalf(u16 addr, u16& pc, u64& cycles)
{
  return (NextByte(addr + 1, pc, cycles) << 8) | NextByte(addr, pc, cycles);
}

void Bus::WriteByte(u16 addr, u8 val)
{
  if (addr >= 0x8000 && addr <= 0x9fff)
  {
    ppu.vram[addr & 0x1fff] = val;
    return;
  }
  else if (addr >= 0xfe00 && addr <= 0xfe9f)
  {
    ppu.oam[addr & 0xff] = val;
    return;
  }
  else if (addr >= 0xff40 && addr <= 0xff4b)
  {
    ppu.WriteIO(mem, addr, val, mem.io.intf);
    return;
  }

  mem.Write(addr, val);
}

void Bus::WriteHalf(u16 addr, u16 val)
{
  WriteByte(addr + 1, val >> 8);
  WriteByte(addr, val);
}

}  // namespace natsukashii::core