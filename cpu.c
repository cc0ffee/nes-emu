#include "cpu.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

const uint16_t STACK = 0x0100;
const uint8_t STACK_RESET = 0xfd;

struct Opcode CPU_OPS_CODES[] = {
        {0x00, "BRK", 1, 7, NoneAddressing},
        {0xea, "NOP", 1, 2, NoneAddressing},

        /* Arithmetic */
        {0x69, "ADC", 2, 2, Immediate},
        {0x65, "ADC", 2, 3, ZeroPage},
        {0x75, "ADC", 2, 4, ZeroPage_X},
        {0x6d, "ADC", 3, 4, Absolute},
        {0x7d, "ADC", 3, 4/*+1 if page crossed*/, Absolute_X},
        {0x79, "ADC", 3, 4/*+1 if page crossed*/, Absolute_Y},
        {0x61, "ADC", 2, 6, Indirect_X},
        {0x71, "ADC", 2, 5/*+1 if page crossed*/, Indirect_Y},

        {0xe9, "SBC", 2, 2, Immediate},
        {0xe5, "SBC", 2, 3, ZeroPage},
        {0xf5, "SBC", 2, 4, ZeroPage_X},
        {0xed, "SBC", 3, 4, Absolute},
        {0xfd, "SBC", 3, 4/*+1 if page crossed*/, Absolute_X},
        {0xf9, "SBC", 3, 4/*+1 if page crossed*/, Absolute_Y},
        {0xe1, "SBC", 2, 6, Indirect_X},
        {0xf1, "SBC", 2, 5/*+1 if page crossed*/, Indirect_Y},

        {0x29, "AND", 2, 2, Immediate},
        {0x25, "AND", 2, 3, ZeroPage},
        {0x35, "AND", 2, 4, ZeroPage_X},
        {0x2d, "AND", 3, 4, Absolute},
        {0x3d, "AND", 3, 4/*+1 if page crossed*/, Absolute_X},
        {0x39, "AND", 3, 4/*+1 if page crossed*/, Absolute_Y},
        {0x21, "AND", 2, 6, Indirect_X},
        {0x31, "AND", 2, 5/*+1 if page crossed*/, Indirect_Y},

        {0x49, "EOR", 2, 2, Immediate},
        {0x45, "EOR", 2, 3, ZeroPage},
        {0x55, "EOR", 2, 4, ZeroPage_X},
        {0x4d, "EOR", 3, 4, Absolute},
        {0x5d, "EOR", 3, 4/*+1 if page crossed*/, Absolute_X},
        {0x59, "EOR", 3, 4/*+1 if page crossed*/, Absolute_Y},
        {0x41, "EOR", 2, 6, Indirect_X},
        {0x51, "EOR", 2, 5/*+1 if page crossed*/, Indirect_Y},

        {0x09, "ORA", 2, 2, Immediate},
        {0x05, "ORA", 2, 3, ZeroPage},
        {0x15, "ORA", 2, 4, ZeroPage_X},
        {0x0d, "ORA", 3, 4, Absolute},
        {0x1d, "ORA", 3, 4/*+1 if page crossed*/, Absolute_X},
        {0x19, "ORA", 3, 4/*+1 if page crossed*/, Absolute_Y},
        {0x01, "ORA", 2, 6, Indirect_X},
        {0x11, "ORA", 2, 5/*+1 if page crossed*/, Indirect_Y},

        /* Shifts */
        {0x0a, "ASL", 1, 2, NoneAddressing},
        {0x06, "ASL", 2, 5, ZeroPage},
        {0x16, "ASL", 2, 6, ZeroPage_X},
        {0x0e, "ASL", 3, 6, Absolute},
        {0x1e, "ASL", 3, 7, Absolute_X},

        {0x4a, "LSR", 1, 2, NoneAddressing},
        {0x46, "LSR", 2, 5, ZeroPage},
        {0x56, "LSR", 2, 6, ZeroPage_X},
        {0x4e, "LSR", 3, 6, Absolute},
        {0x5e, "LSR", 3, 7, Absolute_X},

