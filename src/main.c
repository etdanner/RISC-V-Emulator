#include "cpu.h"
#include "decode.h"
#include "execute.h"
#include "memory.h"
#include "trap.h"
#include <stdio.h>

static void load_program(const uint32_t *prog, size_t n) {
  for (size_t i = 0; i < n; i++) {
    mem_write32(i * 4, prog[i]);
  }
}

uint32_t prog[] = {
    0x00A00093, // addi x1, x0, 10
    0x00300113, // addi x2, x0, 3
    0x002081B3, // add  x3, x1, x2
    0x00100073, // ebreak
};

int main(void) {

  cpu_t cpu;
  cpu_init(&cpu);
  load_program(prog, sizeof prog / sizeof prog[0]);

  for (;;) {
    uint32_t raw;
    trap_cause_t t = mem_fetch32(cpu.pc, &raw);
    if (t != TRAP_NONE) {
      fprintf(stderr, "fetch trap %d at pc=0x%08x\n", t, cpu.pc);
      break;
    }

    decoded_instr_t d = decode(raw);
    t = execute(&cpu, d);

    if (t == TRAP_BREAKPOINT) {
      break; // clean stop
    }
    if (t != TRAP_NONE) {
      fprintf(stderr, "trap %d at pc=0x%08x\n", t, cpu.pc);
      break; // fault
    }
  }

  cpu_dump(&cpu);

  return 0;
}
