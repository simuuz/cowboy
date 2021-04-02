#include "natsukashii.h"

void Emu::reset() {
    cpu.reset();
    cpu.bus.reset();
}

void Emu::run() {
    while(cpu.total_cycles < cycles_frame) {
        cpu.handle_timers();
        cpu.bus.ppu.step(cpu.total_cycles);
        cpu.step();
    }

    cpu.total_cycles -= cycles_frame;
}

void Emu::step() {
    cpu.handle_timers();
    cpu.bus.ppu.step(cpu.total_cycles);
    cpu.step();

    if(cpu.total_cycles >= cycles_frame) {
        cpu.total_cycles -= cycles_frame;
    }
}