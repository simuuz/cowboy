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

      u8 raw;
    };

    Attributes(u8 val) : raw(val) { }
  };
public:
  Sprite(u8 ypos = 0, u8 xpos = 0, u8 tileidx = 0, u8 attribs = 0)
        : ypos(ypos), xpos(xpos), tileidx(tileidx), attribs(attribs) { }
  u8 ypos, xpos, tileidx;
  Attributes attribs;
};

struct Sprites {
  std::array<Sprite, 10> s;
  u8 count = 0;
};

class Ppu
{
public:
  Ppu(bool skip);
  void Reset();
  void Step(u64 cycles, u8& intf);

  std::array<u32, FBSIZE> pixels;
  u32 color1, color2, color3, color4;
  std::array<u8, VRAM_SZ> vram;
  std::array<u8, OAM_SZ> oam;

  friend class Bus;
  bool render = false;
private:
  bool oam_lock = false;
  bool vram_lock = false;
  bool latch = false;
  bool reset = false;
  bool skip = false;

  std::array<u8, FBSIZE> indices;

  enum Mode
  {
    HBlank,
    VBlank,
    OAM,
    LCDTransfer
  };

  u32 GetColor(u8 idx);
  Mode mode = OAM;
  
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
      
      u8 raw;
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

      u8 raw;
    };

    STAT() : raw(0) {}

    void write(u8 value)
    {
      raw = value;
      raw |= 0x80;
    }

    u8 read()
    {
      return (1 << 7) | (raw & 0x7f);
    }
  };
  
  struct IO
  {
    u8 bgp = 0, scy = 0, scx = 0;
    LCDC lcdc, old_lcdc;
    u8 wx = 0, wy = 0, obp0 = 0, obp1 = 0;
    u8 lyc = 0, ly = 0;
    STAT stat;
  } io;

  u8 window_internal_counter = 0;
  u32 fbIndex = 0;

  u8 colorIDbg[FBSIZE];

  u64 curr_cycles = 0;

  Sprites sprites;

  void WriteIO(Mem& mem, u16 addr, u8 val, u8& intf);
  u8 ReadIO(u16 addr);
  template <typename T>
  void WriteVRAM(u16 addr, T val);
  template <typename T>
  T ReadVRAM(u16 addr);
  void ChangeMode(Mode m, u8& intf);
  Sprites FetchSprites();
  void RenderSprites();
  void RenderBGs();
  void Scanline();
  void CompareLYC(u8& intf);
};
}  // namespace natsukashii::core