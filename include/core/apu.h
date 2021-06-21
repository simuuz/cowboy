#pragma once
#include "mem.h"
#include <SDL_audio.h>

constexpr u8 duty_ch1[4] = {
    1, 0x81, 0x87, 0x7E
};

class Apu {
public:
    Apu();
private:
    u8 frequency;
    int frequency_timer;
};