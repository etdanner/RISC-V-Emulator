#include "syscall.h"

ecall_result_t handle_ecall(cpu_t *cpu, int *exit_code) {
  uint32_t syscall = cpu->regs[17]; // a7
  switch (syscall) {
  case 93:                           // exit
    *exit_code = (int)cpu->regs[10]; // a0 = exit code
    return ECALL_HALT;
  default:
    fprintf(stderr, "unknown ecall %u at pc=0x%08x\n", syscall, cpu->pc);
    return ECALL_HALT; // halt on unknown, for now
  }
}
