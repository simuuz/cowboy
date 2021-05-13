#include "cpu.h"

Cpu::Cpu(bool skip) : bus(skip), skip(skip)
{
  //log = fopen("log.txt", "w");
  halt = false;
  ime = false;
  total_cycles = 0;
  cycles = 0;
  tima_cycles = 0;
  div_cycles = 0;

  if (skip)
  {
    regs.af = 0x1b0;
    regs.bc = 0x13;
    regs.de = 0xd8;
    regs.hl = 0x14d;
    regs.sp = 0xfffe;
    regs.pc = 0x100;
  }
  else
  {
    regs.af = 0;
    regs.bc = 0;
    regs.de = 0;
    regs.hl = 0;
    regs.sp = 0;
    regs.pc = 0;
  }
}

void Cpu::reset()
{
  ime = false;
  halt = false;

  if (skip)
  {
    regs.af = 0x1b0;
    regs.bc = 0x13;
    regs.de = 0xd8;
    regs.hl = 0x14d;
    regs.sp = 0xfffe;
    regs.pc = 0x100;
  }
  else
  {
    regs.af = 0;
    regs.bc = 0;
    regs.de = 0;
    regs.hl = 0;
    regs.sp = 0;
    regs.pc = 0;
  }
}

void Cpu::step()
{
  handle_interrupts();

  //fprintf(log,
  //        "A: %02X F: %02X B: %02X C: %02X D: %02X E: %02X H: %02X L: %02X SP: %04X PC: 00:%04X "
  //        "(%02X %02X %02X %02X)\n",
  //        regs.a, regs.f, regs.b, regs.c, regs.d, regs.e, regs.h, regs.l, regs.sp, regs.pc,
  //        bus.read_byte(regs.pc), bus.read_byte(regs.pc + 1), bus.read_byte(regs.pc + 2),
  //        bus.read_byte(regs.pc + 3));

  if (!halt)
  {
    byte opcode = bus.next_byte(regs.pc, regs.pc);
    cycles = opcycles[opcode];
    execute(opcode);
  }
  else
  {
    cycles = 4;
  }

  total_cycles += cycles;
}

