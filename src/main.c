#include <stdint.h> // for uint
#include <stdio.h>

typedef enum {
  TRAP_NONE = -1,              // Executed correctly (nothing happened)
  TRAP_INSN_MISALIGNED = 0,    // Target isn't 4-byte aligned
  TRAP_INSN_ACCESS_FAULT = 1,  // Failed instruction fetch
  TRAP_ILLEGAL_INSN = 2,       // Illegal instruction: decode failed
  TRAP_BREAKPOINT = 3,         // Breakpoint called by ebreak insn
  TRAP_LOAD_MISALIGNED = 4,    // Load addr missaligned
  TRAP_LOAD_ACCESS_FAULT = 5,  // load outside physical mem
  TRAP_STORE_MISALIGNED = 6,   // store addr missaligned
  TRAP_STORE_ACCESS_FAULT = 7, // write outside physical mem
  TRAP_ECALL_U = 8,            // for user mode
  TRAP_ECALL_M = 11,           // for machine mode
} trap_cause_t;

#define MEM_SIZE (1024 * 1024)
#define START_ADDR 0x00000000

static uint32_t reg[32];
static uint32_t pc = START_ADDR;
static uint8_t mem[MEM_SIZE];

static trap_cause_t mem_read32(uint32_t addr, uint32_t *out) {
  if (addr & 0x3)
    return TRAP_LOAD_MISALIGNED;
  if (addr > MEM_SIZE - 4)
    return TRAP_LOAD_ACCESS_FAULT;

  *out = mem[addr] | (uint32_t)mem[addr + 1] << 8 |
         (uint32_t)mem[addr + 2] << 16 | (uint32_t)mem[addr + 3] << 24;
  return TRAP_NONE;
}

static trap_cause_t mem_write32(uint32_t addr, uint32_t in) {
  if (addr & 0x3)
    return TRAP_STORE_MISALIGNED;
  if (addr > MEM_SIZE - 4)
    return TRAP_STORE_ACCESS_FAULT;

  mem[addr] = in & 0xFF;
  mem[addr + 1] = (in >> 8) & 0xFF;
  mem[addr + 2] = (in >> 16) & 0xFF;
  mem[addr + 3] = (in >> 24) & 0xFF;
  return TRAP_NONE;
}

int main(void) {
  mem_write32(0, 0x00500093); // addi x1, x0, 5

  uint32_t inst;
  trap_cause_t res = mem_read32(pc, &inst);
  if (res == TRAP_NONE) {
    printf("pc=0x%08x inst=0x%08x\n", pc, inst);
  } else {
    fprintf(stderr, "trap %d reading pc=%08x\n", res, pc);
  }

  return 0;
}
