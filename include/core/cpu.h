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
                    struct {
                        union {
                            struct {
                                unsigned:1;unsigned:1;unsigned:1;unsigned:1;
                                unsigned carry:1;unsigned hcarry:1;unsigned neg:1;unsigned zero:1;
                            };
                            u8 f;
                        };
                    };
                    u8 a;
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

    template <int group>
    u16 read_r16(u8 bits);
    template <int group>
    void write_r16(u8 bits, u16 val);

    u8 read_r8(u8 bits);
    void write_r8(u8 bits, u8 value);

    void push(u16 val);
    u16 pop();

    const bool conds[4] = {
        !regs.zero, regs.zero,
        !regs.carry, regs.carry
    };
};