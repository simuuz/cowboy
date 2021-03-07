#pragma once
#include "mem.h"

class Cpu {
public:
    Cpu();
    void step();
    void reset();
    Mem mem;
    bool halt = false;
private:
    struct registers {
        union {
            struct {
                u8 f, a;
            };
            u16 af;
        };
            
        union {
            struct {
                u8 c, b;
            };
            u16 bc;
        };

        union {
            struct {
                u8 e, d;
            };
            u16 de;
        };

        union {
            struct {
                u8 l, h;
            };
            u16 hl;
        };
        
        u16 sp = 0, pc = 0;
    } regs;

    void updateF(bool z, bool n, bool h, bool c) {
        regs.f = (z << 7) | (n << 6) | (h << 5) | (c << 4)
               | (0 << 3) | (0 << 2) | (0 << 1) | 0;
    }

    bool cond(u8 opcode) {
        if(opcode & 1) return true;
        u8 bits = (opcode >> 3) & 3;
        switch(bits) {
            case 0: return !((regs.f >> 7) & 1);
            case 1: return  ((regs.f >> 7) & 1);
            case 2: return !((regs.f >> 4) & 1);
            case 3: return  ((regs.f >> 4) & 1);
        }    
    }

    template <int group>
    u16 read_r16(u8 bits);
    template <int group>
    void write_r16(u8 bits, u16 val);

    u8 read_r8(u8 bits);
    void write_r8(u8 bits, u8 value);

    void execute(u8 opcode);
    void push(u16 val);
    u16 pop();
    FILE* log;

    bool ime = false;
    bool ei = false;
};