#include "mem.h"
#include <memory.h>

Mem::Mem(bool skip) : skip(skip) {
    rom_opened = false;
    if(skip) {
        io.bootrom = 1;
        io.tac = 0;
        io.tima = 0;
        io.tma = 0;
        io.intf = 0;
        io.div = 0;
    } else {
        io.bootrom = 0;
        io.tac = 0;
        io.tima = 0;
        io.tma = 0;
        io.intf = 0;
        io.div = 0;
    }

    rom.resize(ROM_SZ_MIN);
    std::fill(rom.begin(), rom.end(), 0);
    memset(bootrom, 0, BOOTROM_SZ);
    memset(extram, 0, EXTRAM_SZ);
    memset(eram, 0, ERAM_SZ);
    memset(wram, 0, WRAM_SZ);
    memset(hram, 0, HRAM_SZ);
}

void Mem::reset() {
    rom_opened = false;
    if(skip) {
        io.bootrom = 1;
        io.tac = 0;
        io.tima = 0;
        io.tma = 0;
        io.intf = 0;
        io.div = 0;
    } else {
        io.bootrom = 0;
        io.tac = 0;
        io.tima = 0;
        io.tma = 0;
        io.intf = 0;
        io.div = 0;
    }

    memset(extram, 0, EXTRAM_SZ);
    memset(eram, 0, ERAM_SZ);
    memset(wram, 0, WRAM_SZ);
    memset(hram, 0, HRAM_SZ);
}

void Mem::load_rom(std::string path) {
    std::ifstream file{path, std::ios::binary};
    file.unsetf(std::ios::skipws);
    
    if(!file.is_open()) {
        printf("Couldn't open %s\n", path.c_str());
        exit(1);
    }

    rom.insert(rom.begin(), std::istream_iterator<byte>(file), std::istream_iterator<byte>());
    file.close();
    
    rom_opened = true;

    MBC = rom[0x147];
    ROM_SIZE = rom[0x148];
    RAM_SIZE = rom[0x149];

    printf("\nMBC: %s\nRom size: %s\nRam size: %s\n\n", MBCs[MBC], ROMs[ROM_SIZE], RAMs[RAM_SIZE]);
}

bool Mem::load_bootrom(std::string path) {
    std::ifstream file{path, std::ios::binary};
    file.unsetf(std::ios::skipws);
    
    if(!file.is_open()) {
        printf("Couldn't open %s\n", path.c_str());
        return false;
    }

    file.read((char*)bootrom, BOOTROM_SZ);
    file.close();
    return true;
}

byte Mem::read(half addr) {
    switch(addr) {
        case 0 ... 0xff:
        if (io.bootrom == 0) {
            return bootrom[addr];
        } else {                        //TODO: make own MBC class and handle them like that
            if (mbc1.mode) {
                switch(ROM_SIZE) {
                    case ROM_32kb ... ROM_512kb:
                    zero_bank = 0;
                    break;
                    case ROM_1mb:
                    setbit<byte, 5>(zero_bank, mbc1.ram_bank & 1);
                    break;
                    case ROM_2mb:
                    setbit<byte, 5>(zero_bank, mbc1.ram_bank & 1);
                    setbit<byte, 6>(zero_bank, mbc1.ram_bank >> 1);
                    break;
                }
                return rom[0x4000 * zero_bank + addr];
            } else {
                return rom[addr];
            }
        }
        break;
        case 0x100 ... 0x3fff:
        if (mbc1.mode) {
            switch(ROM_SIZE) {
                case ROM_32kb ... ROM_512kb:
                zero_bank = 0;
                break;
                case ROM_1mb:
                setbit<byte, 5>(zero_bank, mbc1.ram_bank & 1);
                break;
                case ROM_2mb:
                setbit<byte, 5>(zero_bank, mbc1.ram_bank & 1);
                setbit<byte, 6>(zero_bank, mbc1.ram_bank >> 1);
                break;
            }
            return rom[0x4000 * zero_bank + addr];
        } else {
            return rom[addr];
        }
        break;
        case 0x4000 ... 0x7fff:
        switch(ROM_SIZE) {
            case ROM_32kb:
            high_bank = 1;
            break;
            case ROM_64kb ... ROM_512kb:
            high_bank = mbc1.rom_bank & MBC_BITMASK_LUT[ROM_SIZE];
            break;
            case ROM_1mb:
            high_bank = mbc1.rom_bank & MBC_BITMASK_LUT[ROM_SIZE];
            setbit<byte, 5>(high_bank, mbc1.ram_bank & 1);
            break;
            case ROM_2mb:
            high_bank = mbc1.rom_bank & MBC_BITMASK_LUT[ROM_SIZE];
            setbit<byte, 5>(high_bank, mbc1.ram_bank & 1);
            setbit<byte, 6>(high_bank, mbc1.ram_bank >> 1);
            break;
        }
        return rom[0x4000 * high_bank + (addr - 0x4000)];
        break;
        case 0xa000 ... 0xbfff:
        if (extram_enable) {
            if (RAM_SIZE == RAM_2kb || RAM_SIZE == RAM_8kb) {
                return extram[(addr - 0xa000) % RAM_SIZE];
            } else if (RAM_SIZE == RAM_32kb) {
                if (mbc1.mode)
                    return extram[0x2000 * mbc1.ram_bank + (addr - 0xa000)];
                else
                    return extram[addr - 0xa000];
            }
        } else {
            return 0xff;
        }
        case 0xc000 ... 0xdfff:
        return wram[addr & 0x1fff];
        case 0xe000 ... 0xfdff:
        return eram[addr & 0x1dff];
        case 0xfea0 ... 0xfeff:
        return 0xff;
        case 0xff00 ... 0xff7f:
        return read_io(addr);
        case 0xff80 ... 0xfffe:
        return hram[addr & 0x7f];
        case 0xffff:
        return ie;
    }
}

