#include "bus.h"

Bus::Bus(bool skip) : ppu(skip), mem(skip) { }

u8 Bus::read_byte(u16 addr) {
    if(addr >= 0x8000 && addr <= 0x9fff)
        return ppu.vram[addr & 0x1fff];
    else if(addr >= 0xfe00 && addr <= 0xfe9f)
        return ppu.oam[addr & 0x9f];
    
    return mem.read<u8>(addr);
}

u8 Bus::next_byte(u16 addr, u16& pc) {
    pc++;
    if(addr >= 0x8000 && addr <= 0x9fff)
        return ppu.vram[addr & 0x1fff];
    else if(addr >= 0xfe00 && addr <= 0xfe9f)
        return ppu.oam[addr & 0x9f];

    return mem.read<u8>(addr);
}

u16 Bus::read_word(u16 addr) {
    if(addr >= 0x8000 && addr <= 0x9fff)
        return (ppu.vram[(addr & 0x1fff) + 1] << 8) | ppu.vram[addr & 0x1fff];
    else if(addr >= 0xfe00 && addr <= 0xfe9f)
        return (ppu.oam[(addr & 0x9f) + 1] << 8) | ppu.oam[addr & 0x9f];

    return mem.read<u16>(addr);
}

u16 Bus::next_word(u16 addr, u16& pc) {
    pc += 2;
    if(addr >= 0x8000 && addr <= 0x9fff)
        return (ppu.vram[(addr & 0x1fff) + 1] << 8) | ppu.vram[addr & 0x1fff];
    else if(addr >= 0xfe00 && addr <= 0xfe9f)
        return (ppu.oam[(addr & 0x9f) + 1] << 8) | ppu.oam[addr & 0x9f];

    return mem.read<u16>(addr);
}

void Bus::write_byte(u16 addr, u8 val) {
    if(addr >= 0x8000 && addr <= 0x9fff) {
        ppu.vram[addr & 0x1fff] = val;
        return;
    } else if(addr >= 0xfe00 && addr <= 0xfe9f) {
        ppu.oam[addr & 0x9f] = val;
        return;
    }
    mem.write<u8>(addr, val);
}

void Bus::write_word(u16 addr, u16 val) {
    if(addr >= 0x8000 && addr <= 0x9fff) {
        ppu.vram[(addr & 0x1fff) + 1] = val >> 8;
        ppu.vram[addr & 0x1fff] = val & 0xff;
    } else if(addr >= 0xfe00 && addr <= 0xfe9f) {
        ppu.oam[(addr & 0x9f) + 1] = val >> 8;
        ppu.oam[addr & 0x9f] = val & 0xff;
    }
    mem.write<u16>(addr, val);
}

void Bus::load_rom(std::string filename) {
    mem.load_rom(filename);
    rom_opened = true;
}

void Bus::load_bootrom(std::string filename) {
    mem.load_bootrom(filename);
}

void Bus::reset() {
    mem.reset();
    rom_opened = false;
}