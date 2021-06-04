#pragma once
#include "mem.h"

constexpr int VRAM_SZ = 0x2000;
constexpr int OAM_SZ = 0xa0;
constexpr int WIDTH = 160;
constexpr int HEIGHT = 144;
constexpr int FBSIZE = WIDTH * HEIGHT;

namespace natsukashii::core
{
struct Sprite
{
private:
  struct Attributes
  {
    union
    {
      struct
      {
        unsigned:4;
        unsigned palnum:1;
        unsigned xflip:1;
        unsigned yflip:1;
        unsigned obj_to_bg_prio:1;
      };

      byte raw;
    };

    Attributes(byte val) : raw(val) { }
  };
public:
  Sprite(byte ypos = 0, byte xpos = 0, byte tileidx = 0, byte attribs = 0)
        : ypos(ypos), xpos(xpos), tileidx(tileidx), attribs(attribs) { }
  byte ypos, xpos, tileidx;
  Attributes attribs;
};

class Ppu
{
public:
  Ppu(bool skip);
  void Reset();
  void Step(int cycles, byte& intf);

  std::array<word, FBSIZE> pixels;
  word color1, color2, color3, color4;
  std::array<byte, VRAM_SZ> vram;
  std::array<byte, OAM_SZ> oam;

  friend class Bus;
  bool render = false;
private:
  bool latch = false;
  bool reset = false;
  bool skip = false;
  enum Mode
  {
    HBlank,
    VBlank,
    OAM,
    LCDTransfer
  };

  word GetColor(byte idx);
  Mode mode = OAM;

  byte coloridx_bg = 0, coloridx_sprite = 0;
  
  struct LCDC
  {
    union
    {
      struct
      {
        unsigned bgwin_priority:1;
        unsigned obj_enable:1;
        unsigned obj_size:1;
        unsigned bg_tilemap_area:1;
        unsigned bgwin_tiledata_area:1;
        unsigned window_enable:1;
        unsigned window_tilemap_area:1;
        unsigned enabled:1;
      };
      
      byte raw;
    };

    LCDC() : raw(0) {}
  };

  struct STAT
  {
    union
    {
      struct
      {
        unsigned mode:2;
        unsigned lyceq:1;
        unsigned hblank_int:1;
        unsigned vblank_int:1;
        unsigned oam_int:1;
        unsigned lyceq_int:1;
        unsigned:1;
      };

      byte raw;
    };

    STAT() : raw(0) {}

    void write(byte value)
    {
      raw = value;
      raw |= 0x80;
    }

    byte read()
    {
      return (1 << 7) | (raw & 0x7f);
    }
  };
  
  struct IO
  {
    byte bgp = 0, scy = 0, scx = 0;
    LCDC lcdc;
    byte wx = 0, wy = 0, obp0 = 0, obp1 = 0;
    byte lyc = 0, ly = 0;
    STAT stat;
  } io;

  byte window_internal_counter = 0;
  word fbIndex = 0;

  int curr_cycles = 0;

  void WriteIO(Mem& mem, half addr, byte val);
  byte ReadIO(half addr);
  template <typename T>
  void WriteVRAM(half addr, T val);
  template <typename T>
  T ReadVRAM(half addr);
  void ChangeMode(Mode m, byte& intf);
  bool CanSprites(bool priority);
  void RenderSprites();
  void RenderBGs();
  void Scanline();
  void CompareLYC(byte& intf);
};
}  // namespace natsukashii::core