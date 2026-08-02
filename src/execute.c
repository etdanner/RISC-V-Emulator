#include "execute.h"

trap_cause_t execute(cpu_t *cpu, decoded_instr_t d) {
  switch (d.opcode) {
  case 0x13:
    switch (d.funct3) {
    case 0x0: // addi
      reg_write(cpu, d.rd, cpu->regs[d.rs1] + d.imm);
      break;
    default:
      return TRAP_ILLEGAL_INSTR;
    }
    break; // end of immediate instrs
  default:
    return TRAP_ILLEGAL_INSTR;
  }

  cpu->pc += 4;
  return TRAP_NONE;
}
