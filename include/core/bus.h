#pragma once
#include "ppu.h"

typedef struct _bus_t {
    bool rom_opened;
    mem_t* mem;
    ppu_t* ppu;
} bus_t;

void init_bus(bus_t* bus,bool skip);
uint8_t read_byte(bus_t* bus, uint16_t addr);
uint8_t next_byte(bus_t* bus, uint16_t addr, uint16_t* pc);
void write_byte(bus_t* bus, uint16_t addr, uint8_t val);
uint16_t read_word(bus_t* bus, uint16_t addr);
uint16_t next_word(bus_t* bus, uint16_t addr, uint16_t* pc);
void write_word(bus_t* bus, uint16_t addr, uint16_t val);
void reset_bus(bus_t* bus);