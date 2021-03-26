#pragma once
#include "mbc.h"

static const int BOOTROM_SZ = 0x100;
static const int EXTRAM_SZ = 0x2000;
static const int ERAM_SZ = 0x1e00;
static const int WRAM_SZ = 0x2000;
static const int HRAM_SZ = 0x7f;
static const int ROM_SZ_MIN = 0x8000;

typedef struct mem_io_t {
    uint8_t bootrom, tac, tima, tma, intf, div;
} mem_io;

typedef struct _mem_t {
    mbc1_t mbc1;
    mbc5_t mbc5;
    mem_io io;
    uint8_t ie, MBC, ROM_SIZE, RAM_SIZE;
    uint8_t zero_bank, high_bank;
    uint8_t *rom;
    uint8_t bootrom[BOOTROM_SZ];
    uint8_t extram[EXTRAM_SZ];
    uint8_t wram[WRAM_SZ];
    uint8_t eram[ERAM_SZ];
    uint8_t hram[HRAM_SZ];
    bool skip, extram_enable, rom_opened;
} mem_t;

void init_mem(mem_t* mem, bool skip);
void load_rom(mem_t* mem, char* filename);
bool load_bootrom(mem_t* mem, char* filename);
void reset_mem(mem_t* mem);
uint8_t read(mem_t* mem, uint16_t addr);
void write(mem_t* mem, uint16_t addr, uint8_t val);
uint8_t read_io(mem_io* io, uint16_t addr);
void write_io(mem_io* io, uint16_t addr, uint8_t val);
void handle_joypad(mem_t* mem, uint8_t val);