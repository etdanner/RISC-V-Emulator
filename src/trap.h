#ifndef TRAP_H
#define TRAP_H

#define TOHOST_ADDR 0x1000

typedef enum {
  TRAP_TOHOST = -2,            // to report pass/fail for test suite
  TRAP_NONE = -1,              // Executed correctly (nothing happened)
  TRAP_INSTR_MISALIGNED = 0,   // Target isn't 4-byte aligned
  TRAP_INSTR_ACCESS_FAULT = 1, // Failed instruction fetch
  TRAP_ILLEGAL_INSTR = 2,      // Illegal instruction: decode failed
  TRAP_BREAKPOINT = 3,         // Breakpoint called by ebreak instr
  TRAP_LOAD_MISALIGNED = 4,    // Load addr missaligned
  TRAP_LOAD_ACCESS_FAULT = 5,  // load outside physical mem
  TRAP_STORE_MISALIGNED = 6,   // store addr missaligned
  TRAP_STORE_ACCESS_FAULT = 7, // write outside physical mem
  TRAP_ECALL_U = 8,            // for user mode
  TRAP_ECALL_M = 11,           // for machine mode
} trap_cause_t;

#endif
