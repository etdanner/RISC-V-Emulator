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
    case 0x5: { // braces to give scope for shamt
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
      reg_write(cpu, d.rd, ((int32_t)cpu->regs[d.rs1] < d.imm) ? 1 : 0);
      break;
    case 0x3: // sltiu
      reg_write(cpu, d.rd, (cpu->regs[d.rs1] < (uint32_t)d.imm) ? 1 : 0);
      break;
    default: // illegal I-Format instruction
      return TRAP_ILLEGAL_INSTR;
    }
    break; // end of I-Format

  case 0x37: // lui
    reg_write(cpu, d.rd, d.imm);
    break;

  case 0x17: // auipc
    reg_write(cpu, d.rd, cpu->pc + d.imm);
    break;

  case 0x33: // R-format
    switch (d.funct3) {
    case 0x0:
      if (d.funct7 == 0x00) { // add
        reg_write(cpu, d.rd, cpu->regs[d.rs1] + cpu->regs[d.rs2]);
      } else if (d.funct7 == 0x20) { // sub
        reg_write(cpu, d.rd, cpu->regs[d.rs1] - cpu->regs[d.rs2]);
      } else {
        return TRAP_ILLEGAL_INSTR;
      }
      break;
    case 0x4: // xor
      reg_write(cpu, d.rd, cpu->regs[d.rs1] ^ cpu->regs[d.rs2]);
      break;
    case 0x6: // or
      reg_write(cpu, d.rd, cpu->regs[d.rs1] | cpu->regs[d.rs2]);
      break;
    case 0x7: // and
      reg_write(cpu, d.rd, cpu->regs[d.rs1] & cpu->regs[d.rs2]);
      break;
    case 0x1: { // sll
      uint32_t shamt = cpu->regs[d.rs2] & 0x1F;
      reg_write(cpu, d.rd, cpu->regs[d.rs1] << shamt);
      break;
    }
    case 0x5: {
      uint32_t shamt = cpu->regs[d.rs2] & 0x1F;
      if (d.funct7 == 0x00) { // srl
        reg_write(cpu, d.rd, cpu->regs[d.rs1] >> shamt);
      } else if (d.funct7 == 0x20) { // sra
        reg_write(cpu, d.rd, (uint32_t)((int32_t)cpu->regs[d.rs1] >> shamt));
      } else {
        return TRAP_ILLEGAL_INSTR;
      }
      break;
    }
    case 0x2: // slt
      reg_write(cpu, d.rd,
                ((int32_t)cpu->regs[d.rs1] < (int32_t)cpu->regs[d.rs2]) ? 1
                                                                        : 0);
      break;
    case 0x3: // sltu
      reg_write(cpu, d.rd, (cpu->regs[d.rs1] < cpu->regs[d.rs2]) ? 1 : 0);
      break;
    default: // invalid R-Format instruction
      return TRAP_ILLEGAL_INSTR;
    }
    break; // end of R-format

  case 0x23: // S-format
    switch (d.funct3) {
    case 0x0: { // sb
      trap_cause_t t = mem_write8(cpu->regs[d.rs1] + d.imm, cpu->regs[d.rs2]);
      if (t != TRAP_NONE)
        return t;
      break;
    }
    case 0x1: { // sh
      trap_cause_t t = mem_write16(cpu->regs[d.rs1] + d.imm, cpu->regs[d.rs2]);
      if (t != TRAP_NONE)
        return t;
      break;
    }
    case 0x2: { // sw
      trap_cause_t t = mem_write32(cpu->regs[d.rs1] + d.imm, cpu->regs[d.rs2]);
      if (t != TRAP_NONE)
        return t;
      break;
    }
    default:
      return TRAP_ILLEGAL_INSTR;
    }
    break; // end of S-format

  case 0x3: // load funcs
    switch (d.funct3) {
    case 0x0: { // lb
      uint32_t val;
      trap_cause_t t = mem_read8(cpu->regs[d.rs1] + d.imm, &val);
      if (t != TRAP_NONE)
        return t;
      reg_write(cpu, d.rd, (uint32_t)(int32_t)(int8_t)val); // sign-extends
      break;
    }
    case 0x1: { // lh
      uint32_t val;
      trap_cause_t t = mem_read16(cpu->regs[d.rs1] + d.imm, &val);
      if (t != TRAP_NONE)
        return t;
      reg_write(cpu, d.rd, (uint32_t)(int32_t)(int16_t)val); // sign-extends
      break;
    }
    case 0x2: { // lw
      uint32_t val;
      trap_cause_t t = mem_read32(cpu->regs[d.rs1] + d.imm, &val);
      if (t != TRAP_NONE)
        return t;
      reg_write(cpu, d.rd, val); // no extension
      break;
    }
    case 0x4: { // lbu
      uint32_t val;
      trap_cause_t t = mem_read8(cpu->regs[d.rs1] + d.imm, &val);
      if (t != TRAP_NONE)
        return t;
      reg_write(cpu, d.rd, val); // no-sign-extends
      break;
    }
    case 0x5: { // lhu
      uint32_t val;
      trap_cause_t t = mem_read16(cpu->regs[d.rs1] + d.imm, &val);
      if (t != TRAP_NONE)
        return t;
      reg_write(cpu, d.rd, val); // no-sign-extends
      break;
    }
    default:
      return TRAP_ILLEGAL_INSTR;
    }
    break; // end of load

  case 0x73: // SYSTEM
    switch (d.funct3) {
    case 0x0:
      if (d.imm == 0x0) {
        return TRAP_ECALL_M; // ecall
      } else if (d.imm == 0x1) {
        return TRAP_BREAKPOINT; // ebreak
      } else {
        return TRAP_ILLEGAL_INSTR;
      }
    default:
      return TRAP_ILLEGAL_INSTR;
    }

  default:
    return TRAP_ILLEGAL_INSTR;
  }

  cpu->pc += 4;
  return TRAP_NONE;
}
