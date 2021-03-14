#include "mem.h"
#include <fstream>
#include <memory.h>

Mem::Mem(bool skip) : skip(skip) {
    if(skip) {
        io.bootrom = 1; io.bgp = 0xfc; io.scy = 0;
        io.scx = 0; io.lcdc = 0x91; io.tac = 0; io.tima = 0;
        io.tma = 0; io.intf = 0; io.div = 0;
    } else {
        io.bootrom = 0; io.bgp = 0; io.scy = 0;
        io.scx = 0; io.lcdc = 0; io.tac = 0; io.tima = 0;
        io.tma = 0; io.intf = 0; io.div = 0;
    }
    std::fill(vram.begin(), vram.end(), 0);
    std::fill(extram.begin(), extram.end(), 0);
    std::fill(eram.begin(), eram.end(), 0);
    std::fill(wram.begin(), wram.end(), 0);
    std::fill(hram.begin(), hram.end(), 0);
    std::fill(oam.begin(), oam.end(), 0);
}

void Mem::reset() {
    if(skip) {
        io.bootrom = 1; io.bgp = 0xfc; io.scy = 0;
        io.scx = 0; io.lcdc = 0x91; io.tac = 0; io.tima = 0;
        io.tma = 0; io.intf = 0; io.div = 0;
    } else {
        io.bootrom = 0; io.bgp = 0; io.scy = 0;
        io.scx = 0; io.lcdc = 0; io.tac = 0; io.tima = 0;
        io.tma = 0; io.intf = 0; io.div = 0;
    }
    std::fill(rom.begin(), rom.end(), 0);
    std::fill(vram.begin(), vram.end(), 0);
    std::fill(extram.begin(), extram.end(), 0);
    std::fill(eram.begin(), eram.end(), 0);
    std::fill(wram.begin(), wram.end(), 0);
    std::fill(hram.begin(), hram.end(), 0);
    std::fill(oam.begin(), oam.end(), 0);
}

void Mem::loadROM(std::string filename) {
    std::ifstream file(filename, std::ios::binary);
    
    if(!file.good()) {
        printf("Couldn't open %s\n", filename.c_str());
        exit(1);
    }

    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    rom.resize(size);

    file.read((char*)rom.data(), size);
    file.close();

    MBC = rom[0x147];
    ROM_SIZE = rom[0x148];
    RAM_SIZE = rom[0x149];
    printf("MBC: %s\nRom size: %s\nRam size: %s\n\n", MBCs[MBC].c_str(), ROMs[ROM_SIZE].c_str(), RAMs[RAM_SIZE].c_str());
}

void Mem::loadBootROM(std::string filename) {
    std::ifstream file(filename, std::ios::binary);
    
    if(!file.good()) {
        printf("Couldn't open %s\n", filename.c_str());
        exit(1);
    }

    file.read((char*)bootrom.data(), 256);
    file.close();
}

template <typename T>
T Mem::Read(void* buffer, u16 addr) {
    return *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(buffer))[addr]);
}

