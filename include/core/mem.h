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
    friend class Cpu;
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
        u8 bootrom = 0xff, bgp, scy, scx, lcdc;
        void write(u32 addr, u8 val) {
            switch(addr & 0xff) {
                case 0x47: bgp = val; break;
                case 0x50: bootrom = val; break;
                case 0x10 ... 0x1e: case 0x20 ... 0x26:
                break; //STUB
                case 0x42: scy = val; break;
                case 0x43: scx = val; break;
                case 0x40: lcdc = val; break;
                default:
                printf("IO WRITE: Unsupported IO %02x\n", addr & 0xff);
                exit(1);
            }
        }

        u8 read(u32 addr) {
            switch(addr & 0xff) {
                case 0x47: return bgp;
                case 0x50: return bootrom;
                case 0x10 ... 0x1e: case 0x20 ... 0x26:
                return 0xff; //STUB
                case 0x44: return 0x90;
                case 0x42: return scy;
                case 0x43: return scx;
                case 0x40: return lcdc;
                default:
                printf("IO READ: Unsupported IO %02x\n", addr & 0xff);
                exit(1);
            }
        }
    } io;

    u8 ie = 0;

    template <typename T>
    T Read(void* buffer, u16 addr);

    template <typename T>
    T read(u16 addr, u16& pc, bool inc = true);

    template <typename T>
    void Write(void* buffer, u16 addr, T val);

    template <typename T>
    void write(u16 addr, T val);
};