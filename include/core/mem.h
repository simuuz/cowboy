#pragma once
#include "common.h"
#include <array>
#include <vector>

class Mem {
public:
    Mem();
    void loadROM(std::string filename);
    void loadBootROM(std::string filename);
    void reset();
    bool rom_opened = false;
    template <typename T>
    T read(u16 addr, u16& pc, bool inc = true);
    template <typename T>
    void write(u16 addr, T val);
private:
    std::vector<u8> rom;
    std::array<u8, BOOTROM_SZ> bootrom;
    std::array<u8, VRAM_SZ> vram;
    std::array<u8, EXTRAM_SZ> extram;
    std::array<u8, WRAM_SZ> wram;
    std::array<u8, ERAM_SZ> eram;
    std::array<u8, OAM_SZ> oam;
    std::array<u8, HRAM_SZ> hram;

    struct IO {
        //u8 bootrom = 0, bgp = 0, scy = 0, scx = 0, lcdc = 0, tac = 0, intf = 0;
        u8 bootrom = 1, bgp = 0xfc, scy = 0, scx = 0, lcdc = 0x91, tac = 0, intf = 0;
        void write(u16 addr, u8 val);
        u8 read(u16 addr);
    } io;

    u8 ie = 0;

    template <typename T>
    void Write(void* buffer, u16 addr, T val);
    template <typename T>
    T Read(void* buffer, u16 addr);
};