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

byte Bus::ReadByte(half addr)
{
  if (addr >= 0x8000 && addr <= 0x9fff)
    return ppu.vram[addr & 0x1fff];
  else if (addr >= 0xfe00 && addr <= 0xfe9f)
    return ppu.oam[addr & 0xff];
  else if (addr >= 0xff40 && addr <= 0xff4b)
    return ppu.ReadIO(addr);

  return mem.Read(addr);
}

byte Bus::NextByte(half addr, half& pc)
{
  pc++;
  if (addr >= 0x8000 && addr <= 0x9fff)
    return ppu.vram[addr & 0x1fff];
  else if (addr >= 0xfe00 && addr <= 0xfe9f)
    return ppu.oam[addr & 0xff];
  else if (addr >= 0xff40 && addr <= 0xff4b)
    return ppu.ReadIO(addr);

  return mem.Read(addr);
}

half Bus::ReadHalf(half addr)
{
  if (addr >= 0x8000 && addr <= 0x9fff)
    return (ppu.vram[(addr & 0x1fff) + 1] << 8) | ppu.vram[addr & 0x1fff];
  else if (addr >= 0xfe00 && addr <= 0xfe9f)
    return (ppu.oam[(addr & 0xff) + 1] << 8) | ppu.oam[addr & 0xff];

  return (mem.Read(addr + 1) << 8) | mem.Read(addr);
}

half Bus::NextHalf(half addr, half& pc)
{
  pc += 2;
  if (addr >= 0x8000 && addr <= 0x9fff)
    return (ppu.vram[(addr & 0x1fff) + 1] << 8) | ppu.vram[addr & 0x1fff];
  else if (addr >= 0xfe00 && addr <= 0xfe9f)
    return (ppu.oam[(addr & 0xff) + 1] << 8) | ppu.oam[addr & 0xff];

  return (mem.Read(addr + 1) << 8) | mem.Read(addr);
}

void Bus::WriteByte(half addr, byte val)
{
  if (addr >= 0x8000 && addr <= 0x9fff)
  {
    ppu.vram[addr & 0x1fff] = val;
    return;
  }
  else if (addr >= 0xfe00 && addr <= 0xfe9f)
  {
    ppu.oam[addr & 0x9f] = val;
    return;
  }
  else if (addr >= 0xff40 && addr <= 0xff4b)
  {
    ppu.WriteIO(mem, addr, val);
    return;
  }

  mem.Write(addr, val);
}

void Bus::WriteHalf(half addr, half val)
{
  if (addr >= 0x8000 && addr <= 0x9fff)
  {
    ppu.vram[(addr & 0x1fff) + 1] = val >> 8;
    ppu.vram[addr & 0x1fff] = val & 0xff;
    return;
  }
  else if (addr >= 0xfe00 && addr <= 0xfe9f)
  {
    ppu.oam[(addr & 0xff) + 1] = val >> 8;
    ppu.oam[addr & 0xff] = val & 0xff;
    return;
  }
  else if (addr >= 0xff40 && addr <= 0xff4b)
  {
    ppu.WriteIO(mem, addr + 1, val >> 8);
    ppu.WriteIO(mem, addr, val & 0xff);
    return;
  }

  mem.Write(addr + 1, val >> 8);
  mem.Write(addr, val & 0xff);
}

}  // namespace natsukashii::core