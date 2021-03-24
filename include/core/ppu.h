#pragma once
#include "mem.h"

static const int VRAM_SZ = 0x2000;
static const int OAM_SZ = 0xa0;

typedef struct ppu_IO {
    uint8_t bgp, scy, scx, lcdc;
    uint8_t wx, wy, obp0, obp1;
    uint8_t lyc, ly, stat;
} ppu_io;

typedef struct _ppu_t {
    uint8_t pixels[160*144*3];
    uint8_t background[256*256*3];
    uint8_t window[256*256*3];
    uint8_t vram[VRAM_SZ];
    uint8_t oam[OAM_SZ];
    ppu_io io;
} ppu_t;

uint8_t read_io_ppu(ppu_io* io, uint16_t addr);
void write_io_ppu(ppu_t* ppu, mem_t* mem, uint16_t addr, uint8_t val);

void init_ppu(ppu_t* ppu, bool skip);
void step_ppu(ppu_t* ppu, int cycles);

static const uint8_t palette[12] = {
    0x9f, 0xf4, 0xe5,
    0x00, 0xb9, 0xbe,
    0x00, 0x5f, 0x8c,
    0x00, 0x2b, 0x59
};

void do_background(ppu_t* ppu);