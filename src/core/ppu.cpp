#include "ppu.h"

Ppu::Ppu(bool skip) : mem(skip) {
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

u8 Ppu::IO::read(u16 addr) {
    switch(addr & 0xff) {
        case 0x40: return lcdc;
        case 0x41: return stat;
        case 0x42: return scy;
        case 0x43: return scx;
        case 0x44: return ly;
        case 0x45: return lyc;
        case 0x47: return bgp;
        case 0x48: return obp0;
        case 0x49: return obp1;
        case 0x4a: return wy;
        case 0x4b: return wx;
    }
}

void Ppu::IO::write(Ppu& ppu, u16 addr, u8 val) {
    switch(addr & 0xff) {
        case 0x40: lcdc = val; break;
        case 0x41: stat = val; break;
        case 0x42: scy = val; break;
        case 0x43: scx = val; break;
        case 0x45: lyc = val; break;
        case 0x46: {              //OAM DMA very simple implementation
            u16 start = val << 8;
            for(int i = 0; i < 0xa0; i++)
                ppu.oam[i] = ppu.mem.read<u8>(start + i);
        }
        break;
        case 0x47: bgp = val; break;
        case 0x48: obp0 = val; break;
        case 0x49: obp1 = val; break;
        case 0x4a: wy = val; break;
        case 0x4b: wx = val; break;
    }
}