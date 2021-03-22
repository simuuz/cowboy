#pragma once
#include "ppu.h"

class Bus {
public:
    Bus(bool skip);
    u8 read_byte(u16 addr);
    u8 next_byte(u16 addr, u16& pc);
    void write_byte(u16 addr, u8 val);
    u16 read_word(u16 addr);
    u16 next_word(u16 addr, u16& pc);
    void write_word(u16 addr, u16 val);
    void load_rom(std::string filename);
    void load_bootrom(std::string filename);
    void reset();
    bool rom_opened = false;
    Ppu ppu;
    Mem mem;
};