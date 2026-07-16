#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef struct {
  uint32_t regs[32];
  uint32_t pc;
} cpu_t;

void cpu_init(cpu_t *cpu);
void reg_write(cpu_t *cpu, uint32_t reg, uint32_t val);
void cpu_dump(const cpu_t *cpu);

#endif
