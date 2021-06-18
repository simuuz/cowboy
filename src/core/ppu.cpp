#include "ppu.h"
#include "ini.h"
#include <memory.h>
#include <algorithm>

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

  indices.fill(0);
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
  indices.fill(0);
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

void Ppu::OnEvent(Entry entry, Scheduler* scheduler, byte& intf)
{
  switch (mode)
  {
  case HBlank:
    CompareLYC(intf);

    if (io.ly == 0x90)
    {
      ChangeMode(entry, scheduler, VBlank, intf);
    }
    else
    {
      ChangeMode(entry, scheduler, OAM, intf);
    }
    break;
  case VBlank:
    if (io.ly == 154)
    {
      scheduler->push(Entry(PPU, entry.time + 452));
      io.ly = 0;
      window_internal_counter = 0;
      CompareLYC(intf);
    }
    break;
  case OAM:
    io.ly++;
    ChangeMode(entry, scheduler, LCDTransfer, intf);
    break;
  case LCDTransfer:
    ChangeMode(entry, scheduler, HBlank, intf);
    break;
  }
}

void Ppu::ChangeMode(Entry entry, Scheduler* scheduler, Mode m, byte& intf)
{
  mode = m;
  switch (m)
  {
  case HBlank:
    scheduler->push(Entry(PPU, entry.time + 204));
    render = true;
    Scanline();
    if (io.stat.hblank_int)
    {
      intf |= 2;
    }
    break;
  case VBlank:
    scheduler->push(Entry(PPU, entry.time +  456));
    intf |= 1;
    if (io.stat.vblank_int)
    {
      intf |= 2;
    }
    break;
  case OAM:
    scheduler->push(Entry(PPU, entry.time +  80));
    if (io.stat.oam_int)
    {
      intf |= 2;
    }
    break;
  case LCDTransfer:
    scheduler->push(Entry(PPU, entry.time + 172));
    break;
  }
}

void Ppu::ChangeMode(uint64_t time, Scheduler* scheduler, Mode m, byte& intf)
{
  mode = m;
  switch (m)
  {
  case HBlank:
    scheduler->push(Entry(PPU, time + 204));
    render = true;
    Scanline();
    if (io.stat.hblank_int)
    {
      intf |= 2;
    }
    break;
  case VBlank:
    scheduler->push(Entry(PPU, time +  456));
    intf |= 1;
    if (io.stat.vblank_int)
    {
      intf |= 2;
    }
    break;
  case OAM:
    scheduler->push(Entry(PPU, time +  80));
    if (io.stat.oam_int)
    {
      intf |= 2;
    }
    break;
  case LCDTransfer:
    scheduler->push(Entry(PPU, time + 172));
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

void Ppu::WriteIO(uint64_t time, Scheduler* scheduler, Mem& mem, half addr, byte val)
{
  switch (addr & 0xff)
  {
  case 0x40:
    io.lcdc.raw = val;
    if (io.lcdc.enabled) {
      curr_cycles = 0;
      ChangeMode(time, scheduler, OAM, mem.io.intf);
    }
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
  half bg_tilemap = io.lcdc.bg_tilemap_area == 1 ? 0x9C00 : 0x9800;
  half window_tilemap = io.lcdc.window_tilemap_area == 1 ? 0x9C00 : 0x9800;
  half tiledata = io.lcdc.bgwin_tiledata_area == 1 ? 0x8000 : 0x8800;

  bool render_window = (io.wy <= io.ly && io.lcdc.window_enable);

  for(int x = 0; x < WIDTH; x++)
  {
    half tileline = 0;
    sbyte scrolled_x = io.scx + x;
    sbyte scrolled_y = io.scy + io.ly;
    
    if(io.lcdc.bgwin_priority) {
      byte index = 0;
      shalf real_wx = (shalf)io.wx - 7;
      if(render_window && real_wx <= x)
      {
        scrolled_x = x - real_wx;
        scrolled_y = window_internal_counter;
        
        index = vram[(window_tilemap + (((scrolled_y >> 3) << 5) & 0x3FF) + ((scrolled_x >> 3) & 31)) & 0x1fff];
      }
      else
      {
        index = vram[(bg_tilemap + (((scrolled_y >> 3) << 5) & 0x3FF) + ((scrolled_x >> 3) & 31)) & 0x1fff];
      }

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

    byte colorID = ((byte)bit<byte>(high, 7 - (scrolled_x & 7)) << 1) | ((byte)bit<byte>(low, 7 - (scrolled_x & 7)));
    byte color_index = (io.bgp >> (colorID << 1)) & 3;
    colorIDbg[fbIndex] = colorID;
    pixels[fbIndex] = GetColor(color_index);
    fbIndex++;
  }
  
  if(render_window && io.ly >= io.wy && io.wx >= 0 && io.wx <= 166)
  {
    window_internal_counter++;
  }
}

std::vector<Sprite> Ppu::FetchSprites()
{
  std::vector<Sprite> sprites;
  
  byte height = io.lcdc.obj_size ? 16 : 8;

  for(int i = 0; i < 0xa0 && sprites.size() < 10; i+=4)
  {
    Sprite sprite(oam[i] - 16, oam[i + 1] - 8, oam[i + 2], oam[i + 3]);
    if(io.ly >= (shalf)sprite.ypos && io.ly < ((shalf)sprite.ypos + height))
    {
      sprites.push_back(sprite);
    }
  }

  std::stable_sort(sprites.begin(), sprites.end(), [](Sprite a, Sprite b) {
    return b.xpos <= a.xpos;
  });

  return sprites;
}

void Ppu::RenderSprites()
{
  if(!io.lcdc.obj_enable)
    return;

  std::vector<Sprite> sprites = FetchSprites();
  
  for(auto& sprite : sprites)
  {
    half tile_y;
    if(io.lcdc.obj_size)
    {
      tile_y = (sprite.attribs.yflip) ? ((io.ly - sprite.ypos) ^ 15) : io.ly - sprite.ypos;
    }
    else
    {
      tile_y = (sprite.attribs.yflip) ? ((io.ly - sprite.ypos) ^ 7) & 7 : (io.ly - sprite.ypos) & 7;
    }
    
    byte pal = (sprite.attribs.palnum) ? io.obp1 : io.obp0;
    fbIndex = sprite.xpos + WIDTH * io.ly;
    half tile_index = io.lcdc.obj_size ? sprite.tileidx & ~1 : sprite.tileidx;
    half tile = ReadVRAM<half>(0x8000 | ((tile_index << 4) + (tile_y << 1)));

    for(int x = 0; x < 8; x++)
    {
      sbyte tile_x = (sprite.attribs.xflip) ? 7 - x : x;
      byte high = tile >> 8;
      byte low = tile & 0xff;
      byte colorID = (bit<byte>(high, 7 - tile_x) << 1) | bit<byte>(low, 7 - tile_x);
      byte colorIndex = (pal >> (colorID << 1)) & 3;
      word color = GetColor(colorIndex);
      
      if((sprite.xpos + x) < 166 && colorID != 0 && pixels[fbIndex] != color)
      {
        if(sprite.attribs.obj_to_bg_prio)
        {
          if(colorIDbg[fbIndex] < 1)
          {
            pixels[fbIndex] = color;
          }
        }
        else
        {
          pixels[fbIndex] = color;
        }
      }

      fbIndex++;
    }
  }
}
}  // namespace natsukashii::core