void Cpu::execute(byte opcode)
{
  switch (opcode)
  {
  case 0: case 0x10:
    break;  // NOP
  case 0x01:
  case 0x11:
  case 0x21:
  case 0x31:  // LD r16, half
    write_r16<1>((opcode >> 4) & 3, bus.next_half(regs.pc, regs.pc));
    break;
  case 0x40 ... 0x75:
  case 0x77 ... 0x7f:  // LD r8, r8
    write_r8((opcode >> 3) & 7, read_r8(opcode & 7));
    break;
  case 0x06:
  case 0x16:
  case 0x26:
  case 0x36:
  case 0x0e:
  case 0x1e:
  case 0x2e:
  case 0x3e:  // LD r8, byte
    write_r8((opcode >> 3) & 7, bus.next_byte(regs.pc, regs.pc));
    break;
  case 0x04:
  case 0x14:
  case 0x24:
  case 0x34:
  case 0x0c:
  case 0x1c:
  case 0x2c:
  case 0x3c:
  {  // INC r8
    byte val = read_r8((opcode >> 3) & 7);
    byte result = val + 1;
    bool z = (result == 0);
    bool n = false;
    bool h = ((val & 0xf) == 0xf);
    update_f(z, n, h, (regs.f >> 4) & 1);
    write_r8((opcode >> 3) & 7, result);
  }
  break;
  case 0x05:
  case 0x15:
  case 0x25:
  case 0x35:
  case 0x0d:
  case 0x1d:
  case 0x2d:
  case 0x3d:
  {  // DEC r8
    byte val = read_r8((opcode >> 3) & 7);
    byte result = val - 1;
    bool z = (result == 0);
    bool n = true;
    bool h = ((val & 0xf) == 0);
    update_f(z, n, h, (regs.f >> 4) & 1);
    write_r8((opcode >> 3) & 7, result);
  }
  break;
  case 0x27:
  {  // DAA
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
    update_f(z, n, h, c);
  }
  break;
  case 0xf3:
    ime = false;
    break;  // DI
  case 0xfb:
    ime = true;
    break;  // EI
  case 0x03:
  case 0x13:
  case 0x23:
  case 0x33:
  {  // INC r16
    half reg = read_r16<1>((opcode >> 4) & 3);
    reg++;
    write_r16<1>((opcode >> 4) & 3, reg);
  }
  break;
  case 0x0b:
  case 0x1b:
  case 0x2b:
  case 0x3b:
  {  // DEC r16
    half reg = read_r16<1>((opcode >> 4) & 3);
    reg--;
    write_r16<1>((opcode >> 4) & 3, reg);
  }
  break;
  case 0xa8 ... 0xaf:
  {  // XOR r8
    regs.a ^= read_r8(opcode & 7);
    bool z = (regs.a == 0);
    bool n = false;
    bool h = false;
    bool c = false;
    update_f(z, n, h, c);
  }
  break;
  case 0xee:
  {  // XOR byte
    regs.a ^= bus.next_byte(regs.pc, regs.pc);
    bool z = (regs.a == 0);
    bool n = false;
    bool h = false;
    bool c = false;
    update_f(z, n, h, c);
  }
  break;
  case 0xe0:  // LD (FF00 + byte), A
    bus.write_byte(0xff00 + bus.next_byte(regs.pc, regs.pc), regs.a);
    break;
  case 0xf0:  // LD A, (FF00 + byte)
    regs.a = bus.read_byte(0xff00 + bus.next_byte(regs.pc, regs.pc));
    break;
  case 0xe2:  // LD (FF00 + C), A
    bus.write_byte(0xff00 + regs.c, regs.a);
    break;
  case 0xf2:  // LD A, (FF00 + C)
    regs.a = bus.read_byte(0xff00 + regs.c);
    break;
  case 0x02:
  case 0x12:
  case 0x22:
  case 0x32:  // LD (R16), A
    bus.write_byte(read_r16<2>((opcode >> 4) & 3), regs.a);
    if (opcode == 0x22)
      regs.hl++;
    if (opcode == 0x32)
      regs.hl--;
    break;
  case 0x0a:
  case 0x1a:
  case 0x2a:
  case 0x3a:  // LD A, (R16)
    regs.a = bus.read_byte(read_r16<2>((opcode >> 4) & 3));
    if (opcode == 0x2a)
      regs.hl++;
    if (opcode == 0x3a)
      regs.hl--;
    break;
  case 0x08:  // LD (half), REGS.SP
    bus.write_half(bus.next_half(regs.pc, regs.pc), regs.sp);
    break;
  case 0xf9:  // LD REGS.SP, HL
    regs.sp = regs.hl;
    break;
  case 0xe8:
  {  // ADD REGS.SP, sbyte
    byte offset = bus.next_byte(regs.pc, regs.pc);
    bool z = false;
    bool n = false;
    bool h = (regs.sp & 0xf) + (offset & 0xf) > 0xf;
    bool c = bit<half, 8>((regs.sp & 0xff) + offset);
    update_f(z, n, h, c);
    regs.sp += (sbyte)offset;
  }
  break;
  case 0xf8:
  {  // LD HL, REGS.SP+sbyte
    byte offset = bus.next_byte(regs.pc, regs.pc);
    bool z = false;
    bool n = false;
    bool h = (regs.sp & 0xf) + (offset & 0xf) > 0xf;
    bool c = bit<half, 8>((regs.sp & 0xff) + offset);
    update_f(z, n, h, c);
    regs.hl = regs.sp + (sbyte)offset;
  }
  break;
  case 0x17:
  {  // RLA
    byte old_a = regs.a;
    bool c = (regs.f >> 4) & 1;
    regs.a = (regs.a << 1) | c;
    bool z = false;
    bool n = false;
    bool h = false;
    c = bit<byte, 7>(old_a);
    update_f(z, n, h, c);
  }
  break;
  case 0x07:
  {  // RLCA
    byte old_a = regs.a;
    regs.a = (regs.a << 1) | bit<byte, 7>(old_a);
    bool z = false;
    bool n = false;
    bool h = false;
    bool c = bit<byte, 7>(old_a);
    update_f(z, n, h, c);
  }
  break;
  case 0x1f:
  {  // RRA
    byte old_a = regs.a;
    regs.a >>= 1;
    bool c = (regs.f >> 4) & 1;
    setbit<byte, 7>(regs.a, c);
    bool z = false;
    bool n = false;
    bool h = false;
    c = old_a & 1;
    update_f(z, n, h, c);
  }
  break;
  case 0x0f:
  {  // RRCA
    byte old_a = regs.a;
    regs.a >>= 1;
    setbit<byte, 7>(regs.a, old_a & 1);
    bool z = false;
    bool n = false;
    bool h = false;
    bool c = old_a & 1;
    update_f(z, n, h, c);
  }
  break;
  case 0x90 ... 0x97:
  {  // SUB r8
    byte reg = read_r8(opcode & 7);
    byte result = regs.a - reg;
    bool z = (result == 0);
    bool n = true;
    bool h = (reg & 0xf) > (regs.a & 0xf);
    bool c = result > regs.a;
    update_f(z, n, h, c);
    regs.a = result;
  }
  break;
  case 0xd6:
  {  // SUB byte
    byte op2 = bus.next_byte(regs.pc, regs.pc);
    byte result = regs.a - op2;
    bool z = (result == 0);
    bool n = true;
    bool h = (op2 & 0xf) > (regs.a & 0xf);
    bool c = result > regs.a;
    update_f(z, n, h, c);
    regs.a = result;
  }
  break;
  case 0x80 ... 0x87:
  {  // ADD r8
    byte reg = read_r8(opcode & 7);
    byte result = regs.a + reg;
    bool z = (result == 0);
    bool n = false;
    bool h = (reg & 0xf) + (regs.a & 0xf) > 0xf;
    bool c = result < regs.a;
    update_f(z, n, h, c);
    regs.a = result;
  }
  break;
  case 0xc6:
  {  // ADD byte
    byte op2 = bus.next_byte(regs.pc, regs.pc);
    byte result = regs.a + op2;
    bool z = (result == 0);
    bool n = false;
    bool h = (op2 & 0xf) + (regs.a & 0xf) > 0xf;
    bool c = result < regs.a;
    update_f(z, n, h, c);
    regs.a = result;
  }
  break;
  case 0x88 ... 0x8f:
  {  // ADC r8
    byte reg = read_r8(opcode & 7);
    bool c = (regs.f >> 4) & 1;
    half result = (half)(regs.a + reg + c);
    bool z = ((result & 0xff) == 0);
    bool n = false;
    bool h = (c) ? (regs.a & 0xf) + (reg & 0xf) >= 0xf : (regs.a & 0xf) + (reg & 0xf) > 0xf;
    c = bit<half, 8>(result);
    update_f(z, n, h, c);
    regs.a = (result & 0xff);
  }
  break;
  case 0xce:
  {  // ADC byte
    byte op2 = bus.next_byte(regs.pc, regs.pc);
    bool c = (regs.f >> 4) & 1;
    half result = (half)(regs.a + op2 + c);
    bool z = ((result & 0xff) == 0);
    bool n = false;
    bool h = (c) ? (regs.a & 0xf) + (op2 & 0xf) >= 0xf : (regs.a & 0xf) + (op2 & 0xf) > 0xf;
    c = bit<half, 8>(result);
    update_f(z, n, h, c);
    regs.a = (result & 0xff);
  }
  break;
  case 0x98 ... 0x9f:
  {  // SBC r8
    byte reg = read_r8(opcode & 7);
    bool c = (regs.f >> 4) & 1;
    half result = (half)(regs.a - reg - c);
    bool z = ((result & 0xff) == 0);
    bool n = true;
    bool h = (!c) ? (regs.a & 0xf) < (reg & 0xf) : (regs.a & 0xf) < (reg & 0xf) + c;
    c = bit<half, 8>(result);
    update_f(z, n, h, c);
    regs.a = (result & 0xff);
  }
  break;
  case 0xde:
  {  // SBC byte
    byte op2 = bus.next_byte(regs.pc, regs.pc);
    bool c = (regs.f >> 4) & 1;
    half result = (half)(regs.a - op2 - c);
    bool z = ((result & 0xff) == 0);
    bool n = true;
    bool h = (!c) ? (regs.a & 0xf) < (op2 & 0xf) : (regs.a & 0xf) < (op2 & 0xf) + c;
    c = bit<half, 8>(result);
    update_f(z, n, h, c);
    regs.a = (result & 0xff);
  }
  break;
  case 0xa0 ... 0xa7:
  {  // AND r8
    byte reg = read_r8(opcode & 7);
    regs.a &= reg;
    bool z = (regs.a == 0);
    bool n = false;
    bool h = true;
    bool c = false;
    update_f(z, n, h, c);
  }
  break;
  case 0xe6:
  {  // AND byte
    byte op2 = bus.next_byte(regs.pc, regs.pc);
    regs.a &= op2;
    bool z = (regs.a == 0);
    bool n = false;
    bool h = true;
    bool c = false;
    update_f(z, n, h, c);
  }
  break;
  case 0xb0 ... 0xb7:
  {  // OR r8
    byte reg = read_r8(opcode & 7);
    regs.a |= reg;
    bool z = (regs.a == 0);
    bool n = false;
    bool h = false;
    bool c = false;
    update_f(z, n, h, c);
  }
  break;
  case 0xf6:
  {  // OR byte
    byte op2 = bus.next_byte(regs.pc, regs.pc);
    regs.a |= op2;
    bool z = (regs.a == 0);
    bool n = false;
    bool h = false;
    bool c = false;
    update_f(z, n, h, c);
  }
  break;
  case 0xb8 ... 0xbf:
  {  // CP r8
    byte reg = read_r8(opcode & 7);
    byte result = regs.a - reg;
    bool z = (result == 0);
    bool n = true;
    bool h = (reg & 0xf) > (regs.a & 0xf);
    bool c = result > regs.a;
    update_f(z, n, h, c);
  }
  break;
  case 0xfe:
  {  // CP byte
    byte op2 = bus.next_byte(regs.pc, regs.pc);
    byte result = regs.a - op2;
    bool z = (result == 0);
    bool n = true;
    bool h = (op2 & 0xf) > (regs.a & 0xf);
    bool c = result > regs.a;
    update_f(z, n, h, c);
  }
  break;
  case 0xd9:  // RETI
    regs.pc = pop();
    ime = true;
    break;
  case 0xc0:
  case 0xd0:
  case 0xc8:
  case 0xd8:
  case 0xc9:  // RET cond
    if (cond(opcode))
    {
      regs.pc = pop();
      cycles += 12;
    }
    break;
  case 0xc7:
  case 0xd7:
  case 0xe7:
  case 0xf7:
  case 0xcf:
  case 0xdf:
  case 0xef:
  case 0xff:  // RST vec
    push(regs.pc);
    regs.pc = opcode & 0x38;
    break;
  case 0x2f:
  {  // CPL
    regs.a = ~regs.a;
    bool n = true;
    bool h = true;
    update_f((regs.f >> 7) & 1, n, h, (regs.f >> 4) & 1);
  }
  break;
  case 0x37:
  {  // SCF
    bool n = false;
    bool h = false;
    bool c = true;
    update_f((regs.f >> 7) & 1, n, h, c);
  }
  break;
  case 0x3f:
  {  // CCF
    bool n = false;
    bool h = false;
    bool c = !((regs.f >> 4) & 1);
    update_f((regs.f >> 7) & 1, n, h, c);
  }
  break;
  case 0x76:  // HALT
    halt = true;
    break;
  case 0xc4:
  case 0xd4:
  case 0xcc:
  case 0xdc:
  case 0xcd:
  {  // CALL cond half
    half addr = bus.next_half(regs.pc, regs.pc);
    if (cond(opcode))
    {
      push(regs.pc);
      regs.pc = addr;
      cycles += 12;
    }
  }
  break;
  case 0x09:
  case 0x19:
  case 0x29:
  case 0x39:
  {  // ADD HL, r16
    half reg = read_r16<1>((opcode >> 4) & 3);
    bool n = false;
    bool h = (regs.hl & 0xfff) + (reg & 0xfff) > 0xfff;
    bool c = bit<word, 16>(regs.hl + reg);
    update_f((regs.f >> 7) & 1, n, h, c);
    regs.hl += reg;
  }
  break;
  case 0xea:  // LD (half), A
    bus.write_byte(bus.next_half(regs.pc, regs.pc), regs.a);
    break;
  case 0xfa:  // LD A, (half)
    regs.a = bus.read_byte(bus.next_half(regs.pc, regs.pc));
    break;
  case 0xc2:
  case 0xd2:
  case 0xca:
  case 0xda:
  case 0xc3:
  {  // JP cond half
    half addr = bus.next_half(regs.pc, regs.pc);
    if (cond(opcode))
    {
      regs.pc = addr;
      cycles += 4;
    }
  }
  break;
  case 0xe9:  // JP HL
    regs.pc = regs.hl;
    break;
  case 0x18:  // JR sbyte
    regs.pc += (sbyte)bus.next_byte(regs.pc, regs.pc);
    break;
  case 0x20:
  case 0x30:
  case 0x28:
  case 0x38:
  {  // JR cond sbyte
    sbyte offset = (sbyte)bus.next_byte(regs.pc, regs.pc);
    if (cond(opcode))
    {
      regs.pc += offset;
      cycles += 4;
    }
  }
  break;
  case 0xcb:
  {
    byte cbop = bus.next_byte(regs.pc, regs.pc);
    cycles = cbopcycles[cbop];
    switch (cbop)
    {
    case 0x40 ... 0x7f:
    {  // BIT pos, r8
      bool z = !bit<byte>(read_r8(cbop & 7), (cbop >> 3) & 7);
      bool n = false;
      bool h = true;
      update_f(z, n, h, (regs.f >> 4) & 1);
    }
    break;
    case 0x80 ... 0xbf:
    {  // RES pos, r8
      byte reg = read_r8(cbop & 7);
      byte pos = (cbop >> 3) & 7;
      reg &= ~(1 << pos);
      write_r8(cbop & 7, reg);
    }
    break;
    case 0xc0 ... 0xff:
    {  // SET pos, r8
      byte reg = read_r8(cbop & 7);
      byte pos = (cbop >> 3) & 7;
      reg |= (1 << pos);
      write_r8(cbop & 7, reg);
    }
    break;
    case 0 ... 0x07:
    {  // RLC r8
      byte reg = read_r8(cbop & 7);
      byte old_reg = reg;
      reg = (reg << 1) | bit<byte, 7>(old_reg);
      bool z = (reg == 0);
      bool n = false;
      bool h = false;
      bool c = bit<byte, 7>(old_reg);
      update_f(z, n, h, c);
      write_r8(cbop & 7, reg);
    }
    break;
    case 0x08 ... 0x0f:
    {  // RRC r8
      byte reg = read_r8(cbop & 7);
      byte old_reg = reg;
      reg >>= 1;
      setbit<byte, 7>(reg, old_reg & 1);
      bool z = (reg == 0);
      bool n = false;
      bool h = false;
      bool c = old_reg & 1;
      update_f(z, n, h, c);
      write_r8(cbop & 7, reg);
    }
    break;
    case 0x10 ... 0x17:
    {  // RL r8
      byte reg = read_r8(cbop & 7);
      byte old_reg = reg;
      bool c = (regs.f >> 4) & 1;
      reg = (reg << 1) | c;
      bool z = (reg == 0);
      bool n = false;
      bool h = false;
      c = bit<byte, 7>(old_reg);
      update_f(z, n, h, c);
      write_r8(cbop & 7, reg);
    }
    break;
    case 0x18 ... 0x1f:
    {  // RR r8
      byte reg = read_r8(cbop & 7);
      byte old_reg = reg;
      reg >>= 1;
      bool c = (regs.f >> 4) & 1;
      setbit<byte, 7>(reg, c);
      bool z = (reg == 0);
      bool n = false;
      bool h = false;
      c = old_reg & 1;
      update_f(z, n, h, c);
      write_r8(cbop & 7, reg);
    }
    break;
    case 0x20 ... 0x27:
    {  // SLA r8
      byte reg = read_r8(cbop & 7);
      bool c = bit<byte, 7>(reg);
      reg <<= 1;
      bool z = (reg == 0);
      bool n = false;
      bool h = false;
      update_f(z, n, h, c);
      write_r8(cbop & 7, reg);
    }
    break;
    case 0x28 ... 0x2f:
    {  // SRA r8
      byte reg = read_r8(cbop & 7);
      byte old_reg = reg;
      reg >>= 1;
      setbit<byte, 7>(reg, bit<byte, 7>(old_reg));
      bool z = (reg == 0);
      bool n = false;
      bool h = false;
      bool c = old_reg & 1;
      update_f(z, n, h, c);
      write_r8(cbop & 7, reg);
    }
    break;
    case 0x30 ... 0x37:
    {  // SWAP r8
      byte reg = read_r8(cbop & 7);
      reg = (reg << 4) | (reg >> 4);
      bool z = (reg == 0);
      bool n = false;
      bool h = false;
      bool c = false;
      update_f(z, n, h, c);
      write_r8(cbop & 7, reg);
    }
    break;
    case 0x38 ... 0x3f:
    {  // SRL r8
      byte reg = read_r8(cbop & 7);
      bool c = reg & 1;
      reg >>= 1;
      bool z = (reg == 0);
      bool n = false;
      bool h = false;
      update_f(z, n, h, c);
      write_r8(cbop & 7, reg);
    }
    break;
    default:
      printf("Unrecognized CB prefix opcode: %02x\n", cbop);
      exit(1);
    }
  }
  break;
  case 0xc1:
  case 0xd1:
  case 0xe1:
  case 0xf1:  // POP r16
    write_r16<3>((opcode >> 4) & 3, pop());
    break;
  case 0xc5:
  case 0xd5:
  case 0xe5:
  case 0xf5:  // PUSH r16
    push(read_r16<3>((opcode >> 4) & 3));
    break;
  default:
    printf("Unrecognized opcode: %02x\n", opcode);
    exit(1);
  }
}

