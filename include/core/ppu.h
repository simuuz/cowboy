#pragma once
#include "mem.h"
#include "renderer.h"

constexpr int VRAM_SZ = 0x2000;
constexpr int OAM_SZ = 0xa0;
constexpr int WIDTH = 160;
constexpr int HEIGHT = 144;
constexpr int FBSIZE = WIDTH * HEIGHT * 4;

namespace natsukashii::core
{
struct Sprite
{
  shalf x, y;
  byte tileNum;
  byte lowerByte;
  bool priority, xflip, yflip, palNum;

  Sprite(shalf x, shalf y, byte tileNum, byte attr) : tileNum(tileNum)
  {
    this->y = y - 16;
    this->x = x - 8;
    lowerByte = 0;
    priority = bit<byte, 7>(attr);
    yflip = bit<byte, 6>(attr);
    xflip = bit<byte, 5>(attr);
    palNum = bit<byte, 4>(attr);
  }

  byte Read(byte addr)
  {
    switch(addr)
    {
      case 0: return y;
      case 1: return x;
      case 2: return tileNum;
      case 3: return (priority << 7) | (yflip << 6) | (xflip << 5) | (palNum << 4) | lowerByte;
    }
  }

  void Write(byte addr, byte val)
  {
    switch(addr)
    {
      case 0: y = val; break;
      case 1: x = val; break;
      case 2: tileNum = val; break;
      case 3:
        priority = val & 0x80 == 0;
        yflip = val & 0x40;
        xflip = val & 0x20;
        palNum = val & 0x10;
        lowerByte = val & 0xf;
        break;
    }
  }
};

class Ppu
{
public:
  Ppu(bool skip);
  void Reset();
  void Step(int cycles);

  byte* pixels = nullptr;

  std::array<byte, VRAM_SZ> vram;
  std::array<Sprite, OAM_SZ> oam;
  friend class Bus;
  bool render = false;
private:
  bool skip = false;
  enum Mode
  {
    HBlank,
    VBlank,
    OAM,
    LCDTransfer
  };

  void SetColor(byte color);
  Mode mode = OAM;
  
  struct IO
  {
    byte bgp = 0, scy = 0, scx = 0, lcdc = 0;
    byte wx = 0, wy = 0, obp0 = 0, obp1 = 0;
    byte lyc = 0, ly = 0, stat = 0;
  } io;

  bool statIRQ = false;
  bool vblankIRQ = false;
  word fbIndex = 0;
  bool disabled = true;

  int curr_cycles = 0;

  void WriteIO(Mem& mem, half addr, byte val);
  byte ReadIO(half addr);
  template <typename T>
  void WriteVRAM(half addr, T val);
  template <typename T>
  T ReadVRAM(half addr);
  void ChangeMode(Mode m);
  bool CanSprites(bool priority);
  void RenderOBJs();
  void RenderBGs();
  void Scanline();
  void CompareLYC();
};
}  // namespace natsukashii::core