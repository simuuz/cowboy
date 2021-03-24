#pragma once
#include "bus.h"

typedef struct registers {
    union {
        struct {
            uint8_t f, a;
        };
        uint16_t af;
    };
        
    union {
        struct {
            uint8_t c, b;
        };
        uint16_t bc;
    };

    union {
        struct {
            uint8_t e, d;
        };
        uint16_t de;
    };

    union {
        struct {
            uint8_t l, h;
        };
        uint16_t hl;
    };
    
    uint16_t sp, pc;
} regs_t;

typedef struct _cpu_t {
    FILE* log;
    bus_t* bus;
    regs_t regs;
    bool halt;
    bool skip;
    bool ime;
    bool ei;
    int total_cycles;
    int cycles;
    int tima_cycles;
    int div_cycles;
} cpu_t;

void init_cpu(cpu_t* cpu, bool skip);

void step_cpu(cpu_t* cpu);
void reset_cpu(cpu_t* cpu);
void handle_timers(cpu_t* cpu);

void update_f(uint8_t* f, bool z, bool n, bool h, bool c);
bool cond(uint8_t* f, uint8_t opcode);

uint16_t read_r16(regs_t regs, int group, uint8_t bits);
void write_r16(regs_t* regs, int group, uint8_t bits, uint16_t val);

uint8_t read_r8(cpu_t cpu, uint8_t bits);
void write_r8(cpu_t* cpu, uint8_t bits, uint8_t value);

void execute(cpu_t* cpu, uint8_t opcode);
void push(bus_t* bus, uint16_t* sp, uint16_t val);
uint16_t pop(bus_t* bus,uint16_t* sp);
void handle_interrupts(cpu_t* cpu);

static const int opcycles[256] = {
  //0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    4,  12, 8,  8,  4,  4,  8,  4,  20, 8,  8,  8,  4,  4,  8,  4, //0
	4,  12, 8,  8,  4,  4,  8,  4,  12, 8,  8,  8,  4,  4,  8,  4, //1
    8,  12, 8,  8,  4,  4,  8,  4,  8,  8,  8,  8,  4,  4,  8,  4, //2
    8,  12, 8,  8,  12, 12, 12, 4,  8,  8,  8,  8,  4,  4,  8,  4, //3 
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, //4
	4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, //5
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, //6
    8,  8,  8,  8,  8,  8,  4,  8,  4,  4,  4,  4,  4,  4,  8,  4, //7
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, //8
	4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, //9
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, //A
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, //B
    8,  12, 12, 16, 12, 16, 8,  16, 8,  16, 12, 0,  12, 24, 8, 16, //C
	8,  12, 12, 0,  12, 16, 8,  16, 8,  16, 12, 0,  12, 0,  8, 16, //D
    12, 12, 8,  0,  0,  16, 8,  16, 16, 4,  16, 0,  0,  0,  8, 16, //E
    12, 12, 8,  4,  0,  16, 8,  16, 12, 8,  16, 4,  0,  0,  8, 16  //F
};

static const int cbopcycles[256] = {
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //0
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //1
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //2
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //3
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //4
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //5
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //6
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //7
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //8
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //9
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //A
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //B
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //C
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //D
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8, //E
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8  //F
};//0  1  2  3  4  5  6   7  8  9  A  B  C  D  E   F