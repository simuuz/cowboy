#pragma once
#include "mem.h"

constexpr int VRAM_SZ = 0x2000;
constexpr int OAM_SZ = 0xa0;
constexpr int WIDTH = 160;
constexpr int HEIGHT = 144;
constexpr int FBSIZE = WIDTH * HEIGHT;

namespace natsukashii::core
{
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
    };

    STAT() : mode(0), lyceq(0), hblank_int(0), vblank_int(0),
             oam_int(0), lyceq_int(0) {}

    void write(byte value)
    {
      hblank_int = value >> 4;
      vblank_int = value >> 5;
      oam_int = value >> 6;
      lyceq_int = value >> 7;
    }

    byte read()
    {
      return (1 << 7) | (lyceq_int << 6) | (oam_int << 5) | (vblank_int << 4) |
             (hblank_int << 3) | (lyceq << 2) | mode;
    }
  };
public:
  struct IO
  {
    byte bgp = 0, scy = 0, scx = 0;
    LCDC lcdc;
    byte wx = 0, wy = 0, obp0 = 0, obp1 = 0;
    byte lyc = 0, ly = 0;
    STAT stat;
  } io;
private:
  word fbIndex = 0;
  bool can_access_oam = true;
  bool can_access_vram = true;

  int curr_cycles = 0;

  void WriteIO(Mem& mem, half addr, byte val);
  byte ReadIO(half addr);
  template <typename T>
  void WriteVRAM(half addr, T val);
  template <typename T>
  T ReadVRAM(half addr);
  void ChangeMode(Mode m, byte& intf);
  bool CanSprites(bool priority);
  void RenderOBJs();
  void RenderBGs();
  void Scanline();
  void CompareLYC(byte& intf);
};
}  // namespace natsukashii::core