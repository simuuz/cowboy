#pragma once
#include "mem.h"

constexpr int VRAM_SZ = 0x2000;
constexpr int OAM_SZ = 0xa0;

class Ppu {
public:
    Ppu(bool skip);
    void step(int cycles);
    void reset();

    byte pixels[160*144*3]{};
    byte background[256*256*3]{};
    byte window[256*256*3]{};
    byte vram[VRAM_SZ]{};
    byte oam[OAM_SZ]{};
    friend class Bus;
private:
    struct IO {
        byte bgp = 0, scy = 0, scx = 0, lcdc = 0;
        byte wx = 0, wy = 0, obp0 = 0, obp1 = 0;
        byte lyc = 0, ly = 0, stat = 0;
    } io;
    
    bool skip = false;
    void write_io(Mem& mem, half addr, byte val);
    byte read_io(half addr);

    void do_background();
    const byte palette[12] = {
        0x9f, 0xf4, 0xe5,
        0x00, 0xb9, 0xbe,
        0x00, 0x5f, 0x8c,
        0x00, 0x2b, 0x59
    };
};