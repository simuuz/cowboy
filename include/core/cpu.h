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
        struct {
		    union {
			    struct {
                    u8 f, a;
			    };
			    u16 af;
            };
        };
        
        struct {
		    union {
			    struct {
                    u8 c, b;
			    };
			    u16 bc;
            };
        };
        
        struct {
            union {
                struct {
                    u8 e, d;
                };
                u16 de;
            };
        };
        
        struct {
            union {
                struct {
                    u8 l, h;
                };
                u16 hl;
            };
        };
        
        u16 sp = 0, pc = 0;
    };

    registers regs;

    struct Flags {
        bool z = false, n = false, h = false, c = false;
        void set(u8& f) {
            f = (z << 7) | (n << 6) | (h << 5) | (c << 4)
              | (0 << 3) | (0 << 2) | (0 << 1) | 0;
        }
    };

    Flags flags;

    bool cond(u8 opcode) {
        if(opcode & 1) return true;
        u8 bits = (opcode >> 3) & 3;
        switch(bits) {
            case 0: return !flags.z;
            case 1: return  flags.z;
            case 2: return !flags.c;
            case 3: return  flags.c;
        }    
    }

    template <int group>
    u16 read_r16(u8 bits);
    template <int group>
    void write_r16(u8 bits, u16 val);

    u8 read_r8(u8 bits);
    void write_r8(u8 bits, u8 value);

    void push(u16 val);
    u16 pop();
    FILE* log;

    bool ime = false;
    bool ei = false;
};