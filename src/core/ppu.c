#include "ppu.h"

void init_ppu(ppu_t* ppu, bool skip) {
    if(skip) {
        ppu->io.lcdc = 0x91; ppu->io.scx = 0; ppu->io.scy = 0; ppu->io.lyc = 0;
        ppu->io.bgp = 0xfc; ppu->io.obp0 = 0xff; ppu->io.obp1 = 0xff;
        ppu->io.wx = 0; ppu->io.wy = 0; ppu->io.ly = 0x90;
    } else {
        ppu->io.lcdc = 0; ppu->io.scx = 0; ppu->io.scy = 0; ppu->io.lyc = 0;
        ppu->io.bgp = 0; ppu->io.obp0 = 0; ppu->io.obp1 = 0;
        ppu->io.wx = 0; ppu->io.wy = 0; ppu->io.ly = 0;
    }
}

void step(ppu_t* ppu, int cycles) {
    
}

uint8_t read_io_ppu(ppu_io* io, uint16_t addr) {
    switch(addr & 0xff) {
        case 0x40: return io->lcdc;
        case 0x41: return io->stat;
        case 0x42: return io->scy;
        case 0x43: return io->scx;
        case 0x44: return 0x90;
        case 0x45: return io->lyc;
        case 0x47: return io->bgp;
        case 0x48: return io->obp0;
        case 0x49: return io->obp1;
        case 0x4a: return io->wy;
        case 0x4b: return io->wx;
    }
}

void write_io_ppu(ppu_t* ppu, mem_t* mem, uint16_t addr, uint8_t val) {
    switch(addr & 0xff) {
        case 0x40: ppu->io.lcdc = val; break;
        case 0x41: ppu->io.stat = val; break;
        case 0x42: ppu->io.scy = val; break;
        case 0x43: ppu->io.scx = val; break;
        case 0x45: ppu->io.lyc = val; break;
        case 0x46: {              //OAM DMA very simple implementation
            uint16_t start = val << 8;
            for(int i = 0; i < 0xa0; i++)
                ppu->oam[i] = read(mem, start + i);
        }
        break;
        case 0x47: ppu->io.bgp = val; break;
        case 0x48: ppu->io.obp0 = val; break;
        case 0x49: ppu->io.obp1 = val; break;
        case 0x4a: ppu->io.wy = val; break;
        case 0x4b: ppu->io.wx = val; break;
    }
}