#include "ppu.h"
#include <memory.h>

Ppu::Ppu(bool skip) : skip(skip) {
    memset(background, 0, sizeof(background));
    memset(window, 0, sizeof(window));
    memset(pixels, 0, sizeof(pixels));
    memset(vram, 0, VRAM_SZ);
    memset(oam, 0, OAM_SZ);
    
    if(skip) {
        io.lcdc = 0x91; io.scx = 0; io.scy = 0; io.lyc = 0;
        io.bgp = 0xfc; io.obp0 = 0xff; io.obp1 = 0xff;
        io.wx = 0; io.wy = 0; io.ly = 0x90;
    } else {
        io.lcdc = 0; io.scx = 0; io.scy = 0; io.lyc = 0;
        io.bgp = 0; io.obp0 = 0; io.obp1 = 0;
        io.wx = 0; io.wy = 0; io.ly = 0;
    }
}

void Ppu::reset() {
    memset(background, 0, sizeof(background));
    memset(window, 0, sizeof(window));
    memset(pixels, 0, sizeof(pixels));
    memset(vram, 0, VRAM_SZ);
    memset(oam, 0, OAM_SZ);

    if(skip) {
        io.lcdc = 0x91; io.scx = 0; io.scy = 0; io.lyc = 0;
        io.bgp = 0xfc; io.obp0 = 0xff; io.obp1 = 0xff;
        io.wx = 0; io.wy = 0; io.ly = 0x90;
    } else {
        io.lcdc = 0; io.scx = 0; io.scy = 0; io.lyc = 0;
        io.bgp = 0; io.obp0 = 0; io.obp1 = 0;
        io.wx = 0; io.wy = 0; io.ly = 0;
    }
}

void Ppu::step(int cycles) {
    
}

byte Ppu::read_io(half addr) {
    switch(addr & 0xff) {
        case 0x40: return io.lcdc;
        case 0x41: return io.stat;
        case 0x42: return io.scy;
        case 0x43: return io.scx;
        case 0x44: return 0x90;
        case 0x45: return io.lyc;
        case 0x47: return io.bgp;
        case 0x48: return io.obp0;
        case 0x49: return io.obp1;
        case 0x4a: return io.wy;
        case 0x4b: return io.wx;
    }
}

void Ppu::write_io(Mem& mem, half addr, byte val) {
    switch(addr & 0xff) {
        case 0x40: io.lcdc = val; break;
        case 0x41: io.stat = val; break;
        case 0x42: io.scy = val; break;
        case 0x43: io.scx = val; break;
        case 0x45: io.lyc = val; break;
        case 0x46: {              //OAM DMA very simple implementation
            half start = val << 8;
            for(int i = 0; i < 0xa0; i++)
                oam[i] = mem.read(start + i);
        }
        break;
        case 0x47: io.bgp = val; break;
        case 0x48: io.obp0 = val; break;
        case 0x49: io.obp1 = val; break;
        case 0x4a: io.wy = val; break;
        case 0x4b: io.wx = val; break;
    }
}