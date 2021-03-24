#pragma once
#include "common.h"

enum MBC {
    None, MBC1, MBC1_RAM, MBC1_BATTERY,
    MBC2 = 0x05, MBC2_BATTERY, RAM = 0x08, BATTERY,
    MBC3_RTC = 0x0F, MBC3_RTC_BATTERY, MBC3, MBC3_RAM, MBC3_BATTERY,
    MBC5 = 0x19, MBC5_RAM, MBC5_BATTERY, MBC5_RUMBLE, MBC5_RUMBLE_RAM,
    MBC5_RUMBLE_BATTERY, MBC6 = 0x20, MBC7 = 0x22
};

enum ROM_SIZE {
    ROM_32kb, ROM_64kb,
    ROM_128kb, ROM_256kb,
    ROM_512kb, ROM_1mb,
    ROM_2mb, ROM_4mb, ROM_8mb
};

enum RAM_SIZE {
    RAM_None, RAM_2kb,
    RAM_8kb, RAM_32kb,
    RAM_128kb, RAM_64kb
};

const char* MBCs[] = {
    "ROM Only", "MBC1", "MBC1+RAM", "MBC1+RAM+Battery", [5] = "MBC2", "MBC2+Battery", [8] = "ROM+RAM", "ROM+RAM+Battery",
    [0xB] = "MMM01", "MMM01+RAM", "MMM01+RAM+Battery", [0xF] = "MBC3+Timer+Battery", "MBC3+Timer+RAM+Battery",
    "MBC3", "MBC3+RAM", "MBC3+RAM+Battery", [0x19] = "MBC5", "MBC5+RAM", "MBC5+RAM+Battery",
    "MBC5+Rumble", "MBC5+Rumble+RAM", "MBC5+Rumble+RAM+Battery", [0x20] = "MBC6"
};

const char* ROMs[9] = {
    " 32 KiB",
    " 64 KiB",
    "128 KiB",
    "256 KiB",
    "512 KiB",
    "  1 MiB",
    "  2 MiB",
    "  4 MiB",
    "  8 MiB"
};

const char* RAMs[6] = {
    "   None",
    "  2 KiB",
    "  8 KiB",
    " 32 KiB",
    "128 KiB",
    " 64 KiB"
};

const uint8_t MBC_BITMASK_LUT[9] = {
    [1] = 3, 7, 0xF, 0x1F, 0x1F, 0x1F
};

typedef struct _mbc1_t {
    unsigned rom_bank:5;
    unsigned ram_bank:2;
    unsigned mode:1;
} mbc1_t;

typedef struct _mbc5_t {
    unsigned rom_bank:9;
    unsigned ram_bank:4;
} mbc5_t;