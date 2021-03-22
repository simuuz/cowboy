#pragma once
#include "mem.h"

constexpr int VRAM_SZ = 0x2000;
constexpr int OAM_SZ = 0xa0;

class Ppu {
public:
    Ppu(bool skip);
    void step(int cycles);

    u8 pixels[160*144*3]{};
    u8 background[256*256*3]{};
    u8 window[256*256*3]{};
    u8 vram[VRAM_SZ]{};
    u8 oam[OAM_SZ]{};
private:
    Mem mem;
    struct IO {
        u8 bgp = 0, scy = 0, scx = 0, lcdc = 0;
        u8 wx = 0, wy = 0, obp0 = 0, obp1 = 0;
        u8 lyc = 0, ly = 0, stat = 0;
        u8 read(u16 addr);
        void write(Ppu& ppu, u16 addr, u8 val);
    } io;

    void do_background();
    const u8 palette[12] = {
        0x9f, 0xf4, 0xe5,
        0x00, 0xb9, 0xbe,
        0x00, 0x5f, 0x8c,
        0x00, 0x2b, 0x59
    };
};