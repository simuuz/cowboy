#pragma once
#include "mbc.h"
#include <array>
#include <vector>
#include <iterator>

constexpr int BOOTROM_SZ = 0x100;
constexpr int EXTRAM_SZ = 0x2000;
constexpr int ERAM_SZ = 0x1e00;
constexpr int WRAM_SZ = 0x2000;
constexpr int HRAM_SZ = 0x7f;
constexpr int ROM_SZ_MIN = 0x8000;

class Mem {
public:
    Mem(bool skip);
    void load_rom(std::string filename);
    bool load_bootrom(std::string filename);
    void reset();
    byte read(half addr);
    void write(half addr, byte val);
    
    byte ie = 0;
    bool skip;

    struct IO {
        byte bootrom = 1, tac = 0, tima = 0, tma = 0, intf = 0, div = 0;
        void handle_joypad(byte val);
    } io;
    friend class Ppu;
    bool rom_opened = false;
private:
    void write_io(half addr, byte val);
    byte read_io(half addr);
    std::vector<byte> rom;
    byte bootrom[BOOTROM_SZ];
    byte extram[EXTRAM_SZ];
    byte wram[WRAM_SZ];
    byte eram[ERAM_SZ];
    byte hram[HRAM_SZ];

    mbc1_t mbc1;
    mbc5_t mbc5;
    byte MBC;
    byte ROM_SIZE;
    byte RAM_SIZE;
    bool extram_enable = false;
    byte zero_bank = 0;
    byte high_bank = 0;
};