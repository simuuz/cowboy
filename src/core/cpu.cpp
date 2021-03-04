#include "cpu.h"

Cpu::Cpu() {
    regs.af = 0; regs.bc = 0;
    regs.de = 0; regs.hl = 0;
    regs.sp = 0; regs.pc = 0;
}

void Cpu::reset() {
    regs.af = 0; regs.bc = 0;
    regs.de = 0; regs.hl = 0;
    regs.sp = 0; regs.pc = 0;
}

void Cpu::step() {
    u8 opcode = mem.read<u8>(regs.pc, regs.pc);

    switch(opcode) {
        case 0: break;                                //NOP
        case 0x01: case 0x11: case 0x21: case 0x31:   //LD r16, u16
        write_r16<1>((opcode >> 4) & 3, mem.read<u16>(regs.pc, regs.pc));
        break;
        case 0x40 ... 0x75: case 0x77 ... 0x7f:       //LD r8, r8
        write_r8((opcode >> 3) & 7, read_r8(opcode & 7));
        break;
        case 0x06: case 0x16: case 0x26: case 0x36:
        case 0x0e: case 0x1e: case 0x2e: case 0x3e:   //LD r8, u8
        write_r8((opcode >> 3) & 7, mem.read<u8>(regs.pc, regs.pc));
        break;
        case 0x04: case 0x14: case 0x24: case 0x34:
        case 0x0c: case 0x1c: case 0x2c: case 0x3c: { //INC r8
            u8 val = read_r8((opcode >> 3) & 7);
            u8 result = val + 1;
            flags.z = (result == 0);
            flags.n = false;
            flags.h = ((val & 0xf) == 0xf);
            flags.set(regs.f);
            write_r8((opcode >> 3) & 7, result);
        } break;
        case 0x05: case 0x15: case 0x25: case 0x35:
        case 0x0d: case 0x1d: case 0x2d: case 0x3d: { //DEC r8
            u8 val = read_r8((opcode >> 3) & 7);
            u8 result = val - 1;
            flags.z = (result == 0);
            flags.n = true;
            flags.h = ((val & 0xf) == 0);
            flags.set(regs.f);
            write_r8((opcode >> 3) & 7, result);
        } break;
        case 0x03: case 0x13: case 0x23: case 0x33: { //INC r16
            u16 reg = read_r16<1>((opcode >> 4) & 3);
            reg++;
            write_r16<1>((opcode >> 4) & 3, reg);
        } break;
        case 0x0b: case 0x1b: case 0x2b: case 0x3b: { //DEC r16
            u16 reg = read_r16<1>((opcode >> 4) & 3);
            reg--;
            write_r16<1>((opcode >> 4) & 3, reg);
        } break;
        case 0xa8 ... 0xaf: {                         //XOR r8
            regs.a ^= read_r8(opcode & 7);
            flags.z = (regs.a == 0);
            flags.n = false;
            flags.h = false;
            flags.c = false;
            flags.set(regs.f);
        } break;
        case 0xe0:                                    //LD (FF00 + u8), A
        mem.write<u8>(0xff00 + mem.read<u8>(regs.pc, regs.pc), regs.a);
        break;
        case 0xf0:                                    //LD A, (FF00 + u8)
        regs.a = mem.read<u8>(0xff00 + mem.read<u8>(regs.pc, regs.pc), regs.pc, false);
        break;
        case 0xe2:                                    //LD (FF00 + C), A
        mem.write<u8>(0xff00 + regs.c, regs.a);
        break;
        case 0xf2:                                    //LD A, (FF00 + C)
        regs.a = mem.read<u8>(0xff00 + regs.c, regs.pc, false);
        break;
        case 0x02: case 0x12:                         //LD (R16), A
        mem.write<u8>(read_r16<2>((opcode >> 4) & 3), regs.a);
        break;
        case 0x22:
        mem.write<u8>(regs.hl, regs.a);
        regs.hl++;
        break;
        case 0x32:
        mem.write<u8>(regs.hl, regs.a);
        regs.hl--;
        break;
        case 0x0a: case 0x1a:                         //LD A, (R16)
        regs.a = mem.read<u8>(read_r16<2>((opcode >> 4) & 3), regs.pc, false);
        break;
        case 0x2a:
        regs.a = mem.read<u8>(regs.hl, regs.pc, false);
        regs.hl++;
        break;
        case 0x3a:
        regs.a = mem.read<u8>(regs.hl, regs.pc, false);
        regs.hl--;
        break;
        case 0x17: {                                  //RLA
            u8 old_a = regs.a;
            regs.a = (regs.a << 1) | flags.c;
            flags.z = false;
            flags.n = false;
            flags.h = false;
            flags.c = bit(old_a, 7);
            flags.set(regs.f);
        } break;
        case 0xc9:                                    //RET
        regs.pc = pop();
        break;
        case 0xb8 ... 0xbf: {                         //CP r8
            u8 op2 = read_r8(opcode & 7);
            u8 result = regs.a - op2;
            flags.z = (result == 0);
            flags.n = true;
            flags.h = (op2 & 0xf) > (regs.a & 0xf);
            flags.c = op2 > regs.a;
            flags.set(regs.f);
        } break;
        case 0xfe: {                                  //CP u8
            u8 op2 = mem.read<u8>(regs.pc, regs.pc);
            u8 result = regs.a - op2;
            flags.z = (result == 0);
            flags.n = true;
            flags.h = (op2 & 0xf) > (regs.a & 0xf);
            flags.c = op2 > regs.a;
            flags.set(regs.f);
        } break;
        case 0xc0: case 0xd0: case 0xc8: case 0xd8:   //RET cond  
        if(cond((opcode >> 3) & 3))
            regs.pc = pop();
        break;
        case 0xcd: {                                  //CALL u16
            u16 addr = mem.read<u16>(regs.pc, regs.pc);
            push(regs.pc);
            regs.pc = addr;
        } break;
        case 0xc4: case 0xd4: case 0xcc: case 0xdc: { //CALL cond u16
            u16 addr = mem.read<u16>(regs.pc, regs.pc);
            if(cond((opcode >> 3) & 3)) {
                push(regs.pc);
                regs.pc = addr;
            }
        } break;
        case 0x90 ... 0x97: {                         //SUB r8
            u8 op2 = read_r8(opcode & 7);
            u8 result = regs.a - op2;
            flags.z = (result == 0);
            flags.n = true;
            flags.h = (regs.a & 0xf) < (op2 & 0xf);
            flags.c = result > regs.a;
            flags.set(regs.f);
            regs.a = result;
        } break;
        case 0xea:                                    //LD (u16), A
        mem.write<u8>(mem.read<u16>(regs.pc, regs.pc), regs.a);
        break;
        case 0xfa:                                    //LD A, (u16)
        regs.a = mem.read<u8>(mem.read<u16>(regs.pc, regs.pc), regs.pc, false);
        break;
        case 0x18: {                                  //JR i8
            i8 offset = mem.read<i8>(regs.pc, regs.pc);
            regs.pc += offset;
        } break;
        case 0x20: case 0x30: case 0x28: case 0x38: { //JR cond i8
            i8 offset = mem.read<i8>(regs.pc, regs.pc);
            if(cond((opcode >> 3) & 3))
                regs.pc += offset;
        } break;
        case 0xcb: {
            u8 cbop = mem.read<u8>(regs.pc, regs.pc);
            switch(cbop) {
                case 0x40 ... 0x7f:                   //BIT pos, r8
                flags.z = !bit(read_r8(cbop & 7), (cbop >> 3) & 7);
                flags.n = false;
                flags.h = true;
                flags.set(regs.f);
                break;
                case 0x10 ... 0x17: {                 //RL r8
                    u8 reg = read_r8(cbop & 7);
                    u8 old_reg = reg;
                    reg = (reg << 1) | flags.c;
                    write_r8(cbop & 7, reg);
                    flags.z = (reg == 0);
                    flags.n = false;
                    flags.h = false;
                    flags.c = bit(old_reg, 7);
                    flags.set(regs.f);
                } break;
                default:
                printf("Unrecognized CB prefix opcode: %02x\n", cbop);
                exit(1);
            }
        } break;
        case 0xc1: case 0xd1: case 0xe1: case 0xf1:   //POP r16
        write_r16<3>((opcode >> 4) & 3,  pop());
        break;
        case 0xc5: case 0xd5: case 0xe5: case 0xf5:   //PUSH r16
        push(read_r16<3>((opcode >> 4) & 3));
        break;
        default:
        printf("Unrecognized opcode: %02x\n", opcode);
        exit(1);
    }

    printf("AF: %04x  BC: %04x  DE: %04x  HL: %04x  SP: %04x  PC: %04x\n", regs.af, regs.bc, regs.de, regs.hl, regs.sp, regs.pc);
}

