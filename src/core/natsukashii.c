#include "natsukashii.h"

void init_emu(emu_t *emu, bool skip) {
    init_cpu(&emu->cpu, skip);
    init_bus(&emu->bus, skip);
    emu->rom_opened = false;
}

void run(emu_t *emu) {
    while(emu->cpu.total_cycles < cycles_frame) {
        handle_timers(&emu->cpu);
        step_ppu(&emu->cpu.bus->ppu, emu->cpu.total_cycles);
        step_cpu(&emu->cpu);
    }

    emu->cpu.total_cycles -= cycles_frame;
}

void step_emu(emu_t* emu) {
    handle_timers(&emu->cpu);
    step_ppu(&emu->cpu.bus->ppu, emu->cpu.total_cycles);
    step_cpu(&emu->cpu);

    if(emu->cpu.total_cycles >= cycles_frame) {
        emu->cpu.total_cycles -= cycles_frame;
    }
}

void reset_emu(emu_t* emu) {
    reset_cpu(&emu->cpu);
    emu->rom_opened = false;
}