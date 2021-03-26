#include "mem.h"
#include <memory.h>

void init_mem(mem_t* mem, bool skip) {
    mem->skip = skip;
    mem->rom_opened = false;
    if(skip) {
        mem->io.bootrom = 1;
        mem->io.tac = 0;
        mem->io.tima = 0;
        mem->io.tma = 0;
        mem->io.intf = 0;
        mem->io.div = 0;
    } else {
        mem->io.bootrom = 0;
        mem->io.tac = 0;
        mem->io.tima = 0;
        mem->io.tma = 0;
        mem->io.intf = 0;
        mem->io.div = 0;
    }

    mem->rom = NULL;

    memset(mem->bootrom, 0, BOOTROM_SZ);
    memset(mem->extram, 0, EXTRAM_SZ);
    memset(mem->eram, 0, ERAM_SZ);
    memset(mem->wram, 0, WRAM_SZ);
    memset(mem->hram, 0, HRAM_SZ);
}

void reset_mem(mem_t* mem) {
    mem->rom_opened = false;
    if(mem->skip) {
        mem->io.bootrom = 1;
        mem->io.tac = 0;
        mem->io.tima = 0;
        mem->io.tma = 0;
        mem->io.intf = 0;
        mem->io.div = 0;
    } else {
        mem->io.bootrom = 0;
        mem->io.tac = 0;
        mem->io.tima = 0;
        mem->io.tma = 0;
        mem->io.intf = 0;
        mem->io.div = 0;
    }

    memset(mem->extram, 0, EXTRAM_SZ);
    memset(mem->eram, 0, ERAM_SZ);
    memset(mem->wram, 0, WRAM_SZ);
    memset(mem->hram, 0, HRAM_SZ);
}

