#pragma once
#include "common.h"
#include <array>
#include <vector>

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

const std::string MBCs[35] = {
    "None", "MBC1", "MBC + RAM", "MBC1 + Battery-buffered RAM",
    "", "MBC2", "MBC2 + Battery-buffered RAM", "", "None + RAM", "None + Battery-buffered RAM",
    "", "MMM01", "MMM01 + RAM", "MMM01 + Battery-buffered RAM", "",
    "MBC3 + RTC", "MBC3 + RTC + Battery-buffered RAM", "MBC3", "MBC3 + RAM",
    "MBC3 + Battery-buffered RAM", "", "", "", "", "", "MBC5", "MBC5 + RAM",
    "MBC5 + Battery-buffered RAM", "MBC5 + Rumble", "MBC5 + Rumble + RAM", "MBC5 + Rumble + Battery-buffered RAM",
    "", "MBC6", "", "MBC7"
};

const std::string ROMs[9] = {
    "32 KiB", "64 KiB", "128 KiB", "256 KiB", "512 KiB", "1 MiB", "2 MiB", "4 MiB", "8 MiB"
};

const std::string RAMs[6] = {
    "None", "2 KiB", "8 KiB", "32 KiB", "128 KiB", "64 KiB"
};

const u8 MBC1_BITMASK_LUT[9] = {
    0, 3, 7, 0xF, 0x1F, 0x1F, 0x1F, 0, 0
};

class Mem {
public:
    Mem(bool skip);
    void loadROM(std::string filename);
    void loadBootROM(std::string filename);
    void reset();
    template <typename T>
    T read(u16 addr, u16& pc, bool inc = true);
    template <typename T>
    void write(u16 addr, T val);
    
    u8 ie = 0;

    bool skip;

    struct IO {
        u8 bootrom = 1, bgp = 0xfc, scy = 0, scx = 0, lcdc = 0x91, tac = 0, tima = 0, tma = 0, intf = 0, div = 0;
        //u8 bootrom = 0, bgp = 0, scy = 0, scx = 0, lcdc = 0, tac = 0, tima = 0, tma = 0, intf = 0, div = 0;
        void write(u16 addr, u8 val);
        u8 read(u16 addr);
        void handleJoypad(u8 val);
    } io;
private:
    std::vector<u8> rom;
    std::array<u8, BOOTROM_SZ> bootrom;
    std::array<u8, VRAM_SZ> vram;
    std::array<u8, EXTRAM_SZ> extram;
    std::array<u8, WRAM_SZ> wram;
    std::array<u8, ERAM_SZ> eram;
    std::array<u8, OAM_SZ> oam;
    std::array<u8, HRAM_SZ> hram;

    u8 MBC = MBC::None;
    u8 ROM_SIZE = ROM_SIZE::ROM_32kb;
    u8 RAM_SIZE = RAM_SIZE::RAM_None;
    bool extram_enable = false;
    u8 rom_bank = 0;
    u8 ram_bank = 0;
    bool mode;

    template <typename T>
    void Write(void* buffer, u16 addr, T val);
    template <typename T>
    T Read(void* buffer, u16 addr);
};