#pragma once
#include "bus.h"
#include "cpu.h"

constexpr int cycles_frame = 4194300 / 60;

class Emu {
public:
    Emu(bool skip) : cpu(skip) {  }
    void run();
    void step();
    void reset();
    Cpu cpu;
};