void Cpu::update_f(bool z, bool n, bool h, bool c)
{
  regs.f = (z << 7) | (n << 6) | (h << 5) | (c << 4) | (0 << 3) | (0 << 2) | (0 << 1) | 0;
}

bool Cpu::cond(byte opcode)
{
  if (opcode & 1)
    return true;
  byte bits = (opcode >> 3) & 3;
  switch (bits)
  {
  case 0:
    return !((regs.f >> 7) & 1);
  case 1:
    return ((regs.f >> 7) & 1);
  case 2:
    return !((regs.f >> 4) & 1);
  case 3:
    return ((regs.f >> 4) & 1);
  }
}

half Cpu::pop()
{
  half val = bus.read_half(regs.sp);
  regs.sp += 2;
  return val;
}

void Cpu::push(half val)
{
  regs.sp -= 2;
  bus.write_half(regs.sp, val);
}

byte Cpu::read_r8(byte bits)
{
  switch (bits)
  {
  case 0:
    return regs.b;
  case 1:
    return regs.c;
  case 2:
    return regs.d;
  case 3:
    return regs.e;
  case 4:
    return regs.h;
  case 5:
    return regs.l;
  case 6:
    return bus.read_byte(regs.hl);
  case 7:
    return regs.a;
  }
}

void Cpu::write_r8(byte bits, byte val)
{
  switch (bits)
  {
  case 0:
    regs.b = val;
    break;
  case 1:
    regs.c = val;
    break;
  case 2:
    regs.d = val;
    break;
  case 3:
    regs.e = val;
    break;
  case 4:
    regs.h = val;
    break;
  case 5:
    regs.l = val;
    break;
  case 6:
    bus.write_byte(regs.hl, val);
    break;
  case 7:
    regs.a = val;
    break;
  }
}