u16 Cpu::pop() {
    u16 val = mem.read<u16>(regs.sp, regs.pc, false);
    regs.sp += 2;
    return val;
}

void Cpu::push(u16 val) {
    regs.sp -= 2;
    mem.write<u16>(regs.sp, val);
}

u8 Cpu::read_r8(u8 bits) {
    switch(bits) {
        case 0: return regs.b;
        case 1: return regs.c;
        case 2: return regs.d;
        case 3: return regs.e;
        case 4: return regs.h;
        case 5: return regs.l;
        case 6: return mem.read<u8>(regs.hl, regs.pc, false);
        case 7: return regs.a;
    }
}

void Cpu::write_r8(u8 bits, u8 val) {
    switch(bits) {
        case 0: regs.b = val; break;
        case 1: regs.c = val; break;
        case 2: regs.d = val; break;
        case 3: regs.e = val; break;
        case 4: regs.h = val; break;
        case 5: regs.l = val; break;
        case 6: mem.write<u8>(regs.hl, val); break;
        case 7: regs.a = val; break;
    }
}

template <int group>
u16 Cpu::read_r16(u8 bits) {
    if(group == 1) {
        switch(bits) {
            case 0: return regs.bc;
            case 1: return regs.de;
            case 2: return regs.hl;
            case 3: return regs.sp;
        }
    } else if (group == 2) {
        switch(bits) {
            case 0: return regs.bc;
            case 1: return regs.de;
            case 2: case 3: return regs.hl;
        }
    } else if (group == 3) {
        switch(bits) {
            case 0: return regs.bc;
            case 1: return regs.de;
            case 2: return regs.hl;
            case 3: return regs.af;
        }
    }
}

template <int group>
void Cpu::write_r16(u8 bits, u16 value) {
    if(group == 1) {
        switch(bits) {
            case 0: regs.bc = value; break;
            case 1: regs.de = value; break;
            case 2: regs.hl = value; break;
            case 3: regs.sp = value; break;
        }
    } else if (group == 2) {
        switch(bits) {
            case 0: regs.bc = value; break;
            case 1: regs.de = value; break;
            case 2: case 3: regs.hl = value; break;
        }
    } else if (group == 3) {
        switch(bits) {
            case 0: regs.bc = value; break;
            case 1: regs.de = value; break;
            case 2: regs.hl = value; break;
            case 3:
            regs.a = (value >> 8) & 0xff;
            flags.z = (value >> 7) & 1;
            flags.n = (value >> 6) & 1;
            flags.h = (value >> 5) & 1;
            flags.c = (value >> 4) & 1;
            flags.set(regs.f);
            break;
        }
    }
}