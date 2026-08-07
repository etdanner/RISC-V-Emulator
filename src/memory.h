#ifndef MEMORY_H
#define MEMORY_H

#include "trap.h"
#include <stdint.h>

#define MEM_SIZE (1024 * 1024)

trap_cause_t mem_read8(uint32_t addr, uint32_t *out);
trap_cause_t mem_read16(uint32_t addr, uint32_t *out);
trap_cause_t mem_read32(uint32_t addr, uint32_t *out);
trap_cause_t mem_write8(uint32_t addr, uint32_t val);
trap_cause_t mem_write16(uint32_t addr, uint32_t val);
trap_cause_t mem_write32(uint32_t addr, uint32_t val);
trap_cause_t mem_fetch32(uint32_t addr, uint32_t *out);

#endif