template <int group>
half Cpu::read_r16(byte bits)
{
  if (group == 1)
  {
    switch (bits)
    {
    case 0:
      return regs.bc;
    case 1:
      return regs.de;
    case 2:
      return regs.hl;
    case 3:
      return regs.sp;
    }
  }
  else if (group == 2)
  {
    switch (bits)
    {
    case 0:
      return regs.bc;
    case 1:
      return regs.de;
    case 2:
    case 3:
      return regs.hl;
    }
  }
  else if (group == 3)
  {
    switch (bits)
    {
    case 0:
      return regs.bc;
    case 1:
      return regs.de;
    case 2:
      return regs.hl;
    case 3:
      return regs.af;
    }
  }
}

template <int group>
void Cpu::write_r16(byte bits, half value)
{
  if (group == 1)
  {
    switch (bits)
    {
    case 0:
      regs.bc = value;
      break;
    case 1:
      regs.de = value;
      break;
    case 2:
      regs.hl = value;
      break;
    case 3:
      regs.sp = value;
      break;
    }
  }
  else if (group == 2)
  {
    switch (bits)
    {
    case 0:
      regs.bc = value;
      break;
    case 1:
      regs.de = value;
      break;
    case 2:
    case 3:
      regs.hl = value;
      break;
    }
  }
  else if (group == 3)
  {
    switch (bits)
    {
    case 0:
      regs.bc = value;
      break;
    case 1:
      regs.de = value;
      break;
    case 2:
      regs.hl = value;
      break;
    case 3:
    {
      regs.a = (value >> 8) & 0xff;
      bool z = (value >> 7) & 1;
      bool n = (value >> 6) & 1;
      bool h = (value >> 5) & 1;
      bool c = (value >> 4) & 1;
      update_f(z, n, h, c);
    }
    break;
    }
  }
}

