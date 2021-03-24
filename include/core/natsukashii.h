#pragma once
#include "bus.h"
#include "cpu.h"

static const int cycles_frame = 4194300 / 60;

typedef struct _emu {
    bool rom_opened;
    cpu_t cpu;
    bus_t bus;
} emu_t;

void init_emu(emu_t* emu, bool skip);
void run(emu_t* emu);
void step_emu(emu_t* emu);
void reset_emu(emu_t* emu);