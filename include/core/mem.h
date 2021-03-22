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

struct {
    unsigned rom_bank:5;
    unsigned ram_bank:2;
    unsigned mode:1;
} mbc1_t;

struct {
    unsigned rom_bank:9;
    unsigned ram_bank:4;
} mbc5_t;

class Mem {
public:
    Mem(bool skip);
    void load_rom(std::string filename);
    void load_bootrom(std::string filename);
    void reset();
    template <typename T>
    T read(u16 addr);
    template <typename T>
    void write(u16 addr, T val);
    
    u8 ie = 0;

    bool skip;

    struct IO {
        u8 bootrom = 1, tac = 0, tima = 0, tma = 0, intf = 0, div = 0;
        void write(u16 addr, u8 val);
        u8 read(u16 addr);
        void handle_joypad(u8 val);
    } io;
    friend class Ppu;
private:
    std::vector<u8> rom;
    std::array<u8, BOOTROM_SZ> bootrom;
    std::array<u8, EXTRAM_SZ> extram;
    std::array<u8, WRAM_SZ> wram;
    std::array<u8, ERAM_SZ> eram;
    std::array<u8, HRAM_SZ> hram;

    u8 MBC;
    u8 ROM_SIZE;
    u8 RAM_SIZE;
    bool extram_enable = false;
    u8 zero_bank = 0;
    u8 high_bank = 0;

    template <typename T>
    void Write(void* buffer, u16 addr, T val);
    template <typename T>
    T Read(void* buffer, u16 addr);
};