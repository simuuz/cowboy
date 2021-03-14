#pragma once
#include "mem.h"

class Bus {
public:
    Bus(bool skip);
    u8 readByte(u16 addr);
    u8 nextByte(u16 addr, u16& pc);
    void writeByte(u16 addr, u8 val);
    u16 readWord(u16 addr);
    u16 nextWord(u16 addr, u16& pc);
    void writeWord(u16 addr, u16 val);
    void loadROM(std::string filename);
    void loadBootROM(std::string filename);
    void reset();
    bool rom_opened = false;
    Mem mem;
};