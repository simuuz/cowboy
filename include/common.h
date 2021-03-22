#pragma once
#include <iostream>
#include <cstdint>
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;

template <typename T>
constexpr bool bit(T num, u8 pos) {
    return (num >> pos) & 1;
}

template <typename T>
constexpr void setbit(T& num, u8 pos, bool val) {
    num ^= (-(!!val) ^ num) & (1 << pos);
}