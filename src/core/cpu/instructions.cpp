#include "cpu.h"

namespace natsukashii::core
{
void Cpu::invalid()
{
  printf("Unrecognized opcode: %02x\n", opcode);
  exit(1);
}

void Cpu::misc()
{
  if constexpr (opcode == 0x76) {
    halt = true;
  } else if constexpr (opcode == 0xCB) {
    cb_opcode = bus->NextByte(regs.pc, regs.pc);
    cycles = cbopcycles[cb_opcode];
    cb_prefix[cb_opcode]();
  } else if constexpr (opcode == 0xF3) {
    ime = false;
  } else if constexpr (opcode == 0xFB) {
    ime = true;
  }
}

void Cpu::branch()
{
  if constexpr (opcode == 0x18) {                          // JR
    regs.pc += (sbyte)bus->NextByte(regs.pc, regs.pc);
  } else if constexpr (opcode == 0x20 || opcode == 0x28 ||
                       opcode == 0x30 || opcode == 0x38) {
    sbyte offset = (sbyte)bus->NextByte(regs.pc, regs.pc);
    if (Cond(opcode))
    {
      regs.pc += offset;
      cycles += 4;
    }
  } else if constexpr (opcode == 0xC0 || opcode == 0xC8 || // RET
                       opcode == 0xC9 || opcode == 0xD0 ||
                       opcode == 0xD8 || opcode == 0xD9) {
    if (Cond(opcode))
    {
      regs.pc = Pop();
      cycles += 12;
    }
  } else if constexpr (opcode == 0xE9) {                   // JP
    regs.pc = regs.hl;
  } else if constexpr (opcode == 0xC2 || opcode == 0xC3 ||
                       opcode == 0xCA || opcode == 0xD2 ||
                       opcode == 0xDA) {
    half location = bus->NextHalf(regs.pc, regs.pc);
    if(Cond(opcode))
    {
      regs.pc = location;
      cycles += 4;
    }
  } else if constexpr (opcode == 0xC4 || opcode == 0xCC || // CALL
                       opcode == 0xD4 || opcode == 0xDC ||
                       opcode == 0xCD) {
    half addr = bus->NextHalf(regs.pc, regs.pc);
    if (Cond(opcode))
    {
      Push(regs.pc);
      regs.pc = addr;
      cycles += 12;
    }
  } else if constexpr (opcode == 0xC7 || opcode == 0xCF || opcode == 0xD7 || opcode == 0xDF ||
                       opcode == 0xE7 || opcode == 0xEF || opcode == 0xF7 || opcode == 0xFF) {
    Push(regs.pc);
    regs.pc = opcode & 0x38;
  }
}

void Cpu::load8()
{
  if constexpr ((opcode >= 0x40 && opcode <= 0x75) || (opcode >= 0x77 && opcode <= 0x7F)) {
    WriteR8((opcode >> 3) & 7, ReadR8(opcode & 7));
  } else if constexpr (opcode == 0x06 || opcode == 0x16 || opcode == 0x26 || opcode == 0x36 ||
                       opcode == 0x0e || opcode == 0x1e || opcode == 0x2e || opcode == 0x3e) {
    WriteR8((opcode >> 3) & 7, bus->NextByte(regs.pc, regs.pc));
  } else if constexpr (opcode == 0xe0) {
    bus->WriteByte(0xff00 + bus->NextByte(regs.pc, regs.pc), regs.a);
  } else if constexpr (opcode == 0xf0) {
    regs.a = bus->ReadByte(0xff00 + bus->NextByte(regs.pc, regs.pc));
  } else if constexpr (opcode == 0xe2) {
    bus->WriteByte(0xff00 + regs.c, regs.a);
  } else if constexpr (opcode == 0xf2) {
    regs.a = bus->ReadByte(0xff00 + regs.c);
  } else if constexpr (opcode == 0x02 || opcode == 0x12 || opcode == 0x22 || opcode == 0x32) {
    bus->WriteByte(ReadR16<2>((opcode >> 4) & 3), regs.a);
    if (opcode == 0x22)
      regs.hl++;
    if (opcode == 0x32)
      regs.hl--;
  } else if constexpr (opcode == 0x0a || opcode == 0x1a || opcode == 0x2a || opcode == 0x3a) {
    regs.a = bus->ReadByte(ReadR16<2>((opcode >> 4) & 3));
    if (opcode == 0x2a)
      regs.hl++;
    if (opcode == 0x3a)
      regs.hl--;
  } else if constexpr (opcode == 0xea) {
    bus->WriteByte(bus->NextHalf(regs.pc, regs.pc), regs.a);
  } else if constexpr (opcode == 0xfa) {
    regs.a = bus->ReadByte(bus->NextHalf(regs.pc, regs.pc));
  }
}

void Cpu::load16()
{
  if constexpr (opcode == 0x01 || opcode == 0x11 || opcode == 0x21 || opcode == 0x31) {
    WriteR16<1>((opcode >> 4) & 3, bus->NextHalf(regs.pc, regs.pc));
  } else if constexpr (opcode == 0x08) {
    bus->WriteHalf(bus->NextHalf(regs.pc, regs.pc), regs.sp);
  } else if constexpr (opcode == 0xf9) {
    regs.sp = regs.hl;
  } else if constexpr (opcode == 0xc1 || opcode == 0xd1 || opcode == 0xe1 || opcode == 0xf1) {
    WriteR16<3>((opcode >> 4) & 3, Pop());
  } else if constexpr (opcode == 0xc5 || opcode == 0xd5 || opcode == 0xe5 || opcode == 0xf5) {
    Push(ReadR16<3>((opcode >> 4) & 3));
  }
}

void Cpu::alu8()
{
  if constexpr (opcode == 0x04 || opcode == 0x14 || opcode == 0x24 || opcode == 0x34 ||
                opcode == 0x0c || opcode == 0x1c || opcode == 0x2c || opcode == 0x3c) {
    byte val = ReadR8((opcode >> 3) & 7);
    byte result = val + 1;
    bool z = (result == 0);
    bool n = false;
    bool h = ((val & 0xf) == 0xf);
    UpdateF(z, n, h, (regs.f >> 4) & 1);
    WriteR8((opcode >> 3) & 7, result);
  } else if constexpr (opcode == 0x05 || opcode == 0x15 || opcode == 0x25 || opcode == 0x35 ||
                       opcode == 0x0d || opcode == 0x1d || opcode == 0x2d || opcode == 0x3d) {
    byte val = ReadR8((opcode >> 3) & 7);
    byte result = val - 1;
    bool z = (result == 0);
    bool n = true;
    bool h = ((val & 0xf) == 0);
    UpdateF(z, n, h, (regs.f >> 4) & 1);
    WriteR8((opcode >> 3) & 7, result);
  } else if constexpr (opcode == 0x27) {
    byte offset = 0;

    bool z = (regs.f >> 7) & 1;
    bool n = (regs.f >> 6) & 1;
    bool h = (regs.f >> 5) & 1;
    bool c = (regs.f >> 4) & 1;

    if (h || (!n && ((regs.a & 0xf) > 9)))
    {
      offset |= 0x6;
    }

    if (c || (!n && (regs.a > 0x99)))
    {
      offset |= 0x60;
      c = true;
    }

    regs.a += (n) ? -offset : offset;
    z = (regs.a == 0);
    h = false;
    UpdateF(z, n, h, c);
  } else if constexpr (opcode == 0x2F) {
    regs.a = ~regs.a;
    bool n = true;
    bool h = true;
    UpdateF((regs.f >> 7) & 1, n, h, (regs.f >> 4) & 1);
  } else if constexpr (opcode == 0x37) {
    bool n = false;
    bool h = false;
    bool c = true;
    UpdateF((regs.f >> 7) & 1, n, h, c);
  } else if constexpr (opcode == 0x3f) {
    bool n = false;
    bool h = false;
    bool c = !((regs.f >> 4) & 1);
    UpdateF((regs.f >> 7) & 1, n, h, c);
  } else if constexpr ((opcode >= 0x80 && opcode <= 0x87) || opcode == 0xC6) {
    byte addend = opcode == 0xC6 ? bus->NextByte(regs.pc, regs.pc) : ReadR8(opcode & 7);
    byte result = regs.a + addend;
    bool z = (result == 0);
    bool n = false;
    bool h = (addend & 0xf) + (regs.a & 0xf) > 0xf;
    bool c = result < regs.a;
    UpdateF(z, n, h, c);
    regs.a = result;
  } else if constexpr ((opcode >= 0x88 && opcode <= 0x8F) || opcode == 0xCE) {
    byte addend = opcode == 0xCE ? bus->NextByte(regs.pc, regs.pc) : ReadR8(opcode & 7);
    bool c = (regs.f >> 4) & 1;
    half result = (half)(regs.a + addend + c);
    bool z = ((result & 0xff) == 0);
    bool n = false;
    bool h = (c) ? (regs.a & 0xf) + (addend & 0xf) >= 0xf : (regs.a & 0xf) + (addend & 0xf) > 0xf;
    c = bit<half, 8>(result);
    UpdateF(z, n, h, c);
    regs.a = (result & 0xff);
  } else if constexpr ((opcode >= 0x90 && opcode <= 0x97) || opcode == 0xD6) {
    byte reg = opcode == 0xD6 ? bus->NextByte(regs.pc, regs.pc) : ReadR8(opcode & 7);
    byte result = regs.a - reg;
    bool z = (result == 0);
    bool n = true;
    bool h = (reg & 0xf) > (regs.a & 0xf);
    bool c = result > regs.a;
    UpdateF(z, n, h, c);
    regs.a = result;
  } else if constexpr ((opcode >= 0x98 && opcode <= 0x9F) || opcode == 0xDE) {
    byte reg = opcode == 0xDE ? bus->NextByte(regs.pc, regs.pc) : ReadR8(opcode & 7);
    bool c = (regs.f >> 4) & 1;
    half result = (half)(regs.a - reg - c);
    bool z = ((result & 0xff) == 0);
    bool n = true;
    bool h = (!c) ? (regs.a & 0xf) < (reg & 0xf) : (regs.a & 0xf) < (reg & 0xf) + c;
    c = bit<half, 8>(result);
    UpdateF(z, n, h, c);
    regs.a = (result & 0xff);
  } else if constexpr ((opcode >= 0xa0 && opcode <= 0xa7) || opcode == 0xE6) {  // AND r8
    byte reg = opcode == 0xE6 ? bus->NextByte(regs.pc, regs.pc) : ReadR8(opcode & 7);
    regs.a &= reg;
    bool z = (regs.a == 0);
    bool n = false;
    bool h = true;
    bool c = false;
    UpdateF(z, n, h, c);
  } else if constexpr ((opcode >= 0xA8 && opcode <= 0xAF) || opcode == 0xEE) {  // XOR r8
    regs.a ^= opcode == 0xEE ? bus->NextByte(regs.pc, regs.pc) : ReadR8(opcode & 7);
    bool z = (regs.a == 0);
    bool n = false;
    bool h = false;
    bool c = false;
    UpdateF(z, n, h, c);
  } else if constexpr ((opcode >= 0xb0 && opcode <= 0xb7) || opcode == 0xF6) {  // OR r8
    byte reg = opcode == 0xF6 ? bus->NextByte(regs.pc, regs.pc) : ReadR8(opcode & 7);
    regs.a |= reg;
    bool z = (regs.a == 0);
    bool n = false;
    bool h = false;
    bool c = false;
    UpdateF(z, n, h, c);
  } else if constexpr ((opcode >= 0xb8 && opcode <= 0xbf) || opcode == 0xFE) {  // CP r8
    byte reg = opcode == 0xFE ? bus->NextByte(regs.pc, regs.pc) : ReadR8(opcode & 7);
    byte result = regs.a - reg;
    bool z = (result == 0);
    bool n = true;
    bool h = (reg & 0xf) > (regs.a & 0xf);
    bool c = result > regs.a;
    UpdateF(z, n, h, c);
  }
}

void Cpu::alu16()
{
  if constexpr (opcode == 0xE8) {
    byte offset = bus->NextByte(regs.pc, regs.pc);
    bool z = false;
    bool n = false;
    bool h = (regs.sp & 0xf) + (offset & 0xf) > 0xf;
    bool c = bit<half, 8>((regs.sp & 0xff) + offset);
    UpdateF(z, n, h, c);
    regs.sp += (sbyte)offset;
  } else if constexpr (opcode == 0xF8) {
    byte offset = bus->NextByte(regs.pc, regs.pc);
    bool z = false;
    bool n = false;
    bool h = (regs.sp & 0xf) + (offset & 0xf) > 0xf;
    bool c = bit<half, 8>((regs.sp & 0xff) + offset);
    UpdateF(z, n, h, c);
    regs.hl = regs.sp + (sbyte)offset;
  } else if constexpr (opcode == 0x03 || opcode == 0x13 || opcode == 0x23 || opcode == 0x33) {
    half reg = ReadR16<1>((opcode >> 4) & 3);
    reg++;
    WriteR16<1>((opcode >> 4) & 3, reg);
  } else if constexpr (opcode == 0x0b || opcode == 0x1b || opcode == 0x2b || opcode == 0x3b) {
    half reg = ReadR16<1>((opcode >> 4) & 3);
    reg--;
    WriteR16<1>((opcode >> 4) & 3, reg);
  } else if constexpr (opcode == 0x09 || opcode == 0x19 || opcode == 0x29 || opcode == 0x39) {
    half reg = ReadR16<1>((opcode >> 4) & 3);
    bool n = false;
    bool h = (regs.hl & 0xfff) + (reg & 0xfff) > 0xfff;
    bool c = bit<word, 16>(regs.hl + reg);
    UpdateF((regs.f >> 7) & 1, n, h, c);
    regs.hl += reg;
  }
}

void Cpu::bit8()
{
  if constexpr (opcode == 0x07) {
    byte old_a = regs.a;
    regs.a = (regs.a << 1) | bit<byte, 7>(old_a);
    bool z = false;
    bool n = false;
    bool h = false;
    bool c = bit<byte, 7>(old_a);
    UpdateF(z, n, h, c);
  } else if constexpr (opcode == 0x0F) {
    byte old_a = regs.a;
    regs.a >>= 1;
    setbit<byte, 7>(regs.a, old_a & 1);
    bool z = false;
    bool n = false;
    bool h = false;
    bool c = old_a & 1;
    UpdateF(z, n, h, c);
  } else if constexpr (opcode == 0x17) {
    byte old_a = regs.a;
    bool c = (regs.f >> 4) & 1;
    regs.a = (regs.a << 1) | c;
    bool z = false;
    bool n = false;
    bool h = false;
    c = bit<byte, 7>(old_a);
    UpdateF(z, n, h, c);
  } else if constexpr (opcode == 0x1F) {
    byte old_a = regs.a;
    regs.a >>= 1;
    bool c = (regs.f >> 4) & 1;
    setbit<byte, 7>(regs.a, c);
    bool z = false;
    bool n = false;
    bool h = false;
    c = old_a & 1;
    UpdateF(z, n, h, c);
  }
}

void Cpu::cbops()
{
  if constexpr (cb_opcode >= 0x40 && cb_opcode <= 0x7f) {
    bool z = !bit<byte>(ReadR8(cb_opcode & 7), (cb_opcode >> 3) & 7);
    bool n = false;
    bool h = true;
    UpdateF(z, n, h, (regs.f >> 4) & 1);
  } else if constexpr (cb_opcode >= 0x80 && cb_opcode <= 0xbf) {
    byte reg = ReadR8(cb_opcode & 7);
    byte pos = (cb_opcode >> 3) & 7;
    reg &= ~(1 << pos);
    WriteR8(cb_opcode & 7, reg);
  } else if constexpr (cb_opcode >= 0xc0 && cb_opcode <= 0xff) {
    byte reg = ReadR8(cb_opcode & 7);
    byte pos = (cb_opcode >> 3) & 7;
    reg |= (1 << pos);
    WriteR8(cb_opcode & 7, reg);
  } else if constexpr (cb_opcode >= 0 && cb_opcode <= 0x07) { 
    byte reg = ReadR8(cb_opcode & 7);
    byte old_reg = reg;
    reg = (reg << 1) | bit<byte, 7>(old_reg);
    bool z = (reg == 0);
    bool n = false;
    bool h = false;
    bool c = bit<byte, 7>(old_reg);
    UpdateF(z, n, h, c);
    WriteR8(cb_opcode & 7, reg);
  } else if constexpr (cb_opcode >= 0x08 && cb_opcode <= 0x0f) {
    byte reg = ReadR8(cb_opcode & 7);
    byte old_reg = reg;
    reg >>= 1;
    setbit<byte, 7>(reg, old_reg & 1);
    bool z = (reg == 0);
    bool n = false;
    bool h = false;
    bool c = old_reg & 1;
    UpdateF(z, n, h, c);
    WriteR8(cb_opcode & 7, reg);
  } else if constexpr (cb_opcode >= 0x10 && cb_opcode <= 0x17) {  // RL r8
    byte reg = ReadR8(cb_opcode & 7);
    byte old_reg = reg;
    bool c = (regs.f >> 4) & 1;
    reg = (reg << 1) | c;
    bool z = (reg == 0);
    bool n = false;
    bool h = false;
    c = bit<byte, 7>(old_reg);
    UpdateF(z, n, h, c);
    WriteR8(cb_opcode & 7, reg);
  } else if constexpr (cb_opcode >= 0x18 && cb_opcode <= 0x1f) {  // RR r8
    byte reg = ReadR8(cb_opcode & 7);
    byte old_reg = reg;
    reg >>= 1;
    bool c = (regs.f >> 4) & 1;
    setbit<byte, 7>(reg, c);
    bool z = (reg == 0);
    bool n = false;
    bool h = false;
    c = old_reg & 1;
    UpdateF(z, n, h, c);
    WriteR8(cb_opcode & 7, reg);
  } else if constexpr (cb_opcode >= 0x20 && cb_opcode <= 0x27) {  // SLA r8
    byte reg = ReadR8(cb_opcode & 7);
    bool c = bit<byte, 7>(reg);
    reg <<= 1;
    bool z = (reg == 0);
    bool n = false;
    bool h = false;
    UpdateF(z, n, h, c);
    WriteR8(cb_opcode & 7, reg);
  } else if constexpr (cb_opcode >= 0x28 && cb_opcode <= 0x2f) {  // SRA r8
    byte reg = ReadR8(cb_opcode & 7);
    byte old_reg = reg;
    reg >>= 1;
    setbit<byte, 7>(reg, bit<byte, 7>(old_reg));
    bool z = (reg == 0);
    bool n = false;
    bool h = false;
    bool c = old_reg & 1;
    UpdateF(z, n, h, c);
    WriteR8(cb_opcode & 7, reg);
  } else if constexpr (cb_opcode >= 0x30 && cb_opcode <= 0x37) {  // SWAP r8
    byte reg = ReadR8(cb_opcode & 7);
    reg = (reg << 4) | (reg >> 4);
    bool z = (reg == 0);
    bool n = false;
    bool h = false;
    bool c = false;
    UpdateF(z, n, h, c);
    WriteR8(cb_opcode & 7, reg);
  } else if constexpr (cb_opcode >= 0x38 && cb_opcode <= 0x3f) {  // SRL r8
    byte reg = ReadR8(cb_opcode & 7);
    bool c = reg & 1;
    reg >>= 1;
    bool z = (reg == 0);
    bool n = false;
    bool h = false;
    UpdateF(z, n, h, c);
    WriteR8(cb_opcode & 7, reg);
  }
}

} // natsukashii::core