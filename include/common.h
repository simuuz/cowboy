#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>

using byte = uint8_t;
using half = uint16_t;
using word = uint32_t;
using sbyte = int8_t;
using shalf = int16_t;
using sword = int32_t;

template <typename T>
static constexpr bool bit(T num, byte pos)
{
  return (num >> pos) & 1;
}

template <typename T, byte pos>
static constexpr bool bit(T num)
{
  return (num >> pos) & 1;
}

template <typename T>
void setbit(T& num, byte pos, bool val)
{
  num ^= (-(!!val) ^ num) & (1 << pos);
}

template <typename T, byte pos>
void setbit(T& num, bool val)
{
  num ^= (-(!!val) ^ num) & (1 << pos);
}

static const std::string mbcs[35] = {
  /* $00 */ "ROM ONLY",
  /* $01 */ "MBC1",
  /* $02 */ "MBC1+RAM",
  /* $03 */ "MBC1+RAM+BATTERY",
  /* $04 */ "",
  /* $05 */ "MBC2",
  /* $06 */ "MBC2+BATTERY",
  /* $07 */ "",
  /* $08 */ "ROM+RAM",
  /* $09 */ "ROM+RAM+BATTERY",
  /* $0A */ "",
  /* $0B */ "MMM01",
  /* $0C */ "MMM01+RAM",
  /* $0D */ "MMM01+RAM+BATTERY",
  /* $0E */ "",
  /* $0F */ "MBC3+TIMER+BATTERY",
  /* $10 */ "MBC3+TIMER+RAM+BATTERY",
  /* $11 */ "MBC3",
  /* $12 */ "MBC3+RAM",
  /* $13 */ "MBC3+RAM+BATTERY",
  /* $14 */ "",
  /* $15 */ "",
  /* $16 */ "",
  /* $17 */ "",
  /* $18 */ "",
  /* $19 */ "MBC5",
  /* $1A */ "MBC5+RAM",
  /* $1B */ "MBC5+RAM+BATTERY",
  /* $1C */ "MBC5+RUMBLE",
  /* $1D */ "MBC5+RUMBLE+RAM",
  /* $1E */ "MBC5+RUMBLE+RAM+BATTERY",
  /* $1F */ "",
  /* $20 */ "MBC6",
  /* $21 */ "",
  /* $22 */ "MBC7+SENSOR+RUMBLE+RAM+BATTERY"
};