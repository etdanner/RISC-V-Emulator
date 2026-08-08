#ifndef EXECUTE_H
#define EXECUTE_H

#include "cpu.h"
#include "decode.h"
#include "memory.h"
#include "trap.h"

trap_cause_t execute(cpu_t *cpu, decoded_instr_t d);

#endif
