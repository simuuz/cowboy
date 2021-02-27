#include "cpu.h"

Cpu::Cpu(Mem& mem) : mem(mem) {
    regs.a = 0; regs.f = 0;
    regs.bc = 0; regs.de = 0;
    regs.hl = 0; regs.sp = 0;
    regs.pc = 0;
}

void Cpu::step() {
    u8 opcode = mem.read<u8>(regs.pc, regs.pc);
    switch(opcode) {
        case 0: break;
        case 0x01: case 0x11: case 0x21: case 0x31:
        write_r16<1>((opcode >> 4) & 3, mem.read<u16>(regs.pc, regs.pc));
        break;
        default:
        printf("unrecognized opcode: %02x\n", opcode);
        exit(1);
    }

    printf("AF: %02x%02x\tBC: %04x\tDE: %04x\tHL: %04x\tSP: %04x\tPC: %04x\n", regs.a, regs.f, regs.bc, regs.de, regs.hl, regs.sp, regs.pc);
}

u16 Cpu::pop() {
    regs.sp--;
    u16 val = mem.read<u16>(regs.sp, regs.pc, false);
    return val;
}

void Cpu::push(u16 val) {
    mem.write<u16>(regs.sp, val);
    regs.sp++;
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
        case 0: regs.b = val;
        case 1: regs.c = val;
        case 2: regs.d = val;
        case 3: regs.e = val;
        case 4: regs.h = val;
        case 5: regs.l = val;
        case 6: mem.write<u8>(regs.hl, val);
        case 7: regs.a = val;
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
            case 0: regs.bc = value;
            case 1: regs.de = value;
            case 2: regs.hl = value;
            case 3: regs.sp = value;
        }
    } else if (group == 2) {
        switch(bits) {
            case 0: regs.bc = value;
            case 1: regs.de = value;
            case 2: case 3: regs.hl = value;
        }
    } else if (group == 3) {
        switch(bits) {
            case 0: regs.bc = value;
            case 1: regs.de = value;
            case 2: regs.hl = value;
            case 3: regs.af = value;
        }
    }
}