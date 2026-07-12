#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>

// Based on the RV32I instruction formats
typedef struct {
  uint32_t opcode; // bits 6-0
  uint32_t rd;     // bits 11-7
  uint32_t funct3; // bits 14-12
  uint32_t rs1;    // bits 19-15
  uint32_t rs2;    // bits 24-20
  uint32_t funct7; // bits 31-25
  uint32_t imm;    // bits depend on the opcode
} decoded_instr_t;

decoded_instr_t decode(uint32_t instr);

#endif
