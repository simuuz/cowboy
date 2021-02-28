#pragma once
#include "mem.h"

class Cpu {
public:
    Cpu(Mem& mem);
    void step();
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
    Mem mem;

    struct Flags {
        bool z = false, n = false, h = false, c = false;
        void set(u8& f, bool z, bool n, bool h, bool c) {
            this->z = z;
            this->n = n;
            this->h = h;
            this->c = c;
            f = (z << 7) | (n << 6) | (h << 5) | (c << 4)
              | (0 << 3) | (0 << 2) | (0 << 1) | 0;
        }
    };

    Flags flags;

    bool cond(u8 bits) {
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
};