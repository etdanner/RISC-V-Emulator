#include "memory.h"

static uint8_t mem[MEM_SIZE];

bool mem_load(const uint8_t *buf, size_t len, uint32_t addr) {
  if (len > MEM_SIZE || addr > MEM_SIZE - len)
    return false;
  memcpy(&mem[addr], buf, len);
  return true;
}

// size must be a power of two (1, 2, or 4). the alignment mask assumes it
static trap_cause_t mem_read(uint32_t addr, uint32_t size, uint32_t *out) {
  if (addr & (size - 1))
    return TRAP_LOAD_MISALIGNED;
  if (addr > MEM_SIZE - size)
    return TRAP_LOAD_ACCESS_FAULT;

  uint32_t val = 0;
  for (uint32_t i = 0; i < size; i++) {
    val |= (uint32_t)mem[addr + i] << (8 * i);
  }
  *out = val;
  return TRAP_NONE;
}

static trap_cause_t mem_write(uint32_t addr, uint32_t size, uint32_t in) {
  if (addr & (size - 1))
    return TRAP_STORE_MISALIGNED;
  if (addr > MEM_SIZE - size)
    return TRAP_STORE_ACCESS_FAULT;

  for (uint32_t i = 0; i < size; i++) {
    mem[addr + i] = (in >> (i * 8)) & 0xFF;
  }
  return TRAP_NONE;
}

trap_cause_t mem_read8(uint32_t addr, uint32_t *out) {
  return mem_read(addr, 1, out);
}
trap_cause_t mem_read16(uint32_t addr, uint32_t *out) {
  return mem_read(addr, 2, out);
}
trap_cause_t mem_read32(uint32_t addr, uint32_t *out) {
  return mem_read(addr, 4, out);
}
trap_cause_t mem_write8(uint32_t addr, uint32_t val) {
  return mem_write(addr, 1, val);
}
trap_cause_t mem_write16(uint32_t addr, uint32_t val) {
  return mem_write(addr, 2, val);
}
trap_cause_t mem_write32(uint32_t addr, uint32_t val) {
  return mem_write(addr, 4, val);
}

trap_cause_t mem_fetch32(uint32_t addr, uint32_t *out) {
  trap_cause_t t = mem_read32(addr, out);
  if (t == TRAP_LOAD_MISALIGNED)
    return TRAP_INSTR_MISALIGNED;
  if (t == TRAP_LOAD_ACCESS_FAULT)
    return TRAP_INSTR_ACCESS_FAULT;
  return t;
}