        {0x2a, "ROL", 1, 2, NoneAddressing},
        {0x26, "ROL", 2, 5, ZeroPage},
        {0x36, "ROL", 2, 6, ZeroPage_X},
        {0x2e, "ROL", 3, 6, Absolute},
        {0x3e, "ROL", 3, 7, Absolute_X},

        {0x6a, "ROR", 1, 2, NoneAddressing},
        {0x66, "ROR", 2, 5, ZeroPage},
        {0x76, "ROR", 2, 6, ZeroPage_X},
        {0x6e, "ROR", 3, 6, Absolute},
        {0x7e, "ROR", 3, 7, Absolute_X},

        {0xe6, "INC", 2, 5, ZeroPage},
        {0xf6, "INC", 2, 6, ZeroPage_X},
        {0xee, "INC", 3, 6, Absolute},
        {0xfe, "INC", 3, 7, Absolute_X},

        {0xe8, "INX", 1, 2, NoneAddressing},
        {0xc8, "INY", 1, 2, NoneAddressing},

        {0xc6, "DEC", 2, 5, ZeroPage},
        {0xd6, "DEC", 2, 6, ZeroPage_X},
        {0xce, "DEC", 3, 6, Absolute},
        {0xde, "DEC", 3, 7, Absolute_X},

        {0xca, "DEX", 1, 2, NoneAddressing},
        {0x88, "DEY", 1, 2, NoneAddressing},

        {0xc9, "CMP", 2, 2, Immediate},
        {0xc5, "CMP", 2, 3, ZeroPage},
        {0xd5, "CMP", 2, 4, ZeroPage_X},
        {0xcd, "CMP", 3, 4, Absolute},
        {0xdd, "CMP", 3, 4/*+1 if page crossed*/, Absolute_X},
        {0xd9, "CMP", 3, 4/*+1 if page crossed*/, Absolute_Y},
        {0xc1, "CMP", 2, 6, Indirect_X},
        {0xd1, "CMP", 2, 5/*+1 if page crossed*/, Indirect_Y},

        {0xc0, "CPY", 2, 2, Immediate},
        {0xc4, "CPY", 2, 3, ZeroPage},
        {0xcc, "CPY", 3, 4, Absolute},

        {0xe0, "CPX", 2, 2, Immediate},
        {0xe4, "CPX", 2, 3, ZeroPage},
        {0xec, "CPX", 3, 4, Absolute},


        /* Branching */

        {0x4c, "JMP", 3, 3, NoneAddressing}, //AddressingMode that acts as Immidiate
        {0x6c, "JMP", 3, 5, NoneAddressing}, //AddressingMode:Indirect with 6502 bug

        {0x20, "JSR", 3, 6, NoneAddressing},
        {0x60, "RTS", 1, 6, NoneAddressing},

        {0x40, "RTI", 1, 6, NoneAddressing},

        {0xd0, "BNE", 2, 2 /*(+1 if branch succeeds +2 if to a new page}*/, NoneAddressing},
        {0x70, "BVS", 2, 2 /*(+1 if branch succeeds +2 if to a new page}*/, NoneAddressing},
        {0x50, "BVC", 2, 2 /*(+1 if branch succeeds +2 if to a new page}*/, NoneAddressing},
        {0x30, "BMI", 2, 2 /*(+1 if branch succeeds +2 if to a new page}*/, NoneAddressing},
        {0xf0, "BEQ", 2, 2 /*(+1 if branch succeeds +2 if to a new page}*/, NoneAddressing},
        {0xb0, "BCS", 2, 2 /*(+1 if branch succeeds +2 if to a new page}*/, NoneAddressing},
        {0x90, "BCC", 2, 2 /*(+1 if branch succeeds +2 if to a new page}*/, NoneAddressing},
        {0x10, "BPL", 2, 2 /*(+1 if branch succeeds +2 if to a new page}*/, NoneAddressing},

