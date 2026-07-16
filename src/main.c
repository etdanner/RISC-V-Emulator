#include <stdint.h> // for uint
#include <stdio.h>
#include <trap.h>

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

  uint32_t instr;
  trap_cause_t res = mem_read32(pc, &instr);
  if (res == TRAP_NONE) {
    printf("pc=0x%08x inst=0x%08x\n", pc, instr);
  } else {
    fprintf(stderr, "trap %d reading pc=%08x\n", res, pc);
  }

  return 0;
}
