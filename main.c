#include "cpu.h"
#include <stdio.h>

int main() {
    struct CPU cpu = {0};
    uint8_t prog[] = {0xA9, 0x05, 0x00};    
    load_and_run(&cpu, prog, sizeof(prog));
    printf("Register A: %02X\n", cpu.reg_a);
    printf("Register X: %02X\n", cpu.reg_x);
    printf("Register Y: %02X\n", cpu.reg_y);
    return 0;
}
