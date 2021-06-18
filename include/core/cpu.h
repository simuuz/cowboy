#pragma once
#include "bus.h"

namespace natsukashii::core
{
using namespace natsukashii::util;

class Cpu
{
public:
  Cpu(Scheduler* scheduler, bool skip, Bus* bus);
  void Step();
  void Reset();
  Bus* bus;
  bool halt = false;
  uint64_t timestamp = 0;
  int cycles = 0;
  void HandleTimers();
  bool skip;
  byte opcode;
  Scheduler* scheduler;

  struct registers
  {
    union
    {
      struct
      {
        byte f, a;
      };
      half af;
    };

    union
    {
      struct
      {
        byte c, b;
      };
      half bc;
    };

    union
    {
      struct
      {
        byte e, d;
      };
      half de;
    };

    union
    {
      struct
      {
        byte l, h;
      };
      half hl;
    };

    half sp = 0, pc = 0;
  } regs;

  template <byte instruction = 0xD3>
  using Handler = void (Cpu::*)();
  using CBHandler = void (Cpu::*)(byte instruction);
private:
  void cbops(byte instruction);
  template <byte instruction>
  void load8();
  template <byte instruction>
  void load16();
  template <byte instruction>
  void alu8();
  template <byte instruction>
  void alu16();
  template <byte instruction>
  void bit8();
  template <byte instruction>
  void branch();
  template <byte instruction>
  void misc();
  template <byte instruction>
  void invalid();

  template <byte instruction>
  static constexpr auto NoPrefixGenerator() -> Handler<instruction>;
  static constexpr auto GenerateTableNP() -> std::array<Handler<>, 256>;
  static constexpr auto GenerateTableCB() -> std::array<CBHandler, 256>;

  std::array<Handler<>, 256> no_prefix{};
  std::array<CBHandler, 256> cb_prefix{};

  void UpdateF(bool z, bool n, bool h, bool c);
  template <byte opcode>
  bool Cond();

  template <int group, byte bits>
  half ReadR16();
  template <int group, byte bits>
  void WriteR16(half val);

  template <byte bits>
  byte ReadR8();
  template <byte bits>
  void WriteR8(byte value);

  byte ReadR8(byte bits);
  void WriteR8(byte bits, byte value);

  void Push(half val);
  half Pop();
  FILE* log;
  void HandleInterrupts();
  int tima_cycles = 0;
  int div_cycles = 0;

  bool ime = false;
  bool ei = false;
};
}  // namespace natsukashii::core

static const int opcycles[256] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    4,  12, 8,  8,  4,  4,  8,  4,  20, 8,  8,  8, 4,  4,  8, 4,   // 0
    4,  12, 8,  8,  4,  4,  8,  4,  12, 8,  8,  8, 4,  4,  8, 4,   // 1
    8,  12, 8,  8,  4,  4,  8,  4,  8,  8,  8,  8, 4,  4,  8, 4,   // 2
    8,  12, 8,  8,  12, 12, 12, 4,  8,  8,  8,  8, 4,  4,  8, 4,   // 3
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,   // 4
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,   // 5
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,   // 6
    8,  8,  8,  8,  8,  8,  4,  8,  4,  4,  4,  4, 4,  4,  8, 4,   // 7
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,   // 8
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,   // 9
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,   // A
    4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4, 4,  4,  8, 4,   // B
    8,  12, 12, 16, 12, 16, 8,  16, 8,  16, 12, 0, 12, 24, 8, 16,  // C
    8,  12, 12, 0,  12, 16, 8,  16, 8,  16, 12, 0, 12, 0,  8, 16,  // D
    12, 12, 8,  0,  0,  16, 8,  16, 16, 4,  16, 0, 0,  0,  8, 16,  // E
    12, 12, 8,  4,  0,  16, 8,  16, 12, 8,  16, 4, 0,  0,  8, 16   // F
};

static const int cbopcycles[256] = {
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 0
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 1
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 2
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 3
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 4
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 5
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 6
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 7
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 8
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // 9
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // A
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // B
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // C
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // D
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,  // E
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8   // F
};  // 0  1  2  3  4  5  6   7  8  9  A  B  C  D  E   F