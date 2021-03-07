#include "mem.h"
#include <fstream>
#include <memory.h>

Mem::Mem() {
    std::fill(vram.begin(), vram.end(), 0);
    std::fill(extram.begin(), extram.end(), 0);
    std::fill(eram.begin(), eram.end(), 0);
    std::fill(wram.begin(), wram.end(), 0);
    std::fill(hram.begin(), hram.end(), 0);
    std::fill(oam.begin(), oam.end(), 0);
}

void Mem::reset() {
    std::fill(rom.begin(), rom.end(), 0);
    std::fill(vram.begin(), vram.end(), 0);
    std::fill(extram.begin(), extram.end(), 0);
    std::fill(eram.begin(), eram.end(), 0);
    std::fill(wram.begin(), wram.end(), 0);
    std::fill(hram.begin(), hram.end(), 0);
    std::fill(oam.begin(), oam.end(), 0);
    rom_opened = false;
}

void Mem::loadROM(std::string filename) {
    std::ifstream rom_file(filename, std::ios::binary);

    if(!rom_file.good()) {
        printf("Couldn't open %s\n", filename.c_str());
        exit(1);
    }

    rom_file.seekg(0, std::ios::end);
    auto size = rom_file.tellg();
    rom.resize(size);
    rom_file.seekg(0, std::ios::beg);

    rom_file.read((char*)rom.data(), size);
    rom_opened = true;
}

void Mem::loadBootROM(std::string filename) {
    std::ifstream bootrom_file(filename, std::ios::binary);
    
    if(!bootrom_file.good()) {
        printf("Couldn't open %s\n", filename.c_str());
        exit(1);
    }

    bootrom_file.read((char*)bootrom.data(), 256);
}

template <typename T>
T Mem::Read(void* buffer, u16 addr) {
    return *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(buffer))[addr]);
}

template <typename T>
T Mem::read(u16 addr, u16& pc, bool inc) {
    pc += (inc) ? sizeof(T) : 0;
    if(addr <= 0xff) {
        return (io.bootrom == 0) ? Read<T>(bootrom.data(), addr)
                                 : Read<T>(rom.data(), addr);
    } else if (addr >= 0x100 && addr <= 0x7fff) {
        return Read<T>(rom.data(), addr);
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
        if(sizeof(T) == 1)
            return io.read(addr);
    } else if (addr >= 0xff80 && addr <= 0xfffe) {
        return Read<T>(hram.data(), addr & 0x7f);
    } else {
        if(sizeof(T) == 1)
            return ie;
    }
}

template u8 Mem::read<u8>(u16, u16&, bool);
template i8 Mem::read<i8>(u16, u16&, bool);
template u16 Mem::read<u16>(u16, u16&, bool);
template u32 Mem::read<u32>(u16, u16&, bool);

template <typename T>
void Mem::Write(void* buffer, u16 addr, T val) {
    *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(buffer))[addr]) = val;
}

template <typename T>
void Mem::write(u16 addr, T val) {
    if(addr <= 0x7fff) {
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
        if(sizeof(T) == 1)
            io.write(addr, val);
    } else if (addr >= 0xff80 && addr <= 0xfffe) {
        Write<T>(hram.data(), addr & 0x7f, val);
    } else {
        if(sizeof(T) == 1)
            ie = val;
    }
}

template void Mem::write<u8>(u16, u8);
template void Mem::write<i8>(u16, i8);
template void Mem::write<u16>(u16, u16);
template void Mem::write<u32>(u16, u32);

u8 Mem::IO::read(u16 addr) {
    switch(addr & 0xff) {
        case 0x07: return tac;
        case 0x0f: return intf;
        case 0x47: return bgp;
        case 0x50: return bootrom;
        case 0x10 ... 0x1e: case 0x20 ... 0x26:
        return 0xff; //STUB
        case 0x44: return 0x90;
        case 0x42: return scy;
        case 0x43: return scx;
        case 0x40: return lcdc;
        default:
        printf("IO READ: Unsupported IO %02x\n", addr & 0xff);
        exit(1);
    }
}

void Mem::IO::write(u16 addr, u8 val) {
    switch(addr & 0xff) {
        case 0x01: printf("%c", val); break;
        case 0x02: break;
        case 0x07: tac = val; break;
        case 0x0f: intf = val; break;
        case 0x47: bgp = val; break;
        case 0x50: bootrom = val; break;
        case 0x10 ... 0x1e: case 0x20 ... 0x26:
        break; //STUB
        case 0x42: scy = val; break;
        case 0x43: scx = val; break;
        case 0x40: lcdc = val; break;
        default:
        printf("IO WRITE: Unsupported IO %02x\n", addr & 0xff);
        exit(1);
    }
}