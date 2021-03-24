#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

static bool bit8(uint8_t num, uint8_t pos) {
    return (num >> pos) & 1;
}

static bool bit16(uint16_t num, uint8_t pos) {
    return (num >> pos) & 1;
}

static bool bit32(uint32_t num, uint8_t pos) {
    return (num >> pos) & 1;
}

static void setbit8(uint8_t* num, uint8_t pos, bool val) {
    *num ^= (-(!!val) ^ *num) & (1 << pos);
}

static void setbit16(uint16_t* num, uint8_t pos, bool val) {
    *num ^= (-(!!val) ^ *num) & (1 << pos);
}

static void setbit32(uint32_t* num, uint8_t pos, bool val) {
    *num ^= (-(!!val) ^ *num) & (1 << pos);
}