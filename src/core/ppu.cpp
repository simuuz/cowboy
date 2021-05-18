#include "ppu.h"
#include <memory.h>

namespace natsukashii::core
{
Ppu::Ppu(bool skip, RenderWidget* renderer) : skip(skip), renderer(renderer)
{
  std::fill(pixels.begin(), pixels.end(), 0);
  std::fill(vram.begin(), vram.end(), 0);
  std::fill(oam.begin(), oam.end(), 0);

  io.scx = 0;
  io.scy = 0;
  io.lyc = 0;
  io.wx = 0;
  io.wy = 0;
  io.ly = 0;

  if (skip)
  {
    io.lcdc = 0x91;
    io.bgp = 0xfc;
    io.obp0 = 0xff;
    io.obp1 = 0xff;
  }
  else
  {
    io.lcdc = 0;
    io.bgp = 0;
    io.obp0 = 0;
    io.obp1 = 0;
  }
}

void Ppu::Reset()
{
  std::fill(pixels.begin(), pixels.end(), 0);
  std::fill(vram.begin(), vram.end(), 0);
  std::fill(oam.begin(), oam.end(), 0);

  io.scx = 0;
  io.scy = 0;
  io.lyc = 0;
  io.wx = 0;
  io.wy = 0;
  io.ly = 0;

  if (skip)
  {
    io.lcdc = 0x91;
    io.bgp = 0xfc;
    io.obp0 = 0xff;
    io.obp1 = 0xff;
  }
  else
  {
    io.lcdc = 0;
    io.bgp = 0;
    io.obp0 = 0;
    io.obp1 = 0;
  }
}

void Ppu::CompareLYC()
{
  bool lyc_comp = io.lyc == io.ly;
  setbit<byte, 2>(io.stat, lyc_comp);
  if (lyc_comp && bit<byte, 6>(io.stat))
  {
    statIRQ = true;
  }
}

void Ppu::Step(int cycles)
{
  if (disabled)
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
      ChangeMode(LCDTransfer);
    }
    break;
  case LCDTransfer:
    if (curr_cycles >= 172)
    {
      curr_cycles -= 172;
      ChangeMode(HBlank);
    }
    break;
  case HBlank:
    if (curr_cycles >= 204)
    {
      curr_cycles -= 204;
      io.ly++;
      if (io.ly == 0x90)
      {
        ChangeMode(VBlank);
      }
      else
      {
        ChangeMode(OAM);
      }

      CompareLYC();
    }
    break;
  case VBlank:
    if (curr_cycles >= 456)
    {
      curr_cycles -= 456;
      io.ly++;

      if (io.ly == 154)
      {
        ChangeMode(OAM);
        io.ly = 0;
      }

      CompareLYC();
    }
    break;
  }
}

void Ppu::ChangeMode(Mode m)
{
  mode = m;
  switch (m)
  {
  case LCDTransfer:
    io.stat = (io.stat & 0xfc) | 3;
    break;
  case HBlank:
    Scanline();
    io.stat = io.stat & 0xfc;
    if (bit<byte, 3>(io.stat))
    {
      statIRQ = true;
    }
    break;
  case OAM:
    io.stat = (io.stat & 0xfc) | 2;
    if (bit<byte, 5>(io.stat))
    {
      statIRQ = true;
    }
    break;
  case VBlank:
    renderer->DrawFrame(pixels.data(), 160, 144, 800, 600);
    vblankIRQ = true;
    io.stat = (io.stat & 0xfc) | 1;
    if (bit<byte, 4>(io.stat))
    {
      statIRQ = true;
    }
    break;
  }
}

