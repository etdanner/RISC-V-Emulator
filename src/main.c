#include "cpu.h"
#include "decode.h"
#include "execute.h"
#include "memory.h"
#include "trap.h"
#include <stdio.h>

int main(void) {

  cpu_t cpu;
  cpu_init(&cpu);
  mem_write32(0, 0x00500093); // addi x1, x0, 5

  uint32_t raw = 0;
  trap_cause_t instr = mem_fetch32(cpu.pc, &raw);
  if (instr == TRAP_NONE) {
    decoded_instr_t d = decode(raw);
    trap_cause_t e = execute(&cpu, d);
    if (e != TRAP_NONE) {
      fprintf(stderr, "execute failure");
    }
    cpu_dump(&cpu);
  } else {
    fprintf(stderr, "mem_fetch32 fail");
  }

  return 0;
}
