#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>
#include "ini.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

template <typename T>
static constexpr bool bit(T num, u8 pos)
{
  return (num >> pos) & 1;
}

template <typename T, u8 pos>
static constexpr bool bit(T num)
{
  return (num >> pos) & 1;
}

template <typename T>
void setbit(T& num, u8 pos, bool val)
{
  num ^= (-(!!val) ^ num) & (1 << pos);
}

template <typename T, u8 pos>
void setbit(T& num, bool val)
{
  num ^= (-(!!val) ^ num) & (1 << pos);
}

namespace natsukashii::util
{
template <typename T, T Begin, class Func, T ...Is>
static constexpr void static_for_impl(Func&& f, std::integer_sequence<T, Is...>) {
  (f(std::integral_constant<T, Begin + Is>{ }), ...);
}

template <typename T, T Begin, T End, class Func>
static constexpr void static_for(Func&& f) {
  static_for_impl<T, Begin>(std::forward<Func>(f), std::make_integer_sequence<T, End - Begin>{ });
}
} // natsukashii::util