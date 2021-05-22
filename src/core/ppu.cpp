#include "ppu.h"
#include <memory.h>
#include <time.h>

namespace natsukashii::core
{
Ppu::Ppu(bool skip) : skip(skip)
{
  srand(time(nullptr));
  pixels = new byte[FBSIZE] { 0 };
  vram.fill(0);
  oam.fill(0);

  io.scx = 0;
  io.scy = 0;
  io.lyc = 0;
  io.wx = 0;
  io.wy = 0;
  io.ly = 0;

  if (skip)
  {
    io.lcdc.raw = 0x91;
    io.bgp = 0xfc;
    io.obp0 = 0xff;
    io.obp1 = 0xff;
  }
  else
  {
    io.lcdc.raw = 0;
    io.bgp = 0;
    io.obp0 = 0;
    io.obp1 = 0;
  }
}

void Ppu::Reset()
{
  memset(pixels, 0, FBSIZE);
  vram.fill(0);
  oam.fill(0);

  io.scx = 0;
  io.scy = 0;
  io.lyc = 0;
  io.wx = 0;
  io.wy = 0;
  io.ly = 0;

  if (skip)
  {
    io.lcdc.raw = 0x91;
    io.bgp = 0xfc;
    io.obp0 = 0xff;
    io.obp1 = 0xff;
  }
  else
  {
    io.lcdc.raw = 0;
    io.bgp = 0;
    io.obp0 = 0;
    io.obp1 = 0;
  }
}

void Ppu::CompareLYC(byte& intf)
{
  io.stat.lyceq = io.lyc == io.ly;
  if (io.lyc == io.ly && io.stat.lyceq_int)
  {
    intf |= 2;
  }
}

void Ppu::Step(int cycles, byte& intf)
{
  if (!io.lcdc.enabled)
  {
    return;
  }

  curr_cycles += cycles;

  switch (mode)
  {
  case OAM:
    if (curr_cycles >= 80)
    {
      curr_cycles -= 80;
      ChangeMode(LCDTransfer, intf);
    }
    break;
  case LCDTransfer:
    if (curr_cycles >= 172)
    {
      curr_cycles -= 172;
      ChangeMode(HBlank, intf);
    }
    break;
  case HBlank:
    if (curr_cycles >= 204)
    {
      curr_cycles -= 204;
      io.ly++;
      if (io.ly == 0x90)
      {
        ChangeMode(VBlank, intf);
      }
      else
      {
        ChangeMode(OAM, intf);
      }

      CompareLYC(intf);
    }
    break;
  case VBlank:
    if (curr_cycles >= 456)
    {
      curr_cycles -= 456;
      io.ly++;

      if (io.ly == 154)
      {
        io.ly = 0;
        ChangeMode(OAM, intf);
      }

      CompareLYC(intf);
    }
    break;
  }
}

void Ppu::ChangeMode(Mode m, byte& intf)
{
  mode = m;
  switch (m)
  {
  case HBlank:
    if (io.stat.hblank_int)
    {
      intf |= 2;
    }
    break;
  case VBlank:
    render = true;
    intf |= 1;
    if (io.stat.vblank_int)
    {
      intf |= 2;
    }
    break;
  case OAM:
    if (io.stat.oam_int)
    {
      intf |= 2;
    }
    can_access_oam = false;
    break;
  case LCDTransfer:
    can_access_oam = false;
    can_access_vram = false;
    Scanline();
    break;
  }
}

byte Ppu::ReadIO(half addr)
{
  switch (addr & 0xff)
  {
  case 0x40:
    return io.lcdc.raw;
  case 0x41:
    return io.stat.read();
  case 0x42:
    return io.scy;
  case 0x43:
    return io.scx;
  case 0x44:
    return io.ly;
  case 0x45:
    return io.lyc;
  case 0x47:
    return io.bgp;
  case 0x48:
    return io.obp0;
  case 0x49:
    return io.obp1;
  case 0x4a:
    return io.wy;
  case 0x4b:
    return io.wx;
  }
}

void Ppu::WriteIO(Mem& mem, half addr, byte val)
{
  switch (addr & 0xff)
  {
  case 0x40:
    io.lcdc.raw = val;
    break;
  case 0x41:
    io.stat.write(val);
    break;
  case 0x42:
    io.scy = val;
    break;
  case 0x43:
    io.scx = val;
    break;
  case 0x45:
    io.lyc = val;
    break;
  case 0x46:  // OAM DMA very simple implementation
  {
    half start = (half)val << 8;
    for (byte i = 0; i < 0xa0; i++)
    {
      oam[i] = mem.Read(start + i);
    }
  }
  break;
  case 0x47:
    io.bgp = val;
    break;
  case 0x48:
    io.obp0 = val;
    break;
  case 0x49:
    io.obp1 = val;
    break;
  case 0x4a:
    io.wy = val;
    break;
  case 0x4b:
    io.wx = val;
    break;
  }
}

template <typename T>
void Ppu::WriteVRAM(half addr, T val)
{
  *reinterpret_cast<T*>(&(reinterpret_cast<byte*>(vram.data()))[addr & 0x1fff]) = val;
}

template <typename T>
T Ppu::ReadVRAM(half addr)
{
  return *reinterpret_cast<T*>(&(reinterpret_cast<byte*>(vram.data()))[addr & 0x1fff]);
}

void Ppu::SetColor(byte color)
{
  switch(color)
  {
  case 0:
    pixels[fbIndex] = 0xe0;
    pixels[fbIndex + 1] = 0xf8;
    pixels[fbIndex + 2] = 0xd0;
    break;
  case 1:
    pixels[fbIndex] = 0x88;
    pixels[fbIndex + 1] = 0xc0;
    pixels[fbIndex + 2] = 0x70;
    break;
  case 2:
    pixels[fbIndex] = 0x34;
    pixels[fbIndex + 1] = 0x68;
    pixels[fbIndex + 2] = 0x56;
    break;
  case 3:
    pixels[fbIndex] = 8;
    pixels[fbIndex + 1] = 0x18;
    pixels[fbIndex + 2] = 0x20;
    break;
  }
}

void Ppu::Scanline()
{
  RenderBGs();
  //RenderOBJs();
}

void Ppu::RenderBGs()
{
  for(int x = 0; x < WIDTH; x++) {
    fbIndex = x + WIDTH * io.ly;
    SetColor(x % 4);
    pixels[fbIndex + 3] = 0xff;
  }
}

void Ppu::RenderOBJs()
{
  
}
}  // namespace natsukashii::core