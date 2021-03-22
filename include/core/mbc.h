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

const std::string MBCs[33] = {
    "ROM Only", "MBC1", "MBC1+RAM", "MBC1+RAM+Battery", "", "MBC2", "MBC2+Battery", "", "ROM+RAM", "ROM+RAM+Battery",
    "", "MMM01", "MMM01+RAM", "MMM01+RAM+Battery", "", "MBC3+Timer+Battery", "MBC3+Timer+RAM+Battery",
    "MBC3", "MBC3+RAM", "MBC3+RAM+Battery", "", "", "", "", "", "MBC5", "MBC5+RAM", "MBC5+RAM+Battery",
    "MBC5+Rumble", "MBC5+Rumble+RAM", "MBC5+Rumble+RAM+Battery", "", "MBC6"
};

const std::string ROMs[9] = {
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

const std::string RAMs[6] = {
    "   None",
    "  2 KiB",
    "  8 KiB",
    " 32 KiB",
    "128 KiB",
    " 64 KiB"
};

const u8 MBC_BITMASK_LUT[9] = {
    0, 3, 7, 0xF, 0x1F, 0x1F, 0x1F, 0, 0
};