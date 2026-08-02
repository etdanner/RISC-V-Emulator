#include "memory.h"

static uint8_t mem[MEM_SIZE];

trap_cause_t mem_read32(uint32_t addr, uint32_t *out) {
  if (addr & 0x3)
    return TRAP_LOAD_MISALIGNED;
  if (addr > MEM_SIZE - 4)
    return TRAP_LOAD_ACCESS_FAULT;

  *out = mem[addr] | (uint32_t)mem[addr + 1] << 8 |
         (uint32_t)mem[addr + 2] << 16 | (uint32_t)mem[addr + 3] << 24;
  return TRAP_NONE;
}

trap_cause_t mem_fetch32(uint32_t addr, uint32_t *out) {
  trap_cause_t t = mem_read32(addr, out);
  if (t == TRAP_LOAD_MISALIGNED)
    return TRAP_INSTR_MISALIGNED;
  if (t == TRAP_LOAD_ACCESS_FAULT)
    return TRAP_INSTR_ACCESS_FAULT;
  return t;
}

trap_cause_t mem_write32(uint32_t addr, uint32_t in) {
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
