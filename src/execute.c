#include "execute.h"

trap_cause_t execute(cpu_t *cpu, decoded_instr_t d) {
  switch (d.opcode) {
  case 0x13: // I-Format
    switch (d.funct3) {
    case 0x0: // addi
      reg_write(cpu, d.rd, cpu->regs[d.rs1] + d.imm);
      break;
    case 0x4: // xori
      reg_write(cpu, d.rd, cpu->regs[d.rs1] ^ d.imm);
      break;
    case 0x6: // ori
      reg_write(cpu, d.rd, cpu->regs[d.rs1] | d.imm);
      break;
    case 0x7: // andi
      reg_write(cpu, d.rd, cpu->regs[d.rs1] & d.imm);
      break;
    case 0x1: { // slli
      uint32_t shamt = d.imm & 0x1F;
      if (d.funct7 == 0x00) {
        reg_write(cpu, d.rd, cpu->regs[d.rs1] << shamt);
      } else {
        return TRAP_ILLEGAL_INSTR;
      }
      break;
    }
    case 0x5: { // braces to give scope for dec
      uint32_t shamt = d.imm & 0x1F;
      if (d.funct7 == 0x00) { // srli
        reg_write(cpu, d.rd, cpu->regs[d.rs1] >> shamt);
      } else if (d.funct7 == 0x20) { // srai
        reg_write(cpu, d.rd, (uint32_t)((int32_t)cpu->regs[d.rs1] >> shamt));
      } else {
        return TRAP_ILLEGAL_INSTR;
      }
      break;
    }
    case 0x2: // slti
      reg_write(cpu, d.rd,
                ((int32_t)cpu->regs[d.rs1] < (int32_t)d.imm) ? 1 : 0);
      break;
    case 0x3: // sltiu
      reg_write(cpu, d.rd, (cpu->regs[d.rs1] < (uint32_t)d.imm) ? 1 : 0);
      break;
    default: // illegal I-Format instruction
      return TRAP_ILLEGAL_INSTR;
    }
    break;   // end of I-Format
  case 0x37: // lui
    reg_write(cpu, d.rd, d.imm);
    break;
  case 0x17: // auipc
    reg_write(cpu, d.rd, cpu->pc + d.imm);
    break;
  default:
    return TRAP_ILLEGAL_INSTR;
  }

  cpu->pc += 4;
  return TRAP_NONE;
}
