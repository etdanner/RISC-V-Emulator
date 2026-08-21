#ifndef SYSCALL_H
#define SYSCALL_H

#include "cpu.h"
#include "memory.h"
#include <stdio.h>

typedef enum { ECALL_CONTINUE, ECALL_HALT } ecall_result_t;

ecall_result_t handle_ecall(cpu_t *cpu, int *exit_code);

#endif
