#pragma once
#include "common.h"
#include <array>
#include <vector>

class Mem {
public:
    Mem(std::string bootrom, std::string rom);
private:
    std::vector<u8> rom;
    std::array<u8, 0x100> bootrom;
    std::array<u8, 0x2000> vram;
    std::array<u8, 0x2000> extram;
    std::array<u8, 0x2000> wram;
    std::array<u8, 0x1e00> eram;
    std::array<u8, 0xa0> oam;
    struct IO {
        u8 bootrom = 0;
        void write(u32 addr, u8 val) {
            switch(addr & 0xff) {
                case 0x50: bootrom = val; break;
                default:
                printf("Unsupported IO %02x\n", addr & 0xff);
                exit(1);
            }
        }
        u8 read(u32 addr) {
            switch(addr & 0xff) {
                case 0x50: return bootrom;
                default:
                printf("Unsupported IO %02x\n", addr & 0xff);
                exit(1);
            }
        }
    } io;
    std::array<u8, 0x7f> hram;
    u8 ie = 0;
    template <typename T>
    T read(u16 addr);
    template <typename T>
    void write(u16 addr, T val);
};