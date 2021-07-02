#pragma once
#include "bus.h"

namespace natsukashii::core
{
class Cpu
{
public:
  Cpu(bool skip, Bus* bus);
  void Step();
  void Reset();
  void SaveState(int slot);
  Bus* bus;
  bool halt = false;
  u64 total_cycles = 0;
  u8 cycles = 0;
  void HandleTimers();
  bool skip;
  u8 opcode;
  struct registers
  {
    union
    {
      struct
      {
        u8 f, a;
      };
      u16 af;
    };

    union
    {
      struct
      {
        u8 c, b;
      };
      u16 bc;
    };

    union
    {
      struct
      {
        u8 e, d;
      };
      u16 de;
    };

    union
    {
      struct
      {
        u8 l, h;
      };
      u16 hl;
    };

    u16 sp = 0, pc = 0;
  } regs;

private:
  void UpdateF(bool z, bool n, bool h, bool c);
  bool Cond(u8 opcode);

  template <int group>
  u16 ReadR16(u8 bits);
  template <int group>
  void WriteR16(u8 bits, u16 val);

  u8 ReadR8(u8 bits);
  void WriteR8(u8 bits, u8 value);

  void Execute(u8 opcode);
  void Push(u16 val);
  u16 Pop();
  FILE* log;
  void HandleInterrupts();
  int tima_cycles = 0;
  int div_cycles = 0;

  bool ime = false;
  bool ei = false;
};
}  // namespace natsukashii::core