byte Ppu::ReadIO(half addr)
{
  switch (addr & 0xff)
  {
  case 0x40:
    return io.lcdc;
  case 0x41:
    return io.stat;
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
    io.lcdc = val;
    if (!bit<byte, 7>(val))
    {
      io.stat &= 0xfc;
      disabled = true;
      io.ly = 0;
    }
    else if (disabled)
    {
      disabled = false;
      ChangeMode(OAM);
    }
    break;
  case 0x41:
    io.stat = (val & 0xF8) | (io.stat & 7);
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

bool Ppu::CanSprites(bool priority)
{
  if (!priority)
  {
    return true;
  }
  else
  {
    return pixels[fbIndex] == 0xe0 && pixels[fbIndex] == 0xf8 && pixels[fbIndex] == 0xd0;
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

void Ppu::Scanline()
{
  RenderBGs();
  //RenderOBJs();
}

void Ppu::RenderBGs()
{
  fbIndex = ((word)io.ly * WIDTH * 3);

  auto x = 0;
  auto y = (half)io.ly + (half)io.scy;

  auto wx = (shalf)io.wx - (half)7;
  auto wy = io.wy;

  bool renderWindow = (wy <= io.ly && bit<byte, 5>(io.lcdc));
  half bgtilemap = bit<byte, 3>(io.lcdc) ? 0x9c00 : 0x9800;
  half tiledata = bit<byte, 4>(io.lcdc) ? 0x8000 : 0x8800;
  half wintilemap = bit<byte, 6>(io.lcdc) ? 0x9c00 : 0x9800;

  while (x < 160)
  {
    half tileLine = 0;
    auto tile_x = 0;
    if (renderWindow && wx <= x && bit<byte, 0>(io.lcdc))
    {
      y = io.ly - io.wy;
      auto tmpx = x - wx;
      tile_x = tmpx & 7;
      auto tile_y = y & 7;
      byte tile_index = vram[wintilemap + ((((half)y >> 3) << 5) & 0x3FF) + (((half)tmpx >> 3))];
      if (tiledata == 0x8000)
      {
        tileLine = ReadVRAM<half>(tiledata + ((half)tile_index << 4) + ((half)tile_y << 1));
      }
      else
      {
        tileLine = ReadVRAM<half>(0x9000 + (shalf((sbyte)tile_index)) * 16 + ((half)tile_y << 1));
      }
    }
    else if (bit<byte, 0>(io.lcdc))
    {
      y = (half)io.ly + (half)io.scy;
      auto tile_y = y & 7;
      auto p = (x + io.scx);
      tile_x = p & 7;
      byte tile_index = vram[bgtilemap + (((y >> 3) << 5) & 0x3FF) + ((p >> 3) & 31)];
      if (tiledata == 0x8000)
      {
        tileLine = ReadVRAM<half>(tiledata + ((half)tile_index << 4) + ((half)tile_y << 1));
      }
      else
      {
        tileLine = ReadVRAM<half>(0x9000 + (shalf((sbyte)tile_index)) * 16 + ((half)tile_y << 1));
      }
    }

    byte high = (tileLine >> 8);
    byte low = (tileLine & 0xFF);

    byte colorID = (bit<byte>(high, 7 - tile_x) << 1) | bit<byte>(low, 7 - tile_x);
    byte color = (io.bgp >> (colorID << 1)) & 3;
    pixels[fbIndex] = palette[color * 3];
    pixels[fbIndex + 1] = palette[(color * 3) + 1];
    pixels[fbIndex + 2] = palette[(color * 3) + 2];

    x++;
    fbIndex += 3;
  }
}

void Ppu::RenderOBJs()
{
  if (!bit<byte, 1>(io.lcdc))
  {
    return;
  }

  byte screen_y = io.ly;

  std::vector<Sprite> sprites;
  byte sprite_size = bit<byte, 2>(io.lcdc) ? 16 : 8;
  for (int i = 0; i < 0xa0 && sprites.size() < 10; i += 4)
  {
    shalf sprite_startY = (shalf)oam[i] - 16;
    shalf sprite_endY = sprite_startY + sprite_size;
    if (sprite_startY <= screen_y && screen_y < sprite_endY)
    {
      sprites.push_back(Sprite(oam[i], oam[i + 1], oam[i + 2], oam[i + 3]));
    }
  }

  for (auto sprite : sprites)
  {
    if (sprite.x >= 0 && sprite.x <= 160)
    {
      int tile_y = (sprite.yflip) ? 7 - (screen_y - sprite.y) : ((screen_y - sprite.y) & 7);

      fbIndex = ((word)io.ly * 160 * 3 + (word)sprite.x * 4);
      for (int i = 0; i < 8; i++)
      {
        half tileLine = ReadVRAM<half>(0x8000 + ((half)sprite.tileNum << 4) + ((half)tile_y << 1));
        int tile_x = sprite.xflip ? 7 - i + io.scx : i + io.scx;
        byte hi = (tileLine >> 8);
        byte lo = (tileLine & 0xff);

        byte colorID = (bit<byte>(hi, 7 - tile_x) << 1) | bit<byte>(lo, 7 - tile_x);
        byte color = (io.bgp >> (colorID << 1)) & 3;

        if (CanSprites(sprite.priority) && sprite.x + i <= 160 && colorID != 0)
        {
          pixels[fbIndex] = palette[color * 3];
          pixels[fbIndex + 1] = palette[color * 3 + 1];
          pixels[fbIndex + 2] = palette[color * 3 + 2];
        }

        fbIndex += 3;
      }
    }
  }
}
}  // namespace natsukashii::core