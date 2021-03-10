#include "bus.h"

Bus::Bus() { }

u8 Bus::readByte(u16 addr) {
    u16 dummy;
    return mem.read<u8>(addr, dummy, false);
}

u8 Bus::nextByte(u16 addr, u16& pc) {
    return mem.read<u8>(addr, pc);
}

u16 Bus::readWord(u16 addr) {
    u16 dummy;
    return mem.read<u16>(addr, dummy, false);
}

u16 Bus::nextWord(u16 addr, u16& pc) {
    return mem.read<u16>(addr, pc);
}

void Bus::writeByte(u16 addr, u8 val) {
    mem.write<u8>(addr, val);
}

void Bus::writeWord(u16 addr, u16 val) {
    mem.write<u16>(addr, val);
}

void Bus::loadROM(std::string filename) {
    mem.loadROM(filename);
    rom_opened = true;
}

void Bus::loadBootROM(std::string filename) {
    mem.loadBootROM(filename);
}

void Bus::reset() {
    mem.reset();
    rom_opened = false;
}