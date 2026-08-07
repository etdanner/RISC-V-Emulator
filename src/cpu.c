#include "cpu.h"
#include <stdio.h>
#include <string.h>

#define START_ADDR 0x00000000

void cpu_init(cpu_t *cpu) {
  cpu->pc = START_ADDR;
  memset(cpu->regs, 0, sizeof cpu->regs);
}

void reg_write(cpu_t *cpu, uint32_t reg, uint32_t val) {
  if (reg != 0) { // don't write to 0x reg.
    cpu->regs[reg] = val;
  }
}

void cpu_dump(const cpu_t *cpu) {
  printf("pc = %08x\n", cpu->pc);
  printf("0x reg = %08x\n", cpu->regs[0]);
  for (int i = 1; i < 32; i++) {
    printf("reg%d = %08x ", i, cpu->regs[i]);
    if (i % 4 == 3)
      printf("\n");
  }
}
