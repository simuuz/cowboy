#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

bool bit8(uint8_t num, uint8_t pos) {
    return (num >> pos) & 1;
}

bool bit16(uint16_t num, uint8_t pos) {
    return (num >> pos) & 1;
}

bool bit32(uint32_t num, uint8_t pos) {
    return (num >> pos) & 1;
}

void setbit8(uint8_t* num, uint8_t pos, bool val) {
    *num ^= (-(!!val) ^ *num) & (1 << pos);
}

void setbit16(uint16_t* num, uint8_t pos, bool val) {
    *num ^= (-(!!val) ^ *num) & (1 << pos);
}

void setbit32(uint32_t* num, uint8_t pos, bool val) {
    *num ^= (-(!!val) ^ *num) & (1 << pos);
}