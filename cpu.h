#include <stddef.h>
#include <stdint.h>

struct CPU {
    uint8_t reg_a;
    uint8_t reg_x;
    uint8_t reg_y;
    uint8_t status;
    uint16_t pc;
    uint8_t sp;
    uint8_t memory[0xFFFF];
};

enum AddressingMode {
   Immediate,
   ZeroPage,
   ZeroPage_X,
   ZeroPage_Y,
   Absolute,
   Absolute_X,
   Absolute_Y,
   Indirect_X,
   Indirect_Y,
   NoneAddressing,
};

struct CPUFlags {
    uint8_t CARRY;
    uint8_t ZERO;
    uint8_t INTERRUPT_DISABLE;
    uint8_t DECIMAL_MODE;
    uint8_t BREAK;
    uint8_t BREAK2;
    uint8_t OVERFLOW;
    uint8_t NEGATIVE;
};

struct Opcode {
    uint8_t code;
    char mnemoic[0x4];
    uint8_t len;
    uint8_t cycles;
    enum AddressingMode mode;
};

extern struct Opcode CPU_OPS_CODES[];

uint8_t mem_read(struct CPU *cpu, uint16_t addr);
void mem_write(struct CPU *cpu, uint16_t addr, uint8_t data);
uint16_t mem_read_u16(struct CPU *cpu, uint16_t pos);
void mem_write_u16(struct CPU *cpu, uint16_t pos, uint16_t data);

void load_and_run(struct CPU *cpu, uint8_t *program, size_t prog_size);
void load(struct CPU *cpu, uint8_t *program, size_t prog_size);
void run(struct CPU *cpu);

void update_nz_flags(struct CPU *cpu, uint8_t result);