void Cpu::handle_interrupts()
{
  byte int_mask = bus.mem.ie & bus.mem.io.intf;

  if (int_mask)
  {
    halt = false;
    if (ime)
    {
      if ((bus.mem.ie & 1) && (bus.mem.io.intf & 1))
      {
        bus.mem.io.intf &= ~1;
        push(regs.pc);
        regs.pc = 0x40;
        ime = false;
        cycles += 20;
      }
      else if (bit<byte, 1>(bus.mem.ie) && bit<byte, 1>(bus.mem.io.intf))
      {
        bus.mem.io.intf &= ~2;
        push(regs.pc);
        regs.pc = 0x48;
        ime = false;
        cycles += 20;
      }
      else if (bit<byte, 2>(bus.mem.ie) && bit<byte, 2>(bus.mem.io.intf))
      {
        bus.mem.io.intf &= ~4;
        push(regs.pc);
        regs.pc = 0x50;
        ime = false;
        cycles += 20;
      }
      else if (bit<byte, 3>(bus.mem.ie) && bit<byte, 3>(bus.mem.io.intf))
      {
        bus.mem.io.intf &= ~8;
        push(regs.pc);
        regs.pc = 0x58;
        ime = false;
        cycles += 20;
      }
      else if (bit<byte, 4>(bus.mem.ie) && bit<byte, 4>(bus.mem.io.intf))
      {
        bus.mem.io.intf &= ~16;
        push(regs.pc);
        regs.pc = 0x60;
        ime = false;
        cycles += 20;
      }
    }
  }
}

void Cpu::handle_timers()
{
  constexpr int tima_vals[4] = {1024, 16, 64, 256};
  if ((bus.mem.io.tac >> 2) & 1)
  {
    int tima_val = tima_vals[bus.mem.io.tac & 3];
    tima_cycles += cycles;
    while (tima_cycles >= tima_val)
    {
      tima_cycles -= tima_val;
      if (bus.mem.io.tima == 0xff)
      {
        bus.mem.io.tima = bus.mem.io.tma;
        bus.mem.io.intf |= 0b100;
      }
      else
      {
        bus.mem.io.tima++;
      }
    }
  }

  div_cycles += cycles;
  if (div_cycles >= 256)
  {
    div_cycles -= 256;
    bus.mem.io.div++;
  }
}