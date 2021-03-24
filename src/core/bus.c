#include "bus.h"

void init_bus(bus_t* bus, bool skip) {
    bus->rom_opened = false;
    init_mem(bus->mem, skip);
    init_ppu(bus->ppu, skip);
}

uint8_t read_byte(bus_t* bus, uint16_t addr) {
    if(addr >= 0x8000 && addr <= 0x9fff)
        return bus->ppu->vram[addr & 0x1fff];
    else if(addr >= 0xfe00 && addr <= 0xfe9f)
        return bus->ppu->oam[addr & 0x9f];
    else if(addr >= 0xff40 && addr <= 0xff4b)
        return read_io_ppu(&bus->ppu->io, addr);
    
    return read(&bus->mem, addr);
}

uint8_t next_byte(bus_t* bus, uint16_t addr, uint16_t* pc) {
    (*pc)++;
    if(addr >= 0x8000 && addr <= 0x9fff)
        return bus->ppu->vram[addr & 0x1fff];
    else if(addr >= 0xfe00 && addr <= 0xfe9f)
        return bus->ppu->oam[addr & 0x9f];
    else if(addr >= 0xff40 && addr <= 0xff4b)
        return read_io_ppu(&bus->ppu->io, addr);
    
    return read(&bus->mem, addr);
}

uint16_t read_word(bus_t* bus, uint16_t addr) {
    if(addr >= 0x8000 && addr <= 0x9fff)
        return (bus->ppu->vram[(addr & 0x1fff) + 1] << 8) | bus->ppu->vram[addr & 0x1fff];
    else if(addr >= 0xfe00 && addr <= 0xfe9f)
        return (bus->ppu->oam[(addr & 0x9f) + 1] << 8) | bus->ppu->oam[addr & 0x9f];

    return (read(&bus->mem, addr + 1) << 8) | read(&bus->mem, addr);
}

uint16_t next_word(bus_t* bus, uint16_t addr, uint16_t* pc) {
    pc += 2;
    if(addr >= 0x8000 && addr <= 0x9fff)
        return (bus->ppu->vram[(addr & 0x1fff) + 1] << 8) | bus->ppu->vram[addr & 0x1fff];
    else if(addr >= 0xfe00 && addr <= 0xfe9f)
        return (bus->ppu->oam[(addr & 0x9f) + 1] << 8) | bus->ppu->oam[addr & 0x9f];

    return (read(&bus->mem, addr + 1) << 8) | read(&bus->mem, addr);
}

void write_byte(bus_t* bus, uint16_t addr, uint8_t val) {
    if(addr >= 0x8000 && addr <= 0x9fff) {
        bus->ppu->vram[addr & 0x1fff] = val;
        return;
    } else if(addr >= 0xfe00 && addr <= 0xfe9f) {
        bus->ppu->oam[addr & 0x9f] = val;
        return;
    } else if(addr >= 0xff40 && addr <= 0xff4b) {
        write_io_ppu(&bus->ppu, &bus->mem, addr, val);
        return;
    }

    write(&bus->mem, addr, val);
}

void write_word(bus_t* bus, uint16_t addr, uint16_t val) {
    if(addr >= 0x8000 && addr <= 0x9fff) {
        bus->ppu->vram[(addr & 0x1fff) + 1] = val >> 8;
        bus->ppu->vram[addr & 0x1fff] = val & 0xff;
        return;
    } else if(addr >= 0xfe00 && addr <= 0xfe9f) {
        bus->ppu->oam[(addr & 0x9f) + 1] = val >> 8;
        bus->ppu->oam[addr & 0x9f] = val & 0xff;
        return;
    } else if(addr >= 0xff40 && addr <= 0xff4b) {
        write_io_ppu(&bus->ppu, &bus->mem, addr + 1, val >> 8);
        write_io_ppu(&bus->ppu, &bus->mem, addr, val & 0xff);
        return;
    }

    write(&bus->mem, addr, val);
}

void reset_bus(bus_t* bus) {
    reset_mem(&bus->mem);
}