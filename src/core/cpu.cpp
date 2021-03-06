#include "cpu.h"

Cpu::Cpu() {
    //log = fopen("log.txt", "w");
    //regs.af = 0; regs.bc = 0;
    //regs.de = 0; regs.hl = 0;
    //regs.sp = 0; regs.pc = 0;
    flags.z = true; flags.h = true; flags.c = true;
    regs.af = 0x1b0; regs.bc = 0x13;
    regs.de = 0xd8; regs.hl = 0x14d;
    regs.sp = 0xfffe; regs.pc = 0x100;
}

void Cpu::reset() {
    //regs.af = 0; regs.bc = 0;
    //regs.de = 0; regs.hl = 0;
    //regs.sp = 0; regs.pc = 0;
    flags.z = true; flags.n = false; flags.h = true; flags.c = true;
    regs.af = 0x1b0; regs.bc = 0x13;
    regs.de = 0xd8; regs.hl = 0x14d;
    regs.sp = 0xfffe; regs.pc = 0x100;
}

void Cpu::step() {
    //printf("A: %02x F: %02x B: %02x C: %02x D: %02x E: %02x H: %02x L: %02x SP: %04x PC: 00:%04x (%02x %02x %02x %02x)\n",
    //        regs.a, regs.f, regs.b, regs.c, regs.d, regs.e, regs.h, regs.l, regs.sp, regs.pc,
    //        mem.read<u8>(regs.pc, regs.pc, false), mem.read<u8>(regs.pc + 1, regs.pc, false), mem.read<u8>(regs.pc + 2, regs.pc, false), mem.read<u8>(regs.pc + 3, regs.pc, false));

    //fprintf(log, "A: %02x F: %02x B: %02x C: %02x D: %02x E: %02x H: %02x L: %02x SP: %04x PC: 00:%04x (%02x %02x %02x %02x)\n",
    //        regs.a, regs.f, regs.b, regs.c, regs.d, regs.e, regs.h, regs.l, regs.sp, regs.pc,
    //        mem.read<u8>(regs.pc, regs.pc, false), mem.read<u8>(regs.pc + 1, regs.pc, false), mem.read<u8>(regs.pc + 2, regs.pc, false), mem.read<u8>(regs.pc + 3, regs.pc, false));

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
        case 0x27: {
            u8 offset = 0;

            if (flags.h || (!flags.n && ((regs.a & 0xf) > 9))) {
                offset |= 0x6;
            }

            if (flags.c || (!flags.n && (regs.a > 0x99))) {
                offset |= 0x60;
                flags.c = true;
            }

            regs.a += (flags.n) ? -offset : offset;
            flags.z = (regs.a == 0);
            flags.h = false;
            flags.set(regs.f);
        } break;
        case 0xf3: break; //TODO:                     DI STUB
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
        case 0xee: {                                  //XOR u8
            regs.a ^= mem.read<u8>(regs.pc, regs.pc);
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
        case 0x02: case 0x12: case 0x22: case 0x32:   //LD (R16), A
        mem.write<u8>(read_r16<2>((opcode >> 4) & 3), regs.a);
        if(opcode == 0x22)
            regs.hl++;
        if(opcode == 0x32)
            regs.hl--;
        break;
        case 0x0a: case 0x1a: case 0x2a: case 0x3a:   //LD A, (R16)
        regs.a = mem.read<u8>(read_r16<2>((opcode >> 4) & 3), regs.pc, false);
        if(opcode == 0x2a)
            regs.hl++;
        if(opcode == 0x3a)
            regs.hl--;
        break;
        case 0x08:                                    //LD (u16), SP
        mem.write<u16>(mem.read<u16>(regs.pc, regs.pc), regs.sp);
        break;
        case 0xf9:                                    //LD SP, HL
        regs.sp = regs.hl;
        break;
        case 0xe8: {                                  //ADD SP, i8
            u8 offset = mem.read<u8>(regs.pc, regs.pc);
            flags.z = false;
            flags.n = false;
            flags.h = (regs.sp & 0xf) + (offset & 0xf) > 0xf;
            flags.c = bit<u16>((regs.sp & 0xff) + offset, 8);
            flags.set(regs.f);
            regs.sp += (i8)offset;
        } break;
        case 0xf8: {                                  //LD HL, SP+i8
            u8 offset = mem.read<u8>(regs.pc, regs.pc);
            flags.z = false;
            flags.n = false;
            flags.h = (regs.sp & 0xf) + (offset & 0xf) > 0xf;
            flags.c = bit<u16>((regs.sp & 0xff) + offset, 8);
            flags.set(regs.f);
            regs.hl = regs.sp + (i8)offset;
        } break;
        case 0x17: {                                  //RLA
            u8 old_a = regs.a;
            regs.a = (regs.a << 1) | flags.c;
            flags.z = false;
            flags.n = false;
            flags.h = false;
            flags.c = bit<u8>(old_a, 7);
            flags.set(regs.f);
        } break;
        case 0x07: {                                  //RLCA
            u8 old_a = regs.a;
            regs.a = (regs.a << 1) | bit<u8>(old_a, 7);
            flags.z = false;
            flags.n = false;
            flags.h = false;
            flags.c = bit<u8>(old_a, 7);
            flags.set(regs.f);
        } break;
        case 0x1f: {                                  //RRA
            u8 old_a = regs.a;
            regs.a >>= 1;
            setbit<u8>(regs.a, 7, flags.c);
            flags.z = false;
            flags.n = false;
            flags.h = false;
            flags.c = old_a & 1;
            flags.set(regs.f);
        } break;
        case 0x0f: {                                  //RRCA
            u8 old_a = regs.a;
            regs.a >>= 1;
            setbit<u8>(regs.a, 7, old_a & 1);
            flags.z = false;
            flags.n = false;
            flags.h = false;
            flags.c = old_a & 1;
            flags.set(regs.f);
        } break;
        case 0x90 ... 0x97: {                         //SUB r8
            u8 reg = read_r8(opcode & 7);
            u8 result = regs.a - reg;
            flags.z = (result == 0);
            flags.n = true;
            flags.h = (reg & 0xf) > (regs.a & 0xf);
            flags.c = result > regs.a;
            flags.set(regs.f);
            regs.a = result;
        } break;
        case 0xd6: {                                  //SUB u8
            u8 op2 = mem.read<u8>(regs.pc, regs.pc);
            u8 result = regs.a - op2;
            flags.z = (result == 0);
            flags.n = true;
            flags.h = (op2 & 0xf) > (regs.a & 0xf);
            flags.c = result > regs.a;
            flags.set(regs.f);
            regs.a = result;
        } break;
        case 0x80 ... 0x87: {                         //ADD r8
            u8 reg = read_r8(opcode & 7);
            u8 result = regs.a + reg;
            flags.z = (result == 0);
            flags.n = false;
            flags.h = (reg & 0xf) + (regs.a & 0xf) > 0xf;
            flags.c = result < regs.a;
            flags.set(regs.f);
            regs.a = result;
        } break;
        case 0xc6: {                                  //ADD u8
            u8 op2 = mem.read<u8>(regs.pc, regs.pc);
            u8 result = regs.a + op2;
            flags.z = (result == 0);
            flags.n = false;
            flags.h = (op2 & 0xf) + (regs.a & 0xf) > 0xf;
            flags.c = result < regs.a;
            flags.set(regs.f);
            regs.a = result;            
        } break;
        case 0x88 ... 0x8f: {                         //ADC r8
            u8 reg = read_r8(opcode & 7);
            u16 result = u16(regs.a + reg + flags.c);
            flags.z = ((result & 0xff) == 0);
            flags.n = false;
            flags.h = (flags.c) ? (regs.a & 0xf) + (reg & 0xf) >= 0xf
                                : (regs.a & 0xf) + (reg & 0xf) > 0xf;
            flags.c = bit<u16>(result, 8);
            flags.set(regs.f);
            regs.a = (result & 0xff);
        } break;
        case 0xce: {                                  //ADC u8
            u8 op2 = mem.read<u8>(regs.pc, regs.pc);
            u16 result = u16(regs.a + op2 + flags.c);
            flags.z = ((result & 0xff) == 0);
            flags.n = false;
            flags.h = (flags.c) ? (regs.a & 0xf) + (op2 & 0xf) >= 0xf
                                : (regs.a & 0xf) + (op2 & 0xf) > 0xf;
            flags.c = bit<u16>(result, 8);
            flags.set(regs.f);
            regs.a = (result & 0xff);
        } break;
        case 0x98 ... 0x9f: {                         //SBC r8
            u8 reg = read_r8(opcode & 7);
            u16 result = u16(regs.a - reg - flags.c);
            flags.z = ((result & 0xff) == 0);
            flags.n = true;
            flags.h = (!flags.c) ? (regs.a & 0xf) < (reg & 0xf)
                                 : (regs.a & 0xf) < (reg & 0xf) + flags.c;
            flags.c = bit<u16>(result, 8);
            flags.set(regs.f);
            regs.a = (result & 0xff);
        } break;
        case 0xde: {                                  //SBC u8
            u8 op2 = mem.read<u8>(regs.pc, regs.pc);
            u16 result = u16(regs.a - op2 - flags.c);
            flags.z = ((result & 0xff) == 0);
            flags.n = true;
            flags.h = (!flags.c) ? (regs.a & 0xf) < (op2 & 0xf)
                                 : (regs.a & 0xf) < (op2 & 0xf) + flags.c;
            flags.c = bit<u16>(result, 8);
            flags.set(regs.f);
            regs.a = (result & 0xff);
        } break;
        case 0xa0 ... 0xa7: {                         //AND r8
            u8 reg = read_r8(opcode & 7);
            regs.a &= reg;
            flags.z = (regs.a == 0);
            flags.n = false;
            flags.h = true;
            flags.c = false;
            flags.set(regs.f);
        } break;
        case 0xe6: {                                  //AND u8
            u8 op2 = mem.read<u8>(regs.pc, regs.pc);
            regs.a &= op2;
            flags.z = (regs.a == 0);
            flags.n = false;
            flags.h = true;
            flags.c = false;
            flags.set(regs.f);
        } break;
        case 0xb0 ... 0xb7: {                         //OR r8
            u8 reg = read_r8(opcode & 7);
            regs.a |= reg;
            flags.z = (regs.a == 0);
            flags.n = false;
            flags.h = false;
            flags.c = false;
            flags.set(regs.f);
        } break;
        case 0xf6: {                                  //OR u8
            u8 op2 = mem.read<u8>(regs.pc, regs.pc);
            regs.a |= op2;
            flags.z = (regs.a == 0);
            flags.n = false;
            flags.h = false;
            flags.c = false;
            flags.set(regs.f);
        } break;
        case 0xb8 ... 0xbf: {                         //CP r8
            u8 reg = read_r8(opcode & 7);
            u8 result = regs.a - reg;
            flags.z = (result == 0);
            flags.n = true;
            flags.h = (reg & 0xf) > (regs.a & 0xf);
            flags.c = result > regs.a;
            flags.set(regs.f);
        } break;
        case 0xfe: {                                  //CP u8
            u8 op2 = mem.read<u8>(regs.pc, regs.pc);
            u8 result = regs.a - op2;
            flags.z = (result == 0);
            flags.n = true;
            flags.h = (op2 & 0xf) > (regs.a & 0xf);
            flags.c = result > regs.a;
            flags.set(regs.f);
        } break;
        case 0xc9: case 0xd9: case 0xc0: case 0xd0:   //RET cond
        case 0xc8: case 0xd8:
        if(cond(opcode))
            regs.pc = pop();
        break;
        case 0xc7: case 0xd7: case 0xe7: case 0xf7:
        case 0xcf: case 0xdf: case 0xef: case 0xff: { //RST vec
            push(regs.pc);
            regs.pc = u16(opcode & 0x38);
        } break;
        case 0x2f:                                    //CPL
        regs.a = ~regs.a;
        flags.n = true;
        flags.h = true;
        flags.set(regs.f);
        break;
        case 0x37:                                    //SCF
        flags.n = false;
        flags.h = false;
        flags.c = true;
        flags.set(regs.f);
        break;
        case 0x3f:                                    //CCF
        flags.n = false;
        flags.h = false;
        flags.c = !flags.c;
        flags.set(regs.f);
        break;
        case 0x76: halt = true; break;                //CALL cond u16
        case 0xc4: case 0xd4: case 0xcc: case 0xcd: case 0xdc: {
            u16 addr = mem.read<u16>(regs.pc, regs.pc);
            if(cond(opcode)) {
                push(regs.pc);
                regs.pc = addr;
            }
        } break;
        case 0x09: case 0x19: case 0x29: case 0x39: { //ADD HL, r16
            u16 reg = read_r16<1>((opcode >> 4) & 3);
            flags.n = false;
            flags.h = (regs.hl & 0xfff) + (reg & 0xfff) > 0xfff;
            flags.c = bit<u32>(regs.hl + reg, 16);
            flags.set(regs.f);
            regs.hl += reg;
        } break;
        case 0xea:                                    //LD (u16), A
        mem.write<u8>(mem.read<u16>(regs.pc, regs.pc), regs.a);
        break;
        case 0xfa:                                    //LD A, (u16)
        regs.a = mem.read<u8>(mem.read<u16>(regs.pc, regs.pc), regs.pc, false);
        break;                                        //JP cond u16
        case 0xc2: case 0xc3: case 0xd2: case 0xca: case 0xda: {
            u16 addr = mem.read<u16>(regs.pc, regs.pc);
            if(cond(opcode))
                regs.pc = addr;
        } break;
        case 0xe9: regs.pc = regs.hl; break;          //JP HL
        case 0x18: {                                  //JR i8
            regs.pc += mem.read<i8>(regs.pc, regs.pc);
        } break;
        case 0x20: case 0x30: case 0x28: case 0x38: { //JR cond i8
            i8 offset = mem.read<i8>(regs.pc, regs.pc);
            if(cond(opcode))
                regs.pc += offset;
        } break;
        case 0xcb: {
            u8 cbop = mem.read<u8>(regs.pc, regs.pc);
            switch(cbop) {
                case 0x40 ... 0x7f:                   //BIT pos, r8
                flags.z = !bit<u8>(read_r8(cbop & 7), (cbop >> 3) & 7);
                flags.n = false;
                flags.h = true;
                flags.set(regs.f);
                break;
                case 0x80 ... 0xbf: {                 //RES pos, r8
                    u8 reg = read_r8(cbop & 7);
                    u8 pos = (cbop >> 3) & 7;
                    reg &= ~(1 << pos);
                    write_r8(cbop & 7, reg);
                } break;
                case 0xc0 ... 0xff: {                 //SET pos, r8
                    u8 reg = read_r8(cbop & 7);
                    u8 pos = (cbop >> 3) & 7;
                    reg |= (1 << pos);
                    write_r8(cbop & 7, reg);
                } break;
                case 0 ... 0x07: {                    //RLC r8
                    u8 reg = read_r8(cbop & 7);
                    u8 old_reg = reg;
                    reg = (reg << 1) | bit<u8>(old_reg, 7);
                    flags.z = (reg == 0);
                    flags.n = false;
                    flags.h = false;
                    flags.c = bit<u8>(old_reg, 7);
                    flags.set(regs.f);
                    write_r8(cbop & 7, reg);
                } break;
                case 0x08 ... 0x0f: {                 //RRC r8
                    u8 reg = read_r8(cbop & 7);
                    u8 old_reg = reg;
                    reg >>= 1;
                    setbit<u8>(reg, 7, old_reg & 1);
                    flags.z = (reg == 0);
                    flags.n = false;
                    flags.h = false;
                    flags.c = old_reg & 1;
                    flags.set(regs.f);
                    write_r8(cbop & 7, reg);
                } break;
                case 0x10 ... 0x17: {                 //RL r8
                    u8 reg = read_r8(cbop & 7);
                    u8 old_reg = reg;
                    reg = (reg << 1) | flags.c;
                    flags.z = (reg == 0);
                    flags.n = false;
                    flags.h = false;
                    flags.c = bit<u8>(old_reg, 7);
                    flags.set(regs.f);
                    write_r8(cbop & 7, reg);
                } break;
                case 0x18 ... 0x1f: {                 //RR r8
                    u8 reg = read_r8(cbop & 7);
                    u8 old_reg = reg;
                    reg >>= 1;
                    setbit<u8>(reg, 7, flags.c);
                    flags.z = (reg == 0);
                    flags.n = false;
                    flags.h = false;
                    flags.c = old_reg & 1;
                    flags.set(regs.f);
                    write_r8(cbop & 7, reg);
                } break;
                case 0x20 ... 0x27: {                 //SLA r8
                    u8 reg = read_r8(cbop & 7);
                    flags.c = bit<u8>(reg, 7);
                    reg <<= 1;
                    flags.z = (reg == 0);
                    flags.n = false;
                    flags.h = false;
                    flags.set(regs.f);
                    write_r8(cbop & 7, reg);
                } break;
                case 0x28 ... 0x2f: {                 //SRA r8
                    u8 reg = read_r8(cbop & 7);
                    u8 old_reg = reg;
                    reg >>= 1;
                    setbit<u8>(reg, 7, bit<u8>(old_reg, 7));
                    flags.z = (reg == 0);
                    flags.n = false;
                    flags.h = false;
                    flags.c = old_reg & 1;
                    flags.set(regs.f);
                    write_r8(cbop & 7, reg);
                } break;
                case 0x30 ... 0x37: {                 //SWAP r8
                    u8 reg = read_r8(cbop & 7);
                    reg = (reg << 4) | (reg >> 4);
                    flags.z = (reg == 0);
                    flags.n = false;
                    flags.h = false;
                    flags.c = false;
                    flags.set(regs.f);
                    write_r8(cbop & 7, reg);
                } break;
                case 0x38 ... 0x3f: {                 //SRL r8
                    u8 reg = read_r8(cbop & 7);
                    flags.c = reg & 1;
                    reg >>= 1;
                    flags.z = (reg == 0);
                    flags.n = false;
                    flags.h = false;
                    flags.set(regs.f);
                    write_r8(cbop & 7, reg);
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