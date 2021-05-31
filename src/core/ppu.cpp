#include "ppu.h"
#include "ini.h"
#include <memory.h>

namespace natsukashii::core
{
Ppu::Ppu(bool skip) : skip(skip)
{
  mINI::INIFile file{"config.ini"};
  mINI::INIStructure ini;

  if (!file.read(ini))
  {
    ini["emulator"]["skip"] = "false";
    ini["emulator"]["bootrom"] = "bootrom.bin";
    ini["palette"]["color1"] = "e0f8d0ff";
    ini["palette"]["color2"] = "88c070ff";
    ini["palette"]["color3"] = "346856ff";
    ini["palette"]["color4"] = "81820ff";
    file.generate(ini);
  }

  color1 = std::stoul(ini["palette"]["color1"], nullptr, 16);
  color2 = std::stoul(ini["palette"]["color2"], nullptr, 16);
  color3 = std::stoul(ini["palette"]["color3"], nullptr, 16);
  color4 = std::stoul(ini["palette"]["color4"], nullptr, 16);

  pixels.fill(color4);
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
  fbIndex = 0;
  mode = OAM;
  pixels.fill(color4);
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
      if(io.lcdc.window_enable)
      {
        window_internal_counter++;
      }
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
    break;
  case LCDTransfer:
    render = true;
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
  default:
    return 0xff;
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
      oam[i] = mem.Read(start | i);
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
  default:
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

word Ppu::GetColor(byte idx)
{
  switch(idx)
  {
  case 0:
    return color1;
  case 1:
    return color2;
  case 2:
    return color3;
  case 3:
    return color4;
  }
}

void Ppu::Scanline()
{
  RenderBGs();
  RenderSprites();
}

void Ppu::RenderBGs()
{
  fbIndex = io.ly * WIDTH;
  half bg_tilemap = io.lcdc.bg_tilemap_area ? 0x9C00 : 0x9800;
  half window_tilemap = io.lcdc.window_tilemap_area ? 0x9C00 : 0x9800;
  half tiledata = io.lcdc.bgwin_tiledata_area ? 0x8000 : 0x8800;

  bool render_window = (io.wy <= io.ly && io.lcdc.window_enable);
  shalf winx = (shalf)(io.wx - 7);
  for(int x = 0; x < WIDTH; x++)
  {
    half tileline = 0;
    half scrolled_x = io.scx + x;
    half scrolled_y = io.scy + io.ly;
    
    if(io.lcdc.bgwin_priority) {
      byte index = vram[(bg_tilemap + (((scrolled_y >> 3) << 5) & 0x3FF) + ((scrolled_x >> 3) & 31)) & 0x1fff];

      if(tiledata == 0x8000)
      {
        tileline = ReadVRAM<half>(tiledata + ((half)index << 4) + ((half)(scrolled_y & 7) << 1));
      }
      else
      {
        tileline = ReadVRAM<half>(0x9000 + shalf((sbyte)index) * 16 + ((half)(scrolled_y & 7) << 1));
      }
    } else if (io.lcdc.bgwin_priority && render_window && winx <= x) {
      scrolled_x = x - winx;
      scrolled_y = window_internal_counter - io.wy;

      byte index = vram[(window_tilemap + (((scrolled_y >> 3) << 5) & 0x3FF) + ((scrolled_x >> 3) & 31)) & 0x1fff];

      if(tiledata == 0x8000)
      {
        tileline = ReadVRAM<half>(tiledata + ((half)index << 4) + ((half)(scrolled_y & 7) << 1));
      }
      else
      {
        tileline = ReadVRAM<half>(0x9000 + shalf((sbyte)index) * 16 + ((half)(scrolled_y & 7) << 1));
      }
    }

    byte high = (tileline >> 8);
    byte low = (tileline & 0xff);

    byte color = ((byte)bit<byte>(high, 7 - (scrolled_x & 7)) << 1) | ((byte)bit<byte>(low, 7 - (scrolled_x & 7)));
    byte coloridx = (io.bgp >> (color << 1)) & 3;
    pixels[fbIndex] = GetColor(coloridx);
    fbIndex++;
  }
}

void Ppu::RenderSprites()
{
  if(!io.lcdc.obj_enable)
    return;
}
}  // namespace natsukashii::core