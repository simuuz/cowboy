#include "mem.h"
#include <fstream>
#include <memory.h>

Mem::Mem(std::string bootrom_filename, std::string rom_filename) {
    std::ifstream rom_file(rom_filename, std::ios::binary);

    if(!rom_file.good()) {
        printf("Couldn't open %s\n", rom_filename.c_str());
        exit(1);
    }

    rom_file.seekg(0, std::ios::end);
    auto size = rom_file.tellg();
    rom.resize(size);
    rom_file.seekg(0, std::ios::beg);

    rom_file.read((char*)rom.data(), size);

    std::ifstream bootrom_file(bootrom_filename, std::ios::binary);
    
    if(!bootrom_file.good()) {
        printf("Couldn't open %s\n", bootrom_filename.c_str());
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
T Mem::Read(void* buffer, u16 addr) {
    return *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(buffer))[addr]);
}

template <typename T>
T Mem::read(u16 addr, u16& pc, bool inc) {
    pc += (inc) ? sizeof(T) : 0;
    if(addr >= 0 && addr <= 0x7fff) {
        return (io.bootrom != 0) ? Read<T>(bootrom.data(), addr)
                                 : Read<T>(rom.data(), addr);
    } else if (addr >= 0x8000 && addr <= 0x9fff) {
        return Read<T>(vram.data(), addr & 0x1fff);
    } else if (addr >= 0xa000 && addr <= 0xbfff) {
        return Read<T>(extram.data(), addr & 0x1fff);
    } else if (addr >= 0xc000 && addr <= 0xdfff) {
        return Read<T>(wram.data(), addr & 0x1fff);
    } else if (addr >= 0xe000 && addr <= 0xfdff) {
        return Read<T>(eram.data(), addr & 0x1dff);
    } else if (addr >= 0xfe00 && addr <= 0xfe9f) {
        return Read<T>(oam.data(), addr & 0x9f);
    } else if (addr >= 0xfea0 && addr <= 0xfeff) {
        return 0xff;
    } else if (addr >= 0xff00 && addr <= 0xff7f) {
        return io.read(addr);
    } else if (addr >= 0xff80 && addr <= 0xfffe) {
        return Read<T>(hram.data(), addr & 0x7f);
    }
}

template u8 Mem::read<u8>(u16, u16&, bool);
template u16 Mem::read<u16>(u16, u16&, bool);
template u32 Mem::read<u32>(u16, u16&, bool);

template <typename T>
void Mem::Write(void* buffer, u16 addr, T val) {
    *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(buffer))[addr]) = val;
}

template <typename T>
void Mem::write(u16 addr, T val) {
    if(addr >= 0 && addr <= 0x7fff) {
        printf("Tried to write to ROM space\n");
        exit(1);
    } else if (addr >= 0x8000 && addr <= 0x9fff) {
        Write<T>(vram.data(), addr & 0x1fff, val);
    } else if (addr >= 0xa000 && addr <= 0xbfff) {
        Write<T>(extram.data(), addr & 0x1fff, val);
    } else if (addr >= 0xc000 && addr <= 0xdfff) {
        Write<T>(wram.data(), addr & 0x1fff, val);
    } else if (addr >= 0xe000 && addr <= 0xfdff) {
        Write<T>(eram.data(), addr & 0x1dff, val);
    } else if (addr >= 0xfe00 && addr <= 0xfe9f) {
        Write<T>(oam.data(), addr & 0x9f, val);
    } else if (addr >= 0xfea0 && addr <= 0xfeff) {
        return;
    } else if (addr >= 0xff00 && addr <= 0xff7f) {
        io.write(addr, val);
    } else if (addr >= 0xff80 && addr <= 0xfffe) {
        Write<T>(hram.data(), addr & 0x7f, val);
    }
}

template void Mem::write<u8>(u16, u8);
template void Mem::write<u16>(u16, u16);
template void Mem::write<u32>(u16, u32);