template <typename T>
T Mem::read(u16 addr, u16& pc, bool inc) {
    pc += (inc) ? sizeof(T) : 0;
    if(addr <= 0xff) {
        if(io.bootrom == 0) {
            return Read<T>(bootrom.data(), addr);
        } else {
            if(mode) {
                u8 zero_bank = 0;
                if (ROM_SIZE == ROM_1mb) {
                    zero_bank |= (ram_bank & 1) << 5;
                } else if (ROM_SIZE == ROM_2mb) {
                    zero_bank |= (ram_bank & 3) << 5;
                }
                return Read<T>(rom.data(), 0x4000 * zero_bank + addr);
            } else {
                return Read<T>(rom.data(), addr);
            }
        }
    } else if (addr >= 0x100 && addr <= 0x3fff) {
        if(mode) {
            u8 zero_bank = 0;
            if (ROM_SIZE == ROM_1mb) {
                zero_bank |= (ram_bank & 1) << 5;
            } else if (ROM_SIZE == ROM_2mb) {
                zero_bank |= (ram_bank & 3) << 5;
            }
            return Read<T>(rom.data(), 0x4000 * zero_bank + addr);
        } else {
            return Read<T>(rom.data(), addr);
        }
    } else if (addr >= 0x4000 && addr <= 0x7fff) {
        u8 high_bank;
        if(ROM_SIZE < ROM_1mb) {
            high_bank = rom_bank & MBC1_BITMASK_LUT[ROM_SIZE];
        } else if (ROM_SIZE == ROM_1mb) {
            high_bank = rom_bank & MBC1_BITMASK_LUT[ROM_SIZE];
            high_bank ^= (high_bank & 0b100000);
            high_bank |= (ram_bank & 1) << 5;
        } else if (ROM_SIZE == ROM_2mb) {
            high_bank = rom_bank & MBC1_BITMASK_LUT[ROM_SIZE];
            high_bank ^= (high_bank & 0b1100000);
            high_bank |= (ram_bank & 3) << 5;
        }
        return Read<T>(rom.data(), 0x4000 * high_bank + (addr - 0x4000));
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
        return (T)0xff;
    } else if (addr >= 0xff00 && addr <= 0xff7f) {
        return io.read(addr);
    } else if (addr >= 0xff80 && addr <= 0xfffe) {
        return Read<T>(hram.data(), addr & 0x7f);
    } else {
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
    if(addr <= 0x1fff) {
        extram_enable = ((val & 0xF) == 0xA);
    } else if(addr >= 0x2000 && addr <= 0x3fff) {
        if(ROM_SIZE == ROM_32kb)
            rom_bank = 0;
        u8 mask = val & MBC1_BITMASK_LUT[ROM_SIZE];
        rom_bank = (mask == 0) ? 1 : mask;
    } else if (addr >= 0x4000 && addr <= 0x5fff) {
        ram_bank = val & 3;
    } else if (addr >= 0x6000 && addr <= 0x7fff) {
        mode = (val & 1);
    } else if (addr >= 0x8000 && addr <= 0x9fff) {
        Write<T>(vram.data(), addr & 0x1fff, val);
    } else if (addr >= 0xa000 && addr <= 0xbfff) {
        if(extram_enable) {
            if(RAM_SIZE == RAM_2kb || RAM_SIZE == RAM_8kb) {
                Write<T>(extram.data(), (addr - 0xa000) % RAM_SIZE, val);
            } else if(RAM_SIZE == RAM_32kb) {
                if(mode)
                    Write<T>(extram.data(), 0x2000 * ram_bank + (addr - 0xa000), val);
                else
                    Write<T>(extram.data(), addr - 0xa000, val);
            }
        }
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
    } else {
        ie = val;
    }
}

template void Mem::write<u8>(u16, u8);
template void Mem::write<i8>(u16, i8);
template void Mem::write<u16>(u16, u16);
template void Mem::write<u32>(u16, u32);

u8 Mem::IO::read(u16 addr) {
    switch(addr & 0xff) {
        case 0x00: return 0xff;
        case 0x04: return div;
        case 0x05: return tima;
        case 0x06: return tma;
        case 0x07: return tac;
        case 0x0f: return intf;
        case 0x47: return bgp;
        case 0x50: return bootrom;
        case 0x10 ... 0x1e: return 0xff;
        case 0x20 ... 0x26: return 0xff;
        case 0x44: return 0x90;
        case 0x42: return scy;
        case 0x43: return scx;
        case 0x40: return lcdc;
        case 0x4d: return 0xff;
        default:
        printf("IO READ: Unsupported IO %02x\n", addr & 0xff);
        exit(1);
    }
}

void Mem::IO::write(u16 addr, u8 val) {
    switch(addr & 0xff) {
        //case 0x00: handleJoypad(val); break;
        case 0x01: printf("%c", val); break;
        case 0x02: break;
        case 0x04: div = 0; break;
        case 0x06: tma = val; break;
        case 0x05: tima = val; break;
        case 0x07: tac = val; break;
        case 0x0f: intf = val; break;
        case 0x47: bgp = val; break;
        case 0x50: bootrom = val; break;
        case 0x10 ... 0x1e: break;
        case 0x20 ... 0x26: break; //STUB
        case 0x42: scy = val; break;
        case 0x43: scx = val; break;
        case 0x40: lcdc = val; break;
        default:
        printf("IO WRITE: Unsupported IO %02x\n", addr & 0xff);
        exit(1);
    }
}