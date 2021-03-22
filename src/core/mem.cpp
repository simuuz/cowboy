#include "mem.h"
#include <fstream>
#include <memory.h>

Mem::Mem(bool skip) : skip(skip) {
    if(skip) {
        io.bootrom = 1; io.tac = 0; io.tima = 0;
        io.tma = 0; io.intf = 0; io.div = 0;
    } else {
        io.bootrom = 0; io.tac = 0; io.tima = 0;
        io.tma = 0; io.intf = 0; io.div = 0;
    }

    std::fill(extram.begin(), extram.end(), 0);
    std::fill(eram.begin(), eram.end(), 0);
    std::fill(wram.begin(), wram.end(), 0);
    std::fill(hram.begin(), hram.end(), 0);
}

void Mem::reset() {
    if(skip) {
        io.bootrom = 1; io.tac = 0; io.tima = 0;
        io.tma = 0; io.intf = 0; io.div = 0;
    } else {
        io.bootrom = 0; io.tac = 0; io.tima = 0;
        io.tma = 0; io.intf = 0; io.div = 0;
    }

    std::fill(rom.begin(), rom.end(), 0);
    std::fill(extram.begin(), extram.end(), 0);
    std::fill(eram.begin(), eram.end(), 0);
    std::fill(wram.begin(), wram.end(), 0);
    std::fill(hram.begin(), hram.end(), 0);
}

void Mem::load_rom(std::string filename) {
    std::ifstream file(filename, std::ios::binary);
    file.unsetf(std::ios::skipws);
    
    if(!file.good()) {
        printf("Couldn't open %s\n", filename.c_str());
        exit(1);
    }

    rom.insert(rom.begin(), std::istream_iterator<u8>(file), std::istream_iterator<u8>());
    file.close();

    MBC = rom[0x147];
    ROM_SIZE = rom[0x148];
    RAM_SIZE = rom[0x149];

    printf("\nMBC: %s\nRom size: %s\nRam size: %s\n\n", MBCs[MBC].c_str(), ROMs[ROM_SIZE].c_str(), RAMs[RAM_SIZE].c_str());
}

