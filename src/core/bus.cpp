#include "bus.h"

Bus::Bus(bool skip) : mem(skip), ppu(skip) {  }

void Bus::reset() {
    ppu.reset();
    mem.reset();
}

byte Bus::read_byte(half addr) {
    if(addr >= 0x8000 && addr <= 0x9fff)
        return ppu.vram[addr & 0x1fff];
    else if(addr >= 0xfe00 && addr <= 0xfe9f)
        return ppu.oam[addr & 0x9f];
    else if(addr >= 0xff40 && addr <= 0xff4b)
        return ppu.read_io(addr);
    
    return mem.read(addr);
}

byte Bus::next_byte(half addr, half& pc) {
    pc++;
    if(addr >= 0x8000 && addr <= 0x9fff)
        return ppu.vram[addr & 0x1fff];
    else if(addr >= 0xfe00 && addr <= 0xfe9f)
        return ppu.oam[addr & 0x9f];
    else if(addr >= 0xff40 && addr <= 0xff4b)
        return ppu.read_io(addr);
    
    return mem.read(addr);
}

half Bus::read_half(half addr) {
    if(addr >= 0x8000 && addr <= 0x9fff)
        return (ppu.vram[(addr & 0x1fff) + 1] << 8) | ppu.vram[addr & 0x1fff];
    else if(addr >= 0xfe00 && addr <= 0xfe9f)
        return (ppu.oam[(addr & 0x9f) + 1] << 8) | ppu.oam[addr & 0x9f];

    return (mem.read(addr + 1) << 8) | mem.read(addr);
}

half Bus::next_half(half addr, half& pc) {
    pc += 2;
    if(addr >= 0x8000 && addr <= 0x9fff)
        return (ppu.vram[(addr & 0x1fff) + 1] << 8) | ppu.vram[addr & 0x1fff];
    else if(addr >= 0xfe00 && addr <= 0xfe9f)
        return (ppu.oam[(addr & 0x9f) + 1] << 8) | ppu.oam[addr & 0x9f];

    return (mem.read(addr + 1) << 8) | mem.read(addr);
}

void Bus::write_byte(half addr, byte val) {
    if(addr >= 0x8000 && addr <= 0x9fff) {
        ppu.vram[addr & 0x1fff] = val;
        return;
    } else if(addr >= 0xfe00 && addr <= 0xfe9f) {
        ppu.oam[addr & 0x9f] = val;
        return;
    } else if(addr >= 0xff40 && addr <= 0xff4b) {
        ppu.write_io(mem, addr, val);
        return;
    }

    mem.write(addr, val);
}

void Bus::write_half(half addr, half val) {
    if(addr >= 0x8000 && addr <= 0x9fff) {
        ppu.vram[(addr & 0x1fff) + 1] = val >> 8;
        ppu.vram[addr & 0x1fff] = val & 0xff;
        return;
    } else if(addr >= 0xfe00 && addr <= 0xfe9f) {
        ppu.oam[(addr & 0x9f) + 1] = val >> 8;
        ppu.oam[addr & 0x9f] = val & 0xff;
        return;
    } else if(addr >= 0xff40 && addr <= 0xff4b) {
        ppu.write_io(mem, addr + 1, val >> 8);
        ppu.write_io(mem, addr, val & 0xff);
        return;
    }

    mem.write(addr + 1, val >> 8);
    mem.write(addr, val & 0xff);
}