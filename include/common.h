#pragma once
#include <iostream>
#include <cstdint>

constexpr int BOOTROM_SZ = 0x100;
constexpr int VRAM_SZ = 0x2000;
constexpr int EXTRAM_SZ = 0x2000;
constexpr int ERAM_SZ = 0x1e00;
constexpr int WRAM_SZ = 0x2000;
constexpr int OAM_SZ = 0xa0;
constexpr int HRAM_SZ = 0x7f;
constexpr int ROM_SZ_MIN = 0x8000;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;