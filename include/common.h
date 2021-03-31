#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>

using byte = uint8_t;
using half = uint16_t;
using word = uint32_t;
using sbyte = int8_t;
using shalf = int16_t;
using sword = int32_t;

template <typename T>
static constexpr bool bit(T num, byte pos) {
    return (num >> pos) & 1;
}

template <typename T, byte pos>
static constexpr bool bit(T num) {
    return (num >> pos) & 1;
}

template <typename T>
void setbit(T& num, uint8_t pos, bool val) {
    *num ^= (-(!!val) ^ *num) & (1 << pos);
}

template <typename T, byte pos>
void setbit(T& num, bool val) {
    num ^= (-(!!val) ^ num) & (1 << pos);
}