void Mem::load_bootrom(std::string filename) {
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
T Mem::read(u16 addr) {
    switch(addr) {
        case 0 ... 0xff:
        if (io.bootrom == 0) {
            return Read<T>(bootrom.data(), addr);
        } else {
            if (MBC == MBC1 || MBC == MBC1_BATTERY || MBC == MBC1_RAM) {
                if (mbc1_t.mode) {
                    if (ROM_SIZE < ROM_1mb) {
                        zero_bank = 0;
                    } else if (ROM_SIZE == ROM_1mb) {
                        setbit(zero_bank, 5, mbc1_t.ram_bank & 1);
                    } else if (ROM_SIZE == ROM_2mb) {
                        setbit(zero_bank, 5, mbc1_t.ram_bank & 1);
                        setbit(zero_bank, 6, mbc1_t.ram_bank >> 1);
                    }
                    return Read<T>(rom.data(), 0x4000 * zero_bank + addr);
                } else {
                    return Read<T>(rom.data(), addr);
                }
            } else if (MBC == MBC5 || MBC == MBC5_BATTERY || MBC == MBC5_RAM ||
                       MBC == MBC5_RUMBLE || MBC == MBC5_RUMBLE_BATTERY ||
                       MBC == MBC5_RUMBLE_RAM) {
                return Read<T>(rom.data(), addr);
            }
        }
        break;
        case 0x100 ... 0x3fff:
        if (MBC == MBC1 || MBC == MBC1_BATTERY || MBC == MBC1_RAM) {
            if (mbc1_t.mode) {
                if (ROM_SIZE < ROM_1mb) {
                    zero_bank = 0;
                } else if (ROM_SIZE == ROM_1mb) {
                    setbit(zero_bank, 5, mbc1_t.ram_bank & 1);
                } else if (ROM_SIZE == ROM_2mb) {
                    setbit(zero_bank, 5, mbc1_t.ram_bank & 1);
                    setbit(zero_bank, 6, mbc1_t.ram_bank >> 1);
                }
                return Read<T>(rom.data(), 0x4000 * zero_bank + addr);
            } else {
                return Read<T>(rom.data(), addr);
            }
        } else if (MBC == MBC5 || MBC == MBC5_BATTERY || MBC == MBC5_RAM ||
                   MBC == MBC5_RUMBLE || MBC == MBC5_RUMBLE_BATTERY ||
                   MBC == MBC5_RUMBLE_RAM) {
            return Read<T>(rom.data(), addr);
        }
        break;
        case 0x4000 ... 0x7fff:
        if (MBC == MBC1 || MBC == MBC1_BATTERY || MBC == MBC1_RAM) {
            if (ROM_SIZE < ROM_1mb) {
                high_bank = mbc1_t.rom_bank & MBC_BITMASK_LUT[ROM_SIZE];
            } else if (ROM_SIZE == ROM_1mb) {
                high_bank = mbc1_t.rom_bank & MBC_BITMASK_LUT[ROM_SIZE];
                setbit<u8>(high_bank, 5, mbc1_t.ram_bank & 1);
            } else if (ROM_SIZE == ROM_2mb) {
                high_bank = mbc1_t.rom_bank & MBC_BITMASK_LUT[ROM_SIZE];
                setbit<u8>(high_bank, 5, mbc1_t.ram_bank & 1);
                setbit<u8>(high_bank, 6, (mbc1_t.ram_bank >> 1) & 1);
            }
            return Read<T>(rom.data(), 0x4000 * high_bank + (addr - 0x4000));
        } else if (MBC == MBC5 || MBC == MBC5_BATTERY || MBC == MBC5_RAM ||
                   MBC == MBC5_RUMBLE || MBC == MBC5_RUMBLE_BATTERY ||
                   MBC == MBC5_RUMBLE_RAM) {
            return Read<T>(rom.data(), 0x4000 * mbc5_t.rom_bank + (addr - 0x4000));
        }
        break;
        case 0xa000 ... 0xbfff:
        if (MBC == MBC1 || MBC == MBC1_BATTERY || MBC == MBC1_RAM) {
            return (extram_enable) ? Read<T>(extram.data(), addr & 0x1fff) : 0xff;
        } else if (MBC == MBC5 || MBC == MBC5_BATTERY || MBC == MBC5_RAM ||
                   MBC == MBC5_RUMBLE || MBC == MBC5_RUMBLE_BATTERY ||
                   MBC == MBC5_RUMBLE_RAM) {
            return (extram_enable) ? Read<T>(extram.data(), 0x2000 * mbc5_t.ram_bank + (addr - 0xa000)) : 0xff;
        }
        break;
        case 0xc000 ... 0xdfff:
        return Read<T>(wram.data(), addr & 0x1fff);
        case 0xe000 ... 0xfdff:
        return Read<T>(eram.data(), addr & 0x1dff);
        case 0xfea0 ... 0xfeff:
        return (T)0xff;
        case 0xff00 ... 0xff7f:
        return io.read(addr);
        case 0xff80 ... 0xfffe:
        return Read<T>(hram.data(), addr & 0x7f);
        case 0xffff:
        return ie;
    }
}

template u8 Mem::read<u8>(u16);
template i8 Mem::read<i8>(u16);
template u16 Mem::read<u16>(u16);
template u32 Mem::read<u32>(u16);

template <typename T>
void Mem::Write(void* buffer, u16 addr, T val) {
    *reinterpret_cast<T*>(&(reinterpret_cast<u8*>(buffer))[addr]) = val;
}

template <typename T>
void Mem::write(u16 addr, T val) {
    switch(addr) {
        case 0x0000 ... 0x1fff:
        extram_enable = (val & 0xF) == 0xA;
        break;
        case 0x2000 ... 0x3fff: {
            u8 mask = val & MBC_BITMASK_LUT[ROM_SIZE];
            if (MBC == MBC1 || MBC == MBC1_BATTERY || MBC == MBC1_RAM) {
                mbc1_t.rom_bank = (mask == 0) ? 1 : mask & 0x1f;
            } else if (MBC == MBC5 || MBC == MBC5_BATTERY || MBC == MBC5_RAM ||
                       MBC == MBC5_RUMBLE || MBC == MBC5_RUMBLE_BATTERY ||
                       MBC == MBC5_RUMBLE_RAM) {
                if (addr <= 0x2fff) {
                    mbc5_t.rom_bank = val;
                } else if (addr >= 0x3000 && addr <= 0x3fff) {
                    u8 temp = mbc5_t.rom_bank;
                    setbit(temp, 8, val & 1);
                    mbc5_t.rom_bank = temp;
                }
            }
        }
        break;
        case 0x4000 ... 0x5fff:
        if (MBC == MBC1 || MBC == MBC1_BATTERY || MBC == MBC1_RAM) {
            mbc1_t.ram_bank = val & 3;
        } else if (MBC == MBC5 || MBC == MBC5_BATTERY || MBC == MBC5_RAM ||
                   MBC == MBC5_RUMBLE || MBC == MBC5_RUMBLE_BATTERY ||
                   MBC == MBC5_RUMBLE_RAM) {
            mbc5_t.ram_bank = val & 0xf;
        }
        break;
        case 0x6000 ... 0x7fff:
        mbc1_t.mode = val & 1;
        break;
        case 0xa000 ... 0xbfff:
        if (extram_enable) {
            if (MBC == MBC1 || MBC == MBC1_BATTERY || MBC == MBC1_RAM) {
                if (RAM_SIZE == RAM_2kb || RAM_SIZE == RAM_8kb) {
                    Write<T>(extram.data(), (addr - 0xa000) % RAM_SIZE, val);
                } else if (RAM_SIZE == RAM_32kb) {
                    if (mbc1_t.mode)
                        Write<T>(extram.data(), 0x2000 * mbc1_t.ram_bank + (addr - 0xa000), val);
                    else
                        Write<T>(extram.data(), addr - 0xa000, val);
                }
            } else if (MBC == MBC5 || MBC == MBC5_BATTERY || MBC == MBC5_RAM ||
                       MBC == MBC5_RUMBLE || MBC == MBC5_RUMBLE_BATTERY ||
                       MBC == MBC5_RUMBLE_RAM) {
                Write<T>(extram.data(), 0x2000 * mbc5_t.ram_bank + (addr - 0xa000), val);
            }
        }
        break;
        case 0xc000 ... 0xdfff:
        Write<T>(wram.data(), addr & 0x1fff, val);
        break;
        case 0xe000 ... 0xfdff:
        Write<T>(eram.data(), addr & 0x1dff, val);
        break;
        case 0xfea0 ... 0xfeff:
        break;
        case 0xff00 ... 0xff7f:
        io.write(addr, val);
        break;
        case 0xff80 ... 0xfffe:
        Write<T>(hram.data(), addr & 0x7f, val);
        break;
        case 0xffff:
        ie = val;
        break;
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
        case 0x10 ... 0x1e: return 0xff;
        case 0x20 ... 0x26: return 0xff;
        case 0x4d: return 0xff;
        case 0x50: return bootrom;
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
        case 0x05: tima = val; break;
        case 0x06: tma = val; break;
        case 0x07: tac = val; break;
        case 0x0f: intf = val; break;
        case 0x10 ... 0x1e: break;
        case 0x20 ... 0x26: break; //STUB
        case 0x50: bootrom = val; break;
        default:
        printf("IO WRITE: Unsupported IO %02x\n", addr & 0xff);
        exit(1);
    }
}