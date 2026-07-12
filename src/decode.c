#include "decode.h"

// isolates bits low to high inclusively
static uint32_t bits(uint32_t x, int high, int low) {
  // (shifted int so low at bit 0) & (bit mask as wide as field)
  return (x >> low) & ((1u << (high - low + 1)) - 1);
}

decoded_instr_t decode(uint32_t instr) {
  decoded_instr_t d = {0}; // init everything to 0
  d.opcode = bits(instr, 6, 0);
  d.rd = bits(instr, 11, 7);
  d.funct3 = bits(instr, 14, 12);
  d.rs1 = bits(instr, 19, 15);
  d.rs2 = bits(instr, 24, 20);
  d.funct7 = bits(instr, 31, 25);
  // sign-extended, only for I-type immediate atm.
  d.imm = (int32_t)instr >> 20;

  return d;
}
