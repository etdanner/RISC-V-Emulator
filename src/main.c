#include "cpu.h"
#include "decode.h"
#include "execute.h"
#include "memory.h"
#include "trap.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <string.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <program.bin>\n", argv[0]);
    return 1;
  }

  FILE *f = fopen(argv[1], "rb");
  if (f == NULL) {
    fprintf(stderr, "error opening '%s': %s\n", argv[1], strerror(errno));
    return 1;
    return 1;
  }

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  if (size < 0) {
    fprintf(stderr, "ftell failed\n");
    fclose(f);
    return 1;
  }
  fseek(f, 0, SEEK_SET);

  uint8_t *buf = malloc(size);
  if (buf == NULL) {
    fprintf(stderr, "out of mem\n");
    fclose(f);
    return 1;
  }

  if (fread(buf, 1, size, f) != (size_t)size) {
    fprintf(stderr, "read fewer bytes than expected\n");
    free(buf);
    fclose(f);
    return 1;
  }
  fclose(f);

  if (mem_load(buf, size, 0) == false) {
    fprintf(stderr, "buf too big for mem\n");
    free(buf);
    return 1;
  }
  free(buf);

  cpu_t cpu;
  cpu_init(&cpu);

  for (;;) {
    uint32_t raw;
    trap_cause_t t = mem_fetch32(cpu.pc, &raw);
    if (t != TRAP_NONE) {
      fprintf(stderr, "fetch trap %d at pc=0x%08x\n", t, cpu.pc);
      break;
    }

    decoded_instr_t d = decode(raw);
    t = execute(&cpu, d);

    if (t == TRAP_BREAKPOINT) {
      break; // clean stop
    }
    if (t != TRAP_NONE) {
      fprintf(stderr, "trap %d at pc=0x%08x\n", t, cpu.pc);
      break; // fault
    }
  }

  cpu_dump(&cpu);

  return 0;
}