void load_rom(mem_t* mem, char* filename) {
    FILE* file = fopen(filename, "rb");

    if(mem->rom != NULL) {
        mem->rom = NULL;
    }
    
    if(file == NULL) {
        printf("Couldn't open %s\n", filename);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    mem->rom = (uint8_t*)malloc(size * sizeof(uint8_t));
    fread(mem->rom, size, 1, file);
    fclose(file);

    mem->MBC = mem->rom[0x147];
    mem->ROM_SIZE = mem->rom[0x148];
    mem->RAM_SIZE = mem->rom[0x149];
    mem->rom_opened = true;

    printf("\nMBC: %s\nRom size: %s\nRam size: %s\n\n", MBCs[mem->MBC], ROMs[mem->ROM_SIZE], RAMs[mem->RAM_SIZE]);
}

bool load_bootrom(mem_t* mem, char* filename) {
    FILE* file = fopen(filename, "rb");
    
    if(file == NULL) {
        printf("Couldn't open %s\n", filename);
        return false;
    }

    fread(mem->bootrom, BOOTROM_SZ, 1, file);
    fclose(file);
    return true;
}

uint8_t read(mem_t* mem, uint16_t addr) {
    switch(addr) {
        case 0 ... 0xff:
        if (mem->io.bootrom == 0) {
            return mem->bootrom[addr];
        } else {
            if (mem->MBC == MBC1 || mem->MBC == MBC1_BATTERY || mem->MBC == MBC1_RAM) {
                if (mem->mbc1.mode) {
                    if (mem->ROM_SIZE < ROM_1mb) {
                        mem->zero_bank = 0;
                    } else if (mem->ROM_SIZE == ROM_1mb) {
                        setbit8(&mem->zero_bank, 5, mem->mbc1.ram_bank & 1);
                    } else if (mem->ROM_SIZE == ROM_2mb) {
                        setbit8(&mem->zero_bank, 5, mem->mbc1.ram_bank & 1);
                        setbit8(&mem->zero_bank, 6, mem->mbc1.ram_bank >> 1);
                    }
                    return mem->rom[0x4000 * mem->zero_bank + addr];
                } else {
                    return mem->rom[addr];
                }
            } else if (mem->MBC == MBC5 || mem->MBC == MBC5_BATTERY || mem->MBC == MBC5_RAM ||
                       mem->MBC == MBC5_RUMBLE || mem->MBC == MBC5_RUMBLE_BATTERY ||
                       mem->MBC == MBC5_RUMBLE_RAM) {
                return mem->rom[addr];
            }
        }
        break;
        case 0x100 ... 0x3fff:
        if (mem->MBC == MBC1 || mem->MBC == MBC1_BATTERY || mem->MBC == MBC1_RAM) {
            if (mem->mbc1.mode) {
                if (mem->ROM_SIZE < ROM_1mb) {
                    mem->zero_bank = 0;
                } else if (mem->ROM_SIZE == ROM_1mb) {
                    setbit8(&mem->zero_bank, 5, mem->mbc1.ram_bank & 1);
                } else if (mem->ROM_SIZE == ROM_2mb) {
                    setbit8(&mem->zero_bank, 5, mem->mbc1.ram_bank & 1);
                    setbit8(&mem->zero_bank, 6, mem->mbc1.ram_bank >> 1);
                }
                return mem->rom[0x4000 * mem->zero_bank + addr];
            } else {
                return mem->rom[addr];
            }
        } else if (mem->MBC == MBC5 || mem->MBC == MBC5_BATTERY || mem->MBC == MBC5_RAM ||
                   mem->MBC == MBC5_RUMBLE || mem->MBC == MBC5_RUMBLE_BATTERY ||
                   mem->MBC == MBC5_RUMBLE_RAM) {
            return mem->rom[addr];
        }
        break;
        case 0x4000 ... 0x7fff:
        if (mem->MBC == MBC1 || mem->MBC == MBC1_BATTERY || mem->MBC == MBC1_RAM) {
            if (mem->ROM_SIZE < ROM_1mb) {
                mem->high_bank = mem->mbc1.rom_bank & MBC_BITMASK_LUT[mem->ROM_SIZE];
            } else if (mem->ROM_SIZE == ROM_1mb) {
                mem->high_bank = mem->mbc1.rom_bank & MBC_BITMASK_LUT[mem->ROM_SIZE];
                setbit8(&mem->high_bank, 5, mem->mbc1.ram_bank & 1);
            } else if (mem->ROM_SIZE == ROM_2mb) {
                mem->high_bank = mem->mbc1.rom_bank & MBC_BITMASK_LUT[mem->ROM_SIZE];
                setbit8(&mem->high_bank, 5, mem->mbc1.ram_bank & 1);
                setbit8(&mem->high_bank, 6, (mem->mbc1.ram_bank >> 1) & 1);
            }
            return mem->rom[0x4000 * mem->high_bank + (addr - 0x4000)];
        } else if (mem->MBC == MBC5 || mem->MBC == MBC5_BATTERY || mem->MBC == MBC5_RAM ||
                   mem->MBC == MBC5_RUMBLE || mem->MBC == MBC5_RUMBLE_BATTERY ||
                   mem->MBC == MBC5_RUMBLE_RAM) {
            return mem->rom[0x4000 * mem->mbc5.rom_bank + (addr - 0x4000)];
        }
        break;
        case 0xa000 ... 0xbfff:
        if (mem->MBC == MBC1 || mem->MBC == MBC1_BATTERY || mem->MBC == MBC1_RAM) {
            return (mem->extram_enable) ? mem->extram[addr & 0x1fff] : 0xff;
        } else if (mem->MBC == MBC5 || mem->MBC == MBC5_BATTERY || mem->MBC == MBC5_RAM ||
                   mem->MBC == MBC5_RUMBLE || mem->MBC == MBC5_RUMBLE_BATTERY ||
                   mem->MBC == MBC5_RUMBLE_RAM) {
            return (mem->extram_enable) ? mem->extram[0x2000 * mem->mbc5.ram_bank + (addr - 0xa000)] : 0xff;
        }
        return 0xff;
        case 0xc000 ... 0xdfff:
        return mem->wram[addr & 0x1fff];
        case 0xe000 ... 0xfdff:
        return mem->eram[addr & 0x1dff];
        case 0xfea0 ... 0xfeff:
        return 0xff;
        case 0xff00 ... 0xff7f:
        return read_io(&mem->io, addr);
        case 0xff80 ... 0xfffe:
        return mem->hram[addr & 0x7f];
        case 0xffff:
        return mem->ie;
    }
}

void write(mem_t* mem, uint16_t addr, uint8_t val) {
    switch(addr) {
        case 0x0000 ... 0x1fff:
        mem->extram_enable = (val & 0xF) == 0xA;
        break;
        case 0x2000 ... 0x3fff: {
            uint8_t mask = val & MBC_BITMASK_LUT[mem->ROM_SIZE];
            if (mem->MBC == MBC1 || mem->MBC == MBC1_BATTERY || mem->MBC == MBC1_RAM) {
                mem->mbc1.rom_bank = (mask == 0) ? 1 : mask & 0x1f;
            } else if (mem->MBC == MBC5 || mem->MBC == MBC5_BATTERY || mem->MBC == MBC5_RAM ||
                       mem->MBC == MBC5_RUMBLE || mem->MBC == MBC5_RUMBLE_BATTERY ||
                       mem->MBC == MBC5_RUMBLE_RAM) {
                if (addr <= 0x2fff) {
                    mem->mbc5.rom_bank = val;
                } else if (addr >= 0x3000 && addr <= 0x3fff) {
                    uint8_t temp = mem->mbc5.rom_bank;
                    setbit8(&temp, 8, val & 1);
                    mem->mbc5.rom_bank = temp;
                }
            }
        }
        break;
        case 0x4000 ... 0x5fff:
        if (mem->MBC == MBC1 || mem->MBC == MBC1_BATTERY || mem->MBC == MBC1_RAM) {
            mem->mbc1.ram_bank = val & 3;
        } else if (mem->MBC == MBC5 || mem->MBC == MBC5_BATTERY || mem->MBC == MBC5_RAM ||
                   mem->MBC == MBC5_RUMBLE || mem->MBC == MBC5_RUMBLE_BATTERY ||
                   mem->MBC == MBC5_RUMBLE_RAM) {
            mem->mbc5.ram_bank = val & 0xf;
        }
        break;
        case 0x6000 ... 0x7fff:
        mem->mbc1.mode = val & 1;
        break;
        case 0xa000 ... 0xbfff:
        if (mem->extram_enable) {
            if (mem->MBC == MBC1 || mem->MBC == MBC1_BATTERY || mem->MBC == MBC1_RAM) {
                if (mem->RAM_SIZE == RAM_2kb || mem->RAM_SIZE == RAM_8kb) {
                    mem->extram[(addr - 0xa000) % mem->RAM_SIZE] = val;
                } else if (mem->RAM_SIZE == RAM_32kb) {
                    if (mem->mbc1.mode)
                        mem->extram[0x2000 * mem->mbc1.ram_bank + (addr - 0xa000)] = val;
                    else
                        mem->extram[addr - 0xa000] = val;
                }
            } else if (mem->MBC == MBC5 || mem->MBC == MBC5_BATTERY || mem->MBC == MBC5_RAM ||
                       mem->MBC == MBC5_RUMBLE || mem->MBC == MBC5_RUMBLE_BATTERY ||
                       mem->MBC == MBC5_RUMBLE_RAM) {
                mem->extram[0x2000 * mem->mbc5.ram_bank + (addr - 0xa000)] = val;
            }
        }
        break;
        case 0xc000 ... 0xdfff:
        mem->wram[addr & 0x1fff] = val;
        break;
        case 0xe000 ... 0xfdff:
        mem->eram[addr & 0x1dff] = val;
        break;
        case 0xfea0 ... 0xfeff:
        break;
        case 0xff00 ... 0xff7f:
        write_io(&mem->io, addr, val);
        break;
        case 0xff80 ... 0xfffe:
        mem->hram[addr & 0x7f] = val;
        break;
        case 0xffff:
        mem->ie = val;
        break;
    }
}

uint8_t read_io(mem_io* io, uint16_t addr) {
    switch(addr & 0xff) {
        case 0x00: return 0xff;
        case 0x04: return io->div;
        case 0x05: return io->tima;
        case 0x06: return io->tma;
        case 0x07: return io->tac;
        case 0x0f: return io->intf;
        case 0x10 ... 0x1e: return 0xff;
        case 0x20 ... 0x26: return 0xff;
        case 0x4d: return 0xff;
        case 0x50: return io->bootrom;
        default:
        printf("IO READ: Unsupported IO %02x\n", addr & 0xff);
        exit(1);
    }
}

void write_io(mem_io* io, uint16_t addr, uint8_t val) {
    switch(addr & 0xff) {
        //case 0x00: handleJoypad(val); break;
        case 0x01: printf("%c", val); break;
        case 0x02: break;
        case 0x04: io->div = 0; break;
        case 0x05: io->tima = val; break;
        case 0x06: io->tma = val; break;
        case 0x07: io->tac = val; break;
        case 0x0f: io->intf = val; break;
        case 0x10 ... 0x1e: break;
        case 0x20 ... 0x26: break; //STUB
        case 0x50: io->bootrom = val; break;
        default:
        printf("IO WRITE: Unsupported IO %02x\n", addr & 0xff);
        exit(1);
    }
}