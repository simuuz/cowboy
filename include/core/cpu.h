#pragma once
#include "common.h"

class Cpu {
public:
    Cpu();
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
};