        {0x24, "BIT", 2, 3, ZeroPage},
        {0x2c, "BIT", 3, 4, Absolute},


        /* Stores, Loads */
        {0xa9, "LDA", 2, 2, Immediate},
        {0xa5, "LDA", 2, 3, ZeroPage},
        {0xb5, "LDA", 2, 4, ZeroPage_X},
        {0xad, "LDA", 3, 4, Absolute},
        {0xbd, "LDA", 3, 4/*+1 if page crossed*/, Absolute_X},
        {0xb9, "LDA", 3, 4/*+1 if page crossed*/, Absolute_Y},
        {0xa1, "LDA", 2, 6, Indirect_X},
        {0xb1, "LDA", 2, 5/*+1 if page crossed*/, Indirect_Y},

        {0xa2, "LDX", 2, 2, Immediate},
        {0xa6, "LDX", 2, 3, ZeroPage},
        {0xb6, "LDX", 2, 4, ZeroPage_Y},
        {0xae, "LDX", 3, 4, Absolute},
        {0xbe, "LDX", 3, 4/*+1 if page crossed*/, Absolute_Y},

        {0xa0, "LDY", 2, 2, Immediate},
        {0xa4, "LDY", 2, 3, ZeroPage},
        {0xb4, "LDY", 2, 4, ZeroPage_X},
        {0xac, "LDY", 3, 4, Absolute},
        {0xbc, "LDY", 3, 4/*+1 if page crossed*/, Absolute_X},


        {0x85, "STA", 2, 3, ZeroPage},
        {0x95, "STA", 2, 4, ZeroPage_X},
        {0x8d, "STA", 3, 4, Absolute},
        {0x9d, "STA", 3, 5, Absolute_X},
        {0x99, "STA", 3, 5, Absolute_Y},
        {0x81, "STA", 2, 6, Indirect_X},
        {0x91, "STA", 2, 6, Indirect_Y},

        {0x86, "STX", 2, 3, ZeroPage},
        {0x96, "STX", 2, 4, ZeroPage_Y},
        {0x8e, "STX", 3, 4, Absolute},

        {0x84, "STY", 2, 3, ZeroPage},
        {0x94, "STY", 2, 4, ZeroPage_X},
        {0x8c, "STY", 3, 4, Absolute},


        /* Flags clear */

        {0xD8, "CLD", 1, 2, NoneAddressing},
        {0x58, "CLI", 1, 2, NoneAddressing},
        {0xb8, "CLV", 1, 2, NoneAddressing},
        {0x18, "CLC", 1, 2, NoneAddressing},
        {0x38, "SEC", 1, 2, NoneAddressing},
        {0x78, "SEI", 1, 2, NoneAddressing},
        {0xf8, "SED", 1, 2, NoneAddressing},

        {0xaa, "TAX", 1, 2, NoneAddressing},
        {0xa8, "TAY", 1, 2, NoneAddressing},
        {0xba, "TSX", 1, 2, NoneAddressing},
        {0x8a, "TXA", 1, 2, NoneAddressing},
        {0x9a, "TXS", 1, 2, NoneAddressing},
        {0x98, "TYA", 1, 2, NoneAddressing},

        /* Stack */
        {0x48, "PHA", 1, 3, NoneAddressing},
        {0x68, "PLA", 1, 4, NoneAddressing},
        {0x08, "PHP", 1, 3, NoneAddressing},
        {0x28, "PLP", 1, 4, NoneAddressing},

};

uint8_t mem_read(struct CPU *cpu, uint16_t addr) {
    return cpu->memory[addr];
}
void mem_write(struct CPU *cpu, uint16_t addr, uint8_t data) {
    cpu->memory[addr] = data;
}

