#include "syscall.h"

ecall_result_t handle_ecall(cpu_t *cpu, int *exit_code) {
  uint32_t syscall = cpu->regs[17];

  switch (syscall) {
  case 64: { // write
    uint32_t fd = cpu->regs[10];
    uint32_t addr = cpu->regs[11];
    uint32_t len = cpu->regs[12];

    FILE *stream = (fd == 2) ? stderr : stdout;

    for (uint32_t i = 0; i < len; i++) {
      uint32_t byte;
      trap_cause_t t = mem_read8(addr + i, &byte);
      if (t != TRAP_NONE) {
        reg_write(cpu, 10, i); // return bytes written so far
        return ECALL_CONTINUE;
      }
      fputc((int)byte, stream);
    }

    reg_write(cpu, 10, len); // return total bytes written
    return ECALL_CONTINUE;
  }

  case 93:                           // exit
    *exit_code = (int)cpu->regs[10]; // a0 = exit code
    return ECALL_HALT;
  default:
    fprintf(stderr, "unknown ecall %u at pc=0x%08x\n", syscall, cpu->pc);
    return ECALL_HALT; // halt on unknown, for now
  }
}
