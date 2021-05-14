#pragma once
#include "mem.h"

constexpr int VRAM_SZ = 0x2000;
constexpr int OAM_SZ = 0xa0;
constexpr int WIDTH = 160;
constexpr int HEIGHT = 144;
constexpr int FBSIZE = WIDTH * HEIGHT * 3;

namespace natsukashii::core
{
struct Sprite
{
  shalf x, y;
  byte tileNum;
  bool priority, xflip, yflip, palNum;

  Sprite(shalf x, shalf y, byte tileNum, byte attr) : tileNum(tileNum)
  {
    this->y = y - 16;
    this->x = x - 8;
    priority = bit<byte, 7>(attr);
    yflip = bit<byte, 6>(attr);
    xflip = bit<byte, 5>(attr);
    palNum = bit<byte, 4>(attr);
  }
};

class Ppu
{
public:
  Ppu(bool skip);
  void reset();
  void step(int cycles);
  bool render = false;

  byte pixels[FBSIZE]{};
  byte vram[VRAM_SZ]{};
  byte oam[OAM_SZ]{};
  friend class Bus;

private:
  enum Mode
  {
    HBlank,
    VBlank,
    OAM,
    LCDTransfer
  } mode;

  struct IO
  {
    byte bgp = 0, scy = 0, scx = 0, lcdc = 0;
    byte wx = 0, wy = 0, obp0 = 0, obp1 = 0;
    byte lyc = 0, ly = 0, stat = 0;
  } io;

  bool statIRQ = false;
  bool vblankIRQ = false;
  word fbIndex = 0;
  bool disabled = false;

  int curr_cycles = 0;

  const byte palette[12] = {0xe0, 0xf8, 0xd0, 0x88, 0xc0, 0x70, 0x34, 0x68, 0x56, 0x08, 0x18, 0x20};

  bool skip = false;
  void write_io(Mem& mem, half addr, byte val);
  byte read_io(half addr);
  template <typename T>
  void write_vram(half addr, T val);
  template <typename T>
  T read_vram(half addr);
  void change_mode(Mode m);
  bool can_we_has_sprite(bool priority);
  void renderOBJs();
  void renderBGs();
  void scanline();
};
}  // namespace natsukashii::core