void Mem::write(half addr, byte val) {
    switch(addr) {
        case 0x0000 ... 0x1fff:
        extram_enable = (val & 0xF) == 0xA;
        break;
        case 0x2000 ... 0x3fff: {
            byte mask = val & MBC_BITMASK_LUT[ROM_SIZE];
            mbc1.rom_bank = (mask == 0) ? 1 : mask;
        }
        break;
        case 0x4000 ... 0x5fff:
        mbc1.ram_bank = val & 3;
        break;
        case 0x6000 ... 0x7fff:
        mbc1.mode = val & 1;
        break;
        case 0xa000 ... 0xbfff:
        if (extram_enable) {
            if (RAM_SIZE == RAM_2kb || RAM_SIZE == RAM_8kb) {
                extram[(addr - 0xa000) % RAM_SIZE] = val;
            } else if (RAM_SIZE == RAM_32kb) {
                if (mbc1.mode)
                    extram[0x2000 * mbc1.ram_bank + (addr - 0xa000)] = val;
                else
                    extram[addr - 0xa000] = val;
            }
        }
        break;
        case 0xc000 ... 0xdfff:
        wram[addr & 0x1fff] = val;
        break;
        case 0xe000 ... 0xfdff:
        eram[addr & 0x1dff] = val;
        break;
        case 0xfea0 ... 0xfeff:
        break;
        case 0xff00 ... 0xff7f:
        write_io(addr, val);
        break;
        case 0xff80 ... 0xfffe:
        hram[addr & 0x7f] = val;
        break;
        case 0xffff:
        ie = val;
        break;
    }
}

byte Mem::read_io(half addr) {
    switch(addr & 0xff) {
        case 0x00: return 0xff;
        case 0x04: return io.div;
        case 0x05: return io.tima;
        case 0x06: return io.tma;
        case 0x07: return io.tac;
        case 0x0f: return io.intf;
        case 0x10 ... 0x1e: return 0xff;
        case 0x20 ... 0x26: return 0xff;
        case 0x4d: return 0xff;
        case 0x50: return io.bootrom;
        default:
        printf("IO READ: Unsupported IO %02x\n", addr & 0xff);
    }
}

void Mem::write_io(half addr, byte val) {
    switch(addr & 0xff) {
        //case 0x00: handle_joypad(val); break;
        case 0x01: 
        printf("%c", val);
        fflush(stdout);
        break;
        case 0x02: break;
        case 0x04: io.div = 0; break;
        case 0x05: io.tima = val; break;
        case 0x06: io.tma = val; break;
        case 0x07: io.tac = val; break;
        case 0x0f: io.intf = val; break;
        case 0x10 ... 0x1e: break;
        case 0x20 ... 0x26: break; //STUB
        case 0x50: io.bootrom = val; break;
        default:
        printf("IO WRITE: Unsupported IO %02x\n", addr & 0xff);
    }
}