uint16_t mem_read_u16(struct CPU *cpu, uint16_t pos) {
    uint16_t low = mem_read(cpu, pos);
    uint16_t high = mem_read(cpu, pos + 1);
    return (high << 8) | low;
}
void mem_write_u16(struct CPU *cpu, uint16_t pos, uint16_t data) {
    uint8_t low = (data & 0xFF);
    uint8_t high = (data >> 8) & 0xFF;
    mem_write(cpu, pos, low);
    mem_write(cpu, pos + 1, high);
}

uint16_t get_operand_address(struct CPU *cpu, enum AddressingMode mode) {
    switch (mode) {
    case Immediate: return cpu->pc; 
    case ZeroPage: return mem_read(cpu, cpu->pc);
    case Absolute: return mem_read_u16(cpu, cpu->pc);
    case ZeroPage_X: {
        uint16_t pos = mem_read(cpu, cpu->pc);
        uint16_t addr = (uint16_t)(pos + cpu->reg_x);
        return addr & 0xFF;
    }
    case ZeroPage_Y: {
        uint16_t pos = mem_read(cpu, cpu->pc);
        uint16_t addr = (uint16_t)(pos + cpu->reg_y);
        return addr & 0xFF;
    }
    case Absolute_X: {
        uint16_t base = mem_read_u16(cpu, cpu->pc);
        uint16_t addr = (uint16_t)(base + cpu->reg_x);
        return addr & 0xFF;
    }
    case Absolute_Y: {
        uint16_t base = mem_read_u16(cpu, cpu->pc);
        uint16_t addr = (uint16_t)(base + cpu->reg_y);
        return addr & 0xFF;
    }
    case Indirect_X: {
        uint8_t base = mem_read(cpu, cpu->pc);
        uint8_t ptr = (base + cpu->reg_x) & 0xFF;
        uint8_t low = mem_read(cpu, (uint16_t)ptr);
        uint8_t high = mem_read(cpu, (uint16_t)(ptr+1));
        return ((uint16_t)high << 8) | (uint16_t)low;
    }
    case Indirect_Y: {
        uint8_t base = mem_read(cpu, cpu->pc);
        uint8_t ptr = (base + cpu->reg_y) & 0xFF;
        uint8_t low = mem_read(cpu, (uint16_t)ptr);
        uint8_t high = mem_read(cpu, (uint16_t)(ptr+1));
        return ((uint16_t)high << 8) | (uint16_t)low;
    }
    case NoneAddressing:
        exit(EXIT_FAILURE);
    }
}

struct Opcode* find_opcode(uint8_t opcode) {
    int num_opcodes = sizeof(CPU_OPS_CODES) / sizeof(struct Opcode);
    for (int i = 0; i < num_opcodes; i++) {
        if (CPU_OPS_CODES[i].code == opcode) {
            return &CPU_OPS_CODES[i];
        }
    }
    return NULL;
}

void reset(struct CPU *cpu) {
    cpu->reg_a = 0;
    cpu->reg_x = 0;
    cpu->reg_y = 0;
    cpu->status = 0;
    cpu->pc = mem_read_u16(cpu, 0xFFFC);
    printf("Reset PC set to: 0x%04X\n", cpu->pc);
}

void load(struct CPU *cpu, uint8_t *program, size_t prog_size) {
    printf("Loaded program into memory:\n");
    for(size_t i = 0; i < prog_size; i++) {
        cpu->memory[0x8000 + i] = program[i];
        printf("0x%04zX: 0x%02X\n", 0x8000 + i, cpu->memory[0x8000 + i]);
    }
    uint16_t pos = 0xFFFC;
    uint16_t data = 0x8000;
    mem_write_u16(cpu, pos, data);
}

void load_and_run(struct CPU *cpu, uint8_t *program, size_t prog_size) {
    load(cpu, program, prog_size);
    reset(cpu);
    run(cpu);
}

void update_nz_flags(struct CPU *cpu, uint8_t result) {
    if (result == 0) {
        cpu->status = cpu->status | 0b00000010;
    } else {
        cpu->status = cpu->status & 0b11111101;
    }
                
    if ((result & 0b10000000) != 0) {
        cpu->status = cpu->status | 0b10000000;
    } else {
        cpu->status = cpu->status & 0b01111111;
    }
    return;
}

