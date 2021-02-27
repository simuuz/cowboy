#pragma once
#include "common.h"
#include <array>
#include <vector>

class Mem {
public:
    Mem(std::string bootrom, std::string rom);
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
        u8 bootrom = 0xff;
        void write(u32 addr, u8 val) {
            switch(addr & 0xff) {
                case 0x50: bootrom = val; break;
                default:
                printf("IO WRITE: Unsupported IO %02x\n", addr & 0xff);
                exit(1);
            }
        }

        u8 read(u32 addr) {
            switch(addr & 0xff) {
                case 0x50: return bootrom;
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