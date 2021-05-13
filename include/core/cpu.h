#pragma once
#include "bus.h"

class Cpu
{
public:
  Cpu(bool skip);
  void step();
  void reset();
  Bus bus;
  bool halt = false;
  int total_cycles = 0;
  int cycles = 0;
  void handle_timers();
  bool skip;

private:
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

  void update_f(bool z, bool n, bool h, bool c);
  bool cond(byte opcode);

  template <int group>
  half read_r16(byte bits);
  template <int group>
  void write_r16(byte bits, half val);

  byte read_r8(byte bits);
  void write_r8(byte bits, byte value);

  void execute(byte opcode);
  void push(half val);
  half pop();
  FILE* log;
  void handle_interrupts();
  int tima_cycles = 0;
  int div_cycles = 0;

  bool ime = false;
  bool ei = false;
};

inline int opcycles[256] = {
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

inline int cbopcycles[256] = {
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