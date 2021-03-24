#include "cpu.h"

void init_cpu(cpu_t* cpu, bool skip) {
    cpu->log = fopen("log.txt", "w");
    if(skip) {
        cpu->regs.af = 0x1b0; cpu->regs.bc = 0x13;
        cpu->regs.de = 0xd8; cpu->regs.hl = 0x14d;
        cpu->regs.sp = 0xfffe; cpu->regs.pc = 0x100;
    } else {
        cpu->regs.af = 0; cpu->regs.bc = 0;
        cpu->regs.de = 0; cpu->regs.hl = 0;
        cpu->regs.sp = 0; cpu->regs.pc = 0;
    }
}

void reset_cpu(cpu_t* cpu) {
    reset_bus(cpu->bus);
    if(cpu->skip) {
        cpu->regs.af = 0x1b0; cpu->regs.bc = 0x13;
        cpu->regs.de = 0xd8; cpu->regs.hl = 0x14d;
        cpu->regs.sp = 0xfffe; cpu->regs.pc = 0x100;
    } else {
        cpu->regs.af = 0; cpu->regs.bc = 0;
        cpu->regs.de = 0; cpu->regs.hl = 0;
        cpu->regs.sp = 0; cpu->regs.pc = 0;
    }
}

void step_cpu(cpu_t* cpu) {
    handle_interrupts(cpu);

    //fprintf(log, "A: %02X F: %02X B: %02X C: %02X D: %02X E: %02X H: %02X L: %02X SP: %04X PC: 00:%04X (%02X %02X %02X %02X)\n",
    //                regs.a, regs.f, regs.b, regs.c, regs.d, regs.e, regs.h, regs.l, regs.sp, regs.pc, 
    //                bus.read_byte(regs.pc), bus.read_byte(regs.pc + 1), bus.read_byte(regs.pc + 2), bus.read_byte(regs.pc + 3));

    if(!cpu->halt) {
        uint8_t opcode = next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
        cpu->cycles = opcycles[opcode];
        execute(cpu, opcode);
    } else {
        cpu->cycles = 4;
    }

    cpu->total_cycles += cpu->cycles;
}

void execute(cpu_t* cpu, uint8_t opcode) {
    switch(opcode) {
        case 0: break;                                //NOP
        case 0x01: case 0x11: case 0x21: case 0x31:   //LD r16, uint16_t
        write_r16(&cpu->regs, 1, (opcode >> 4) & 3, next_word(cpu->bus, cpu->regs.pc, &cpu->regs.pc));
        break;
        case 0x40 ... 0x75: case 0x77 ... 0x7f:       //LD r8, r8
        write_r8(cpu, (opcode >> 3) & 7, read_r8(*cpu, opcode & 7));
        break;
        case 0x06: case 0x16: case 0x26: case 0x36:
        case 0x0e: case 0x1e: case 0x2e: case 0x3e:   //LD r8, uint8_t
        write_r8(cpu, (opcode >> 3) & 7, next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc));
        break;
        case 0x04: case 0x14: case 0x24: case 0x34:
        case 0x0c: case 0x1c: case 0x2c: case 0x3c: { //INC r8
            uint8_t val = read_r8(*cpu, (opcode >> 3) & 7);
            uint8_t result = val + 1;
            bool z = (result == 0);
            bool n = false;
            bool h = ((val & 0xf) == 0xf);
            update_f(&cpu->regs.f, z, n, h, (cpu->regs.f >> 4) & 1);
            write_r8(cpu, (opcode >> 3) & 7, result);
        }
        break;
        case 0x05: case 0x15: case 0x25: case 0x35:
        case 0x0d: case 0x1d: case 0x2d: case 0x3d: { //DEC r8
            uint8_t val = read_r8(*cpu, (opcode >> 3) & 7);
            uint8_t result = val - 1;
            bool z = (result == 0);
            bool n = true;
            bool h = ((val & 0xf) == 0);
            update_f(&cpu->regs.f, z, n, h, (cpu->regs.f >> 4) & 1);
            write_r8(cpu, (opcode >> 3) & 7, result);
        }
        break;
        case 0x27: {                                  //DAA
            uint8_t offset = 0;
            
            bool z = (cpu->regs.f >> 7) & 1;
            bool n = (cpu->regs.f >> 6) & 1;
            bool h = (cpu->regs.f >> 5) & 1;
            bool c = (cpu->regs.f >> 4) & 1;

            if (h || (!n && ((cpu->regs.a & 0xf) > 9))) {
                offset |= 0x6;
            }

            if (c || (!n && (cpu->regs.a > 0x99))) {
                offset |= 0x60;
                c = true;
            }

            cpu->regs.a += (n) ? -offset : offset;
            z = (cpu->regs.a == 0);
            h = false;
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0xf3: cpu->ime = false; break;                //DI
        case 0xfb: cpu->ime = true; break;                 //EI
        case 0x03: case 0x13: case 0x23: case 0x33: { //INC r16
            uint16_t reg = read_r16(cpu->regs, 1, (opcode >> 4) & 3);
            reg++;
            write_r16(&cpu->regs, 1, (opcode >> 4) & 3, reg);
        }
        break;
        case 0x0b: case 0x1b: case 0x2b: case 0x3b: { //DEC r16
            uint16_t reg = read_r16(cpu->regs, 1, (opcode >> 4) & 3);
            reg--;
            write_r16(&cpu->regs, 1, (opcode >> 4) & 3, reg);
        }
        break;
        case 0xa8 ... 0xaf: {                         //XOR r8
            cpu->regs.a ^= read_r8(*cpu, opcode & 7);
            bool z = (cpu->regs.a == 0);
            bool n = false;
            bool h = false;
            bool c = false;
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0xee: {                                  //XOR uint8_t
            cpu->regs.a ^= next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            bool z = (cpu->regs.a == 0);
            bool n = false;
            bool h = false;
            bool c = false;
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0xe0:                                    //LD (FF00 + uint8_t), A
        write_byte(cpu->bus, 0xff00 + next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc), cpu->regs.a);
        break;
        case 0xf0:                                    //LD A, (FF00 + uint8_t)
        cpu->regs.a = read_byte(cpu->bus, 0xff00 + next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc));
        break;
        case 0xe2:                                    //LD (FF00 + C), A
        write_byte(cpu->bus, 0xff00 + cpu->regs.c, cpu->regs.a);
        break;
        case 0xf2:                                    //LD A, (FF00 + C)
        cpu->regs.a = read_byte(cpu->bus, 0xff00 + cpu->regs.c);
        break;
        case 0x02: case 0x12: case 0x22: case 0x32:   //LD (R16), A
        write_byte(cpu->bus, read_r16(cpu->regs, 2, (opcode >> 4) & 3), cpu->regs.a);
        if(opcode == 0x22)
            cpu->regs.hl++;
        if(opcode == 0x32)
            cpu->regs.hl--;
        break;
        case 0x0a: case 0x1a: case 0x2a: case 0x3a:   //LD A, (R16)
        cpu->regs.a = read_byte(cpu->bus, read_r16(cpu->regs, 2, (opcode >> 4) & 3));
        if(opcode == 0x2a)
            cpu->regs.hl++;
        if(opcode == 0x3a)
            cpu->regs.hl--;
        break;
        case 0x08:                                    //LD (uint16_t), SP
        write_word(cpu->bus, next_word(cpu->bus, cpu->regs.pc, &cpu->regs.pc), cpu->regs.sp);
        break;
        case 0xf9:                                    //LD SP, HL
        cpu->regs.sp = cpu->regs.hl;
        break;
        case 0xe8: {                                  //ADD SP, i8
            uint8_t offset = next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            bool z = false;
            bool n = false;
            bool h = (cpu->regs.sp & 0xf) + (offset & 0xf) > 0xf;
            bool c = bit16((cpu->regs.sp & 0xff) + offset, 8);
            update_f(&cpu->regs.f, z, n, h, c);
            cpu->regs.sp += (int8_t)offset;
        }
        break;
        case 0xf8: {                                  //LD HL, SP+i8
            uint8_t offset = next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            bool z = false;
            bool n = false;
            bool h = (cpu->regs.sp & 0xf) + (offset & 0xf) > 0xf;
            bool c = bit16((cpu->regs.sp & 0xff) + offset, 8);
            update_f(&cpu->regs.f, z, n, h, c);
            cpu->regs.hl = cpu->regs.sp + (int8_t)offset;
        }
        break;
        case 0x17: {                                  //RLA
            uint8_t old_a = cpu->regs.a;
            bool c = (cpu->regs.f >> 4) & 1;
            cpu->regs.a = (cpu->regs.a << 1) | c;
            bool z = false;
            bool n = false;
            bool h = false;
            c = bit8(old_a, 7);
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0x07: {                                  //RLCA
            uint8_t old_a = cpu->regs.a;
            cpu->regs.a = (cpu->regs.a << 1) | bit8(old_a, 7);
            bool z = false;
            bool n = false;
            bool h = false;
            bool c = bit8(old_a, 7);
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0x1f: {                                  //RRA
            uint8_t old_a = cpu->regs.a;
            cpu->regs.a >>= 1;
            bool c = (cpu->regs.f >> 4) & 1;
            setbit8(&cpu->regs.a, 7, c);
            bool z = false;
            bool n = false;
            bool h = false;
            c = old_a & 1;
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0x0f: {                                  //RRCA
            uint8_t old_a = cpu->regs.a;
            cpu->regs.a >>= 1;
            setbit8(&cpu->regs.a, 7, old_a & 1);
            bool z = false;
            bool n = false;
            bool h = false;
            bool c = old_a & 1;
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0x90 ... 0x97: {                         //SUB r8
            uint8_t reg = read_r8(*cpu, opcode & 7);
            uint8_t result = cpu->regs.a - reg;
            bool z = (result == 0);
            bool n = true;
            bool h = (reg & 0xf) > (cpu->regs.a & 0xf);
            bool c = result > cpu->regs.a;
            update_f(&cpu->regs.f, z, n, h, c);
            cpu->regs.a = result;
        }
        break;
        case 0xd6: {                                  //SUB uint8_t
            uint8_t op2 = next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            uint8_t result = cpu->regs.a - op2;
            bool z = (result == 0);
            bool n = true;
            bool h = (op2 & 0xf) > (cpu->regs.a & 0xf);
            bool c = result > cpu->regs.a;
            update_f(&cpu->regs.f, z, n, h, c);
            cpu->regs.a = result;
        }
        break;
        case 0x80 ... 0x87: {                         //ADD r8
            uint8_t reg = read_r8(*cpu, opcode & 7);
            uint8_t result = cpu->regs.a + reg;
            bool z = (result == 0);
            bool n = false;
            bool h = (reg & 0xf) + (cpu->regs.a & 0xf) > 0xf;
            bool c = result < cpu->regs.a;
            update_f(&cpu->regs.f, z, n, h, c);
            cpu->regs.a = result;
        }
        break;
        case 0xc6: {                                  //ADD uint8_t
            uint8_t op2 = next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            uint8_t result = cpu->regs.a + op2;
            bool z = (result == 0);
            bool n = false;
            bool h = (op2 & 0xf) + (cpu->regs.a & 0xf) > 0xf;
            bool c = result < cpu->regs.a;
            update_f(&cpu->regs.f, z, n, h, c);
            cpu->regs.a = result;            
        }
        break;
        case 0x88 ... 0x8f: {                         //ADC r8
            uint8_t reg = read_r8(*cpu, opcode & 7);
            bool c = (cpu->regs.f >> 4) & 1;
            uint16_t result = (uint16_t)(cpu->regs.a + reg + c);
            bool z = ((result & 0xff) == 0);
            bool n = false;
            bool h = (c) ? (cpu->regs.a & 0xf) + (reg & 0xf) >= 0xf
                         : (cpu->regs.a & 0xf) + (reg & 0xf) > 0xf;
            c = bit16(result, 8);
            update_f(&cpu->regs.f, z, n, h, c);
            cpu->regs.a = (result & 0xff);
        }
        break;
        case 0xce: {                                  //ADC uint8_t
            uint8_t op2 = next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            bool c = (cpu->regs.f >> 4) & 1;
            uint16_t result = (uint16_t)(cpu->regs.a + op2 + c);
            bool z = ((result & 0xff) == 0);
            bool n = false;
            bool h = (c) ? (cpu->regs.a & 0xf) + (op2 & 0xf) >= 0xf
                         : (cpu->regs.a & 0xf) + (op2 & 0xf) > 0xf;
            c = bit16(result, 8);
            update_f(&cpu->regs.f, z, n, h, c);
            cpu->regs.a = (result & 0xff);
        }
        break;
        case 0x98 ... 0x9f: {                         //SBC r8
            uint8_t reg = read_r8(*cpu, opcode & 7);
            bool c = (cpu->regs.f >> 4) & 1;
            uint16_t result = (uint16_t)(cpu->regs.a - reg - c);
            bool z = ((result & 0xff) == 0);
            bool n = true;
            bool h = (!c) ? (cpu->regs.a & 0xf) < (reg & 0xf)
                          : (cpu->regs.a & 0xf) < (reg & 0xf) + c;
            c = bit16(result, 8);
            update_f(&cpu->regs.f, z, n, h, c);
            cpu->regs.a = (result & 0xff);
        }
        break;
        case 0xde: {                                  //SBC uint8_t
            uint8_t op2 = next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            bool c = (cpu->regs.f >> 4) & 1;
            uint16_t result = (uint16_t)(cpu->regs.a - op2 - c);
            bool z = ((result & 0xff) == 0);
            bool n = true;
            bool h = (!c) ? (cpu->regs.a & 0xf) < (op2 & 0xf)
                          : (cpu->regs.a & 0xf) < (op2 & 0xf) + c;
            c = bit16(result, 8);
            update_f(&cpu->regs.f, z, n, h, c);
            cpu->regs.a = (result & 0xff);
        }
        break;
        case 0xa0 ... 0xa7: {                         //AND r8
            uint8_t reg = read_r8(*cpu, opcode & 7);
            cpu->regs.a &= reg;
            bool z = (cpu->regs.a == 0);
            bool n = false;
            bool h = true;
            bool c = false;
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0xe6: {                                  //AND uint8_t
            uint8_t op2 = next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            cpu->regs.a &= op2;
            bool z = (cpu->regs.a == 0);
            bool n = false;
            bool h = true;
            bool c = false;
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0xb0 ... 0xb7: {                         //OR r8
            uint8_t reg = read_r8(*cpu, opcode & 7);
            cpu->regs.a |= reg;
            bool z = (cpu->regs.a == 0);
            bool n = false;
            bool h = false;
            bool c = false;
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0xf6: {                                  //OR uint8_t
            uint8_t op2 = next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            cpu->regs.a |= op2;
            bool z = (cpu->regs.a == 0);
            bool n = false;
            bool h = false;
            bool c = false;
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0xb8 ... 0xbf: {                         //CP r8
            uint8_t reg = read_r8(*cpu, opcode & 7);
            uint8_t result = cpu->regs.a - reg;
            bool z = (result == 0);
            bool n = true;
            bool h = (reg & 0xf) > (cpu->regs.a & 0xf);
            bool c = result > cpu->regs.a;
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0xfe: {                                  //CP uint8_t
            uint8_t op2 = next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            uint8_t result = cpu->regs.a - op2;
            bool z = (result == 0);
            bool n = true;
            bool h = (op2 & 0xf) > (cpu->regs.a & 0xf);
            bool c = result > cpu->regs.a;
            update_f(&cpu->regs.f, z, n, h, c);
        }
        break;
        case 0xd9:                                    //RETI
        cpu->regs.pc = pop(cpu->bus, &cpu->regs.sp);
        cpu->ime = true;
        break;
        case 0xc0: case 0xd0: case 0xc8: case 0xd8: case 0xc9: //RET cond
        if(cond(&cpu->regs.f, opcode)) {
            cpu->regs.pc = pop(cpu->bus, &cpu->regs.sp);
            cpu->cycles += 12;
        }
        break;
        case 0xc7: case 0xd7: case 0xe7: case 0xf7:
        case 0xcf: case 0xdf: case 0xef: case 0xff:   //RST vec
        push(cpu->bus, &cpu->regs.sp, cpu->regs.pc);
        cpu->regs.pc = opcode & 0x38;
        break;
        case 0x2f: {                                  //CPL
            cpu->regs.a = ~cpu->regs.a;
            bool n = true;
            bool h = true;
            update_f(&cpu->regs.f, (cpu->regs.f >> 7) & 1, n, h, (cpu->regs.f >> 4) & 1);
        }
        break;
        case 0x37: {                                  //SCF
            bool n = false;
            bool h = false;
            bool c = true;
            update_f(&cpu->regs.f, (cpu->regs.f >> 7) & 1, n, h, c);
        }
        break;
        case 0x3f: {                                  //CCF
            bool n = false;
            bool h = false;
            bool c = !((cpu->regs.f >> 4) & 1);
            update_f(&cpu->regs.f, (cpu->regs.f >> 7) & 1, n, h, c);
        }
        break;
        case 0x76:                                    //HALT
        cpu->halt = true;
        break;
        case 0xc4: case 0xd4: case 0xcc: case 0xdc: case 0xcd: { //CALL cond uint16_t
            uint16_t addr = next_word(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            if(cond(&cpu->regs.f, opcode)) {
                push(cpu->bus, &cpu->regs.sp, cpu->regs.pc);
                cpu->regs.pc = addr;
                cpu->cycles += 12;
            }
        }
        break;
        case 0x09: case 0x19: case 0x29: case 0x39: { //ADD HL, r16
            uint16_t reg = read_r16(cpu->regs, 1, (opcode >> 4) & 3);
            bool n = false;
            bool h = (cpu->regs.hl & 0xfff) + (reg & 0xfff) > 0xfff;
            bool c = bit32(cpu->regs.hl + reg, 16);
            update_f(&cpu->regs.f, (cpu->regs.f >> 7) & 1, n, h, c);
            cpu->regs.hl += reg;
        }
        break;
        case 0xea:                                    //LD (uint16_t), A
        write_byte(cpu->bus, next_word(cpu->bus, cpu->regs.pc, &cpu->regs.pc), cpu->regs.a);
        break;
        case 0xfa:                                    //LD A, (uint16_t)
        cpu->regs.a = read_byte(cpu->bus, next_word(cpu->bus, cpu->regs.pc, &cpu->regs.pc));
        break;
        case 0xc2: case 0xd2: case 0xca: case 0xda: case 0xc3: { //JP cond uint16_t
            uint16_t addr = next_word(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            if(cond(&cpu->regs.f, opcode)) {
                cpu->regs.pc = addr;
                cpu->cycles += 4;
            }
        }
        break;
        case 0xe9:                                    //JP HL
        cpu->regs.pc = cpu->regs.hl;
        break;
        case 0x18:                                    //JR int8_t
        cpu->regs.pc += (int8_t)next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
        break;
        case 0x20: case 0x30: case 0x28: case 0x38: { //JR cond int8_t
            int8_t offset = (int8_t)next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            if(cond(&cpu->regs.f, opcode)) {
                cpu->regs.pc += offset;
                cpu->cycles += 4;
            }
        }
        break;
        case 0xcb: {
            uint8_t cbop = next_byte(cpu->bus, cpu->regs.pc, &cpu->regs.pc);
            cpu->cycles = cbopcycles[cbop];
            switch(cbop) {
                case 0x40 ... 0x7f: {                 //BIT pos, r8
                    bool z = !bit8(read_r8(*cpu, cbop & 7), (cbop >> 3) & 7);
                    bool n = false;
                    bool h = true;
                    update_f(&cpu->regs.f, z, n, h, (cpu->regs.f >> 4) & 1);
                }
                break;
                case 0x80 ... 0xbf: {                 //RES pos, r8
                    uint8_t reg = read_r8(*cpu, cbop & 7);
                    uint8_t pos = (cbop >> 3) & 7;
                    reg &= ~(1 << pos);
                    write_r8(cpu, cbop & 7, reg);
                }
                break;
                case 0xc0 ... 0xff: {                 //SET pos, r8
                    uint8_t reg = read_r8(*cpu, cbop & 7);
                    uint8_t pos = (cbop >> 3) & 7;
                    reg |= (1 << pos);
                    write_r8(cpu, cbop & 7, reg);
                }
                break;
                case 0 ... 0x07: {                    //RLC r8
                    uint8_t reg = read_r8(*cpu, cbop & 7);
                    uint8_t old_reg = reg;
                    reg = (reg << 1) | bit8(old_reg, 7);
                    bool z = (reg == 0);
                    bool n = false;
                    bool h = false;
                    bool c = bit8(old_reg, 7);
                    update_f(&cpu->regs.f, z, n, h, c);
                    write_r8(cpu, cbop & 7, reg);
                }
                break;
                case 0x08 ... 0x0f: {                 //RRC r8
                    uint8_t reg = read_r8(*cpu, cbop & 7);
                    uint8_t old_reg = reg;
                    reg >>= 1;
                    setbit8(&reg, 7, old_reg & 1);
                    bool z = (reg == 0);
                    bool n = false;
                    bool h = false;
                    bool c = old_reg & 1;
                    update_f(&cpu->regs.f, z, n, h, c);
                    write_r8(cpu, cbop & 7, reg);
                }
                break;
                case 0x10 ... 0x17: {                 //RL r8
                    uint8_t reg = read_r8(*cpu, cbop & 7);
                    uint8_t old_reg = reg;
                    bool c = (cpu->regs.f >> 4) & 1;
                    reg = (reg << 1) | c;
                    bool z = (reg == 0);
                    bool n = false;
                    bool h = false;
                    c = bit8(old_reg, 7);
                    update_f(&cpu->regs.f, z, n, h, c);
                    write_r8(cpu, cbop & 7, reg);
                }
                break;
                case 0x18 ... 0x1f: {                 //RR r8
                    uint8_t reg = read_r8(*cpu, cbop & 7);
                    uint8_t old_reg = reg;
                    reg >>= 1;
                    bool c = (cpu->regs.f >> 4) & 1;
                    setbit8(&reg, 7, c);
                    bool z = (reg == 0);
                    bool n = false;
                    bool h = false;
                    c = old_reg & 1;
                    update_f(&cpu->regs.f, z, n, h, c);
                    write_r8(cpu, cbop & 7, reg);
                }
                break;
                case 0x20 ... 0x27: {                 //SLA r8
                    uint8_t reg = read_r8(*cpu, cbop & 7);
                    bool c = bit8(reg, 7);
                    reg <<= 1;
                    bool z = (reg == 0);
                    bool n = false;
                    bool h = false;
                    update_f(&cpu->regs.f, z, n, h, c);
                    write_r8(cpu, cbop & 7, reg);
                }
                break;
                case 0x28 ... 0x2f: {                 //SRA r8
                    uint8_t reg = read_r8(*cpu, cbop & 7);
                    uint8_t old_reg = reg;
                    reg >>= 1;
                    setbit8(&reg, 7, bit8(old_reg, 7));
                    bool z = (reg == 0);
                    bool n = false;
                    bool h = false;
                    bool c = old_reg & 1;
                    update_f(&cpu->regs.f, z, n, h, c);
                    write_r8(cpu, cbop & 7, reg);
                }
                break;
                case 0x30 ... 0x37: {                 //SWAP r8
                    uint8_t reg = read_r8(*cpu, cbop & 7);
                    reg = (reg << 4) | (reg >> 4);
                    bool z = (reg == 0);
                    bool n = false;
                    bool h = false;
                    bool c = false;
                    update_f(&cpu->regs.f, z, n, h, c);
                    write_r8(cpu, cbop & 7, reg);
                }
                break;
                case 0x38 ... 0x3f: {                 //SRL r8
                    uint8_t reg = read_r8(*cpu, cbop & 7);
                    bool c = reg & 1;
                    reg >>= 1;
                    bool z = (reg == 0);
                    bool n = false;
                    bool h = false;
                    update_f(&cpu->regs.f, z, n, h, c);
                    write_r8(cpu, cbop & 7, reg);
                }
                break;
                default:
                printf("Unrecognized CB prefix opcode: %02x\n", cbop);
                exit(1);
            }
        }
        break;
        case 0xc1: case 0xd1: case 0xe1: case 0xf1:   //POP r16
        write_r16(&cpu->regs, 3, (opcode >> 4) & 3,  pop(cpu->bus, &cpu->regs.sp));
        break;
        case 0xc5: case 0xd5: case 0xe5: case 0xf5:   //PUSH r16
        push(cpu->bus, &cpu->regs.sp, read_r16(cpu->regs, 3, (opcode >> 4) & 3));
        break;
        default:
        printf("Unrecognized opcode: %02x\n", opcode);
        exit(1);
    }
}

void update_f(uint8_t* f, bool z, bool n, bool h, bool c) {
    *f = (z << 7) | (n << 6) | (h << 5) | (c << 4) |
         (0 << 3) | (0 << 2) | (0 << 1) | 0;
}

bool cond(uint8_t* f, uint8_t opcode) {
    if(opcode & 1)
        return true;
    uint8_t bits = (opcode >> 3) & 3;
    switch(bits) {
        case 0: return !((*f >> 7) & 1);
        case 1: return  ((*f >> 7) & 1);
        case 2: return !((*f >> 4) & 1);
        case 3: return  ((*f >> 4) & 1);
    }    
}

uint16_t pop(bus_t* bus, uint16_t* sp) {
    uint16_t val = read_word(bus, *sp);
    (*sp) += 2;
    return val;
}

void push(bus_t* bus, uint16_t* sp, uint16_t val) {
    (*sp) -= 2;
    write_word(bus, *sp, val);
}

uint8_t read_r8(cpu_t cpu, uint8_t bits) {
    switch(bits) {
        case 0: return cpu.regs.b;
        case 1: return cpu.regs.c;
        case 2: return cpu.regs.d;
        case 3: return cpu.regs.e;
        case 4: return cpu.regs.h;
        case 5: return cpu.regs.l;
        case 6: return read_byte(cpu.bus, cpu.regs.hl);
        case 7: return cpu.regs.a;
    }
}

void write_r8(cpu_t* cpu, uint8_t bits, uint8_t val) {
    switch(bits) {
        case 0: cpu->regs.b = val; break;
        case 1: cpu->regs.c = val; break;
        case 2: cpu->regs.d = val; break;
        case 3: cpu->regs.e = val; break;
        case 4: cpu->regs.h = val; break;
        case 5: cpu->regs.l = val; break;
        case 6: write_byte(cpu->bus, cpu->regs.hl, val); break;
        case 7: cpu->regs.a = val; break;
    }
}

uint16_t read_r16(regs_t regs, int group, uint8_t bits) {
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
            case 2: case 3:
            return regs.hl;
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

void write_r16(regs_t* regs, int group, uint8_t bits, uint16_t value) {
    if(group == 1) {
        switch(bits) {
            case 0: regs->bc = value; break;
            case 1: regs->de = value; break;
            case 2: regs->hl = value; break;
            case 3: regs->sp = value; break;
        }
    } else if (group == 2) {
        switch(bits) {
            case 0: regs->bc = value; break;
            case 1: regs->de = value; break;
            case 2: case 3:
            regs->hl = value;
            break;
        }
    } else if (group == 3) {
        switch(bits) {
            case 0: regs->bc = value; break;
            case 1: regs->de = value; break;
            case 2: regs->hl = value; break;
            case 3: {
                regs->a = (value >> 8) & 0xff;
                bool z = (value >> 7) & 1;
                bool n = (value >> 6) & 1;
                bool h = (value >> 5) & 1;
                bool c = (value >> 4) & 1;
                update_f(&regs->f, z, n, h, c);
            }
            break;
        }
    }
}

void handle_interrupts(cpu_t* cpu) {
    uint8_t int_mask = cpu->bus->mem->ie & cpu->bus->mem->io.intf;

    if(int_mask) {
        cpu->halt = false;
        if(cpu->ime) {
            if((cpu->bus->mem->ie & 1) && (cpu->bus->mem->io.intf & 1)) {
                cpu->bus->mem->io.intf &= ~1;
                push(cpu->bus, &cpu->regs.sp, cpu->regs.pc);
                cpu->regs.pc = 0x40;
                cpu->ime = false;
                cpu->cycles += 20;
            } else if(bit8(cpu->bus->mem->ie, 1) && bit8(cpu->bus->mem->io.intf, 1)) {
                cpu->bus->mem->io.intf &= ~2;
                push(cpu->bus, &cpu->regs.sp, cpu->regs.pc);
                cpu->regs.pc = 0x48;
                cpu->ime = false;
                cpu->cycles += 20;
            } else if(bit8(cpu->bus->mem->ie, 2) && bit8(cpu->bus->mem->io.intf, 2)) {
                cpu->bus->mem->io.intf &= ~4;
                push(cpu->bus, &cpu->regs.sp, cpu->regs.pc);
                cpu->regs.pc = 0x50;
                cpu->ime = false;
                cpu->cycles += 20;
            } else if(bit8(cpu->bus->mem->ie, 5) && bit8(cpu->bus->mem->io.intf, 3)) {
                cpu->bus->mem->io.intf &= ~8;
                push(cpu->bus, &cpu->regs.sp, cpu->regs.pc);
                cpu->regs.pc = 0x58;
                cpu->ime = false;
                cpu->cycles += 20;
            } else if(bit8(cpu->bus->mem->ie, 4) && bit8(cpu->bus->mem->io.intf, 4)) {
                cpu->bus->mem->io.intf &= ~16;
                push(cpu->bus, &cpu->regs.sp, cpu->regs.pc);
                cpu->regs.pc = 0x60;
                cpu->ime = false;
                cpu->cycles += 20;
            }
        }
    }
}

void handle_timers(cpu_t* cpu) {
    const int tima_vals[4] = { 1024, 16, 64, 256 };
    if((cpu->bus->mem->io.tac >> 2) & 1) {
        int tima_val = tima_vals[cpu->bus->mem->io.tac & 3];
        cpu->tima_cycles += cpu->cycles;
        while(cpu->tima_cycles >= tima_val) {
            cpu->tima_cycles -= tima_val;
            if(cpu->bus->mem->io.tima == 0xff) {
                cpu->bus->mem->io.tima = cpu->bus->mem->io.tma;
                cpu->bus->mem->io.intf |= 0b100;
            } else {
                cpu->bus->mem->io.tima++;
            }
        }
    }

    cpu->div_cycles += cpu->cycles;
    if(cpu->div_cycles >= 256) {
        cpu->div_cycles -= 256;
        cpu->bus->mem->io.div++;
    }
}