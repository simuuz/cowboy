#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include "ini.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <utility>

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

static const std::string mbcs[35] = {
  "ROM ONLY",
  "MBC1",
  "MBC1+RAM",
  "MBC1+RAM+BATTERY",
  "",
  "MBC2",
  "MBC2+BATTERY",
  "",
  "ROM+RAM",
  "ROM+RAM+BATTERY",
  "",
  "MMM01",
  "MMM01+RAM",
  "MMM01+RAM+BATTERY",
  "",
  "MBC3+TIMER+BATTERY",
  "MBC3+TIMER+RAM+BATTERY",
  "MBC3",
  "MBC3+RAM",
  "MBC3+RAM+BATTERY",
  "",
  "",
  "",
  "",
  "",
  "MBC5",
  "MBC5+RAM",
  "MBC5+RAM+BATTERY",
  "MBC5+RUMBLE",
  "MBC5+RUMBLE+RAM",
  "MBC5+RUMBLE+RAM+BATTERY",
  "",
  "MBC6",
  "",
  "MBC7+SENSOR+RUMBLE+RAM+BATTERY"
};

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