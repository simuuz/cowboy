#include "mem.h"
#include <fstream>

Mem::Mem(std::string bootrom, std::string rom) {
    std::ifstream rom_file(rom, std::ios::binary);

    if(!rom_file.good()) {
        printf("Couldn't open %s\n", rom.c_str());
        exit(1);
    }

    rom_file.seekg(0, std::ios::end);
    auto size = rom_file.tellg();
    rom.resize(size);
    rom_file.seekg(0, std::ios::beg);

    rom_file.read((char*)rom.data(), size);

    std::ifstream bootrom_file(bootrom, std::ios::binary);
    
    if(!bootrom_file.good()) {
        printf("Couldn't open %s\n", rom.c_str());
        exit(1);
    }

    bootrom_file.read((char*)bootrom.data(), size);

    std::fill(vram.begin(), vram.end(), 0);
    std::fill(extram.begin(), extram.end(), 0);
    std::fill(eram.begin(), eram.end(), 0);
    std::fill(wram.begin(), wram.end(), 0);
    std::fill(hram.begin(), hram.end(), 0);
    std::fill(oam.begin(), oam.end(), 0);
}

template <typename T>
T Mem::read(u16 addr) {
    if(addr >= 0 && addr <= 0x7fff) {
        return (io.bootrom != 0) ? *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(bootrom))[addr])
                                 : *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(rom))[addr]);
    } else if (addr >= 0x8000 && addr <= 0x9fff) {
        return *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(vram))[addr & 0x1fff]);
    } else if (addr >= 0xa000 && addr <= 0xbfff) {
        return *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(extram))[addr & 0x1fff]);
    } else if (addr >= 0xc000 && addr <= 0xdfff) {
        return *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(wram))[addr & 0x1fff]);
    } else if (addr >= 0xe000 && addr <= 0xfdff) {
        return *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(eram))[addr & 0x1dff]);
    } else if (addr >= 0xfe00 && addr <= 0xfe9f) {
        return *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(oam))[addr & 0x9f]);
    } else if (addr >= 0xfea0 && addr <= 0xfeff) {
        return 0xff;
    } else if (addr >= 0xff00 && addr <= 0xff7f) {
        return io.read(addr);
    } else if (addr >= 0xff80 && addr <= 0xfffe) {
        return *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(hram))[addr & 0x7f]);
    }
}

template <typename T>
void Mem::write(u16 addr, T val) {
    if(addr >= 0 && addr <= 0x7fff) {
        printf("Tried to write to ROM space\n");
        exit(1);
    } else if (addr >= 0x8000 && addr <= 0x9fff) {
        *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(vram))[addr & 0x1fff]) = val;
    } else if (addr >= 0xa000 && addr <= 0xbfff) {
        *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(extram))[addr & 0x1fff]) = val;
    } else if (addr >= 0xc000 && addr <= 0xdfff) {
        *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(wram))[addr & 0x1fff]) = val;
    } else if (addr >= 0xe000 && addr <= 0xfdff) {
        *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(eram))[addr & 0x1dff]) = val;
    } else if (addr >= 0xfe00 && addr <= 0xfe9f) {
        *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(oam))[addr & 0x9f]) = val;
    } else if (addr >= 0xfea0 && addr <= 0xfeff) {
        return;
    } else if (addr >= 0xff00 && addr <= 0xff7f) {
        io.read(addr, val);
    } else if (addr >= 0xff80 && addr <= 0xfffe) {
        *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(hram))[addr & 0x7f]) = val;
    }
}