void run(struct CPU *cpu) {
    while(1) {
        printf("PC: 0x%04X\n", cpu->pc);
        uint8_t opcode = mem_read(cpu, cpu->pc);
        struct Opcode *op = find_opcode(opcode);
        cpu->pc += 1;
        uint16_t current_pc_state = cpu->pc;
        switch (opcode) {
            // ADC
            case 0x69:
            case 0x65:
            case 0x75:
            case 0x6d:
            case 0x7d:
            case 0x79:
            case 0x61:
            case 0x71: {}

            // SBC
            case 0xe9:
            case 0xe5:
            case 0xf5:
            case 0xed:
            case 0xfd:
            case 0xf9:
            case 0xe1:
            case 0xf1: {}

            // AND
            case 0x29:
            case 0x25:
            case 0x35:
            case 0x2d:
            case 0x3d:
            case 0x39:
            case 0x21:
            case 0x31: {
                uint16_t addr = get_operand_address(cpu, op->mode);
                uint8_t val = mem_read(cpu, addr);
                cpu->reg_a = cpu->reg_a & val;
                update_nz_flags(cpu, cpu->reg_a);
                break;
            }

            // EOR
            case 0x49:
            case 0x45:
            case 0x55:
            case 0x4d:
            case 0x5d:
            case 0x59:
            case 0x41:
            case 0x51: {
                uint16_t addr = get_operand_address(cpu, op->mode);
                uint8_t val = mem_read(cpu, addr);
                cpu->reg_a = cpu->reg_a ^ val;
                update_nz_flags(cpu, cpu->reg_a);
                break;
            }

            //ORA
            case 0x09:
            case 0x05:
            case 0x15:
            case 0x0d:
            case 0x1d:
            case 0x19:
            case 0x01:
            case 0x11: {
                uint16_t addr = get_operand_address(cpu, op->mode);
                uint8_t val = mem_read(cpu, addr);
                cpu->reg_a = cpu->reg_a | val;
                update_nz_flags(cpu, cpu-> reg_a);
                break;
            }

            // ASL
            case 0x0a:
            case 0x06:
            case 0x16:
            case 0x0e:
            case 0x1e: {}

            // LSR
            case 0x4a:
            case 0x46:
            case 0x56:
            case 0x4e:
            case 0x5e: {}

            // ROL
            case 0x2a:
            case 0x26:
            case 0x36:
            case 0x2e:
            case 0x3e: {}

            // ROR
            case 0x6a:
            case 0x66:
            case 0x76:
            case 0x6e:
            case 0x7e: {}
            
            // INC
            case 0xe6:
            case 0xf6:
            case 0xee:
            case 0xfe: {}

            // INX
            case 0xe8: {}
            // INY
            case 0xc8: {}

            // DEC
            case 0xc6:
            case 0xd6:
            case 0xce:
            case 0xde: {}

            // DEX
            case 0xca: {}
            // DEY
            case 0x88: {}

            // CMP
            case 0xc9:
            case 0xc5:
            case 0xd5:
            case 0xcd:
            case 0xdd:
            case 0xd9:
            case 0xc1:
            case 0xd1: {}

            // CPY
            case 0xc0:
            case 0xc4:
            case 0xcc: {}

            // CPX:
            case 0xe0:
            case 0xe4:
            case 0xec: {}

            // JMP
            case 0x4c: {
                uint16_t addr = mem_read_u16(cpu, cpu->pc);
                cpu->pc = addr;
                break;
            }
            case 0x6c: {
    
            }

            // JSR
            case 0x20: {
                uint16_t addr = mem_read_u16(cpu, ((cpu->pc + 2) - 1));
                uint16_t t_addr = mem_read_u16(cpu, cpu->pc);
                cpu->pc = t_addr;
                break;
            }
            // RTS
            case 0x60: {}

            // RTI
            case 0x40: {}

            // BNE
            case 0xd0: {}
            // BVS
            case 0x70: {}
            // BVC
            case 0x50: {}
            // BMI
            case 0x30: {}
            // BEQ
            case 0xf0: {}
            // BCS
            case 0xb0: {}
            // BCC
            case 0x90: {}
            // BPL
            case 0x10: {}

            //BIT
            case 0x24:
            case 0x2c: {}

            // LDA
            case 0xa9:
            case 0xa5:
            case 0xb5:
            case 0xad:
            case 0xbd:
            case 0xb9:
            case 0xa1:
            case 0xb1: {
                uint16_t addr = get_operand_address(cpu, op->mode);
                uint8_t val = mem_read(cpu, addr);
                cpu->reg_a = val;
                update_nz_flags(cpu, cpu->reg_a);
                break;
            }

            // LDX
            case 0xa2:
            case 0xa6:
            case 0xb6:
            case 0xae:
            case 0xbe: {
                uint16_t addr = get_operand_address(cpu, op->mode);
                uint8_t val = mem_read(cpu, addr);
                cpu->reg_x = val;
                update_nz_flags(cpu, cpu->reg_x);
                break;
            }

            // LDY
            case 0xa0:
            case 0xa4:
            case 0xb4:
            case 0xac:
            case 0xbc: {
                uint16_t addr = get_operand_address(cpu, op->mode);
                uint8_t val = mem_read(cpu, addr);
                cpu->reg_y = val;
                update_nz_flags(cpu, cpu->reg_y);
                break;
            }
            
            // STA
            case 0x85:
            case 0x95:
            case 0x8d:
            case 0x9d:
            case 0x99:
            case 0x81:
            case 0x91: {
                uint16_t addr = get_operand_address(cpu, op->mode);
                mem_write(cpu, addr, cpu->reg_a);
                break;
            }

            // STX
            case 0x86:
            case 0x96:
            case 0x8e: {
                uint16_t addr = get_operand_address(cpu, op->mode);
                mem_write(cpu, addr, cpu->reg_x);
                break;
            }

            // STY
            case 0x84:
            case 0x94:
            case 0x8c: {
                uint16_t addr = get_operand_address(cpu, op->mode);
                mem_write(cpu, addr, cpu->reg_y);
                break;
            }

            // CLD
            case 0xd8: {}
            // CLI
            case 0x58: {}
            // CLV
            case 0xb8: {}
            // CLC
            case 0x18: {}
            // SEC
            case 0x38: {}
            // SEI
            case 0x78: {}
            // SED
            case 0xf8: {}

            // TAX
            case 0xaa: {
                cpu->reg_x = cpu->reg_a;
                update_nz_flags(cpu, cpu->reg_x);
                break;                
            }
            // TAY
            case 0xa8: {
                cpu->reg_y = cpu->reg_a;
                update_nz_flags(cpu, cpu->reg_y);
                break;
            }
            // TSX
            case 0xba: {
                cpu->reg_x = cpu->sp;
                update_nz_flags(cpu, cpu->reg_x);
                break;
            }
            // TXA
            case 0x8a: {
                cpu->reg_a = cpu->reg_x;
                update_nz_flags(cpu, cpu->reg_a);
                break;
            }
            // TXS
            case 0x9a: {
                cpu->sp = cpu->reg_x;
                break;
            }
            // TYA
            case 0x98: {
                cpu->reg_a = cpu->reg_y;
                update_nz_flags(cpu, cpu->reg_a);
                break;
            }

            // PHA
            case 0x48: {}
            // PLA
            case 0x68: {}
            // PHP
            case 0x08: {}
            // PLP
            case 0x28: {}

            // NOP
            case 0xea: {
                continue;
            }
            // BRK
            case 0x00: {
                return;
            }
            default: {
                printf("Unhandled opcode: 1x%02X\n", opcode);
                return;
            }
        }
        if (current_pc_state == cpu->pc) {
            cpu->pc += (uint16_t)(op->len - 1);
        }
    }
}
