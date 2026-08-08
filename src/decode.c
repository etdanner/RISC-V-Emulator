#include "decode.h"

// isolates bits low to high inclusively
static uint32_t bits(uint32_t x, int low, int high) {
  // (shifted int so low at bit 0) & (bit mask as wide as field)
  return (x >> low) & ((1u << (high - low + 1)) - 1);
}

decoded_instr_t decode(uint32_t instr) {
  decoded_instr_t d = {0}; // init everything to 0
  d.opcode = bits(instr, 0, 6);
  d.rd = bits(instr, 7, 11);
  d.funct3 = bits(instr, 12, 14);
  d.rs1 = bits(instr, 15, 19);
  d.rs2 = bits(instr, 20, 24);
  d.funct7 = bits(instr, 25, 31);

  switch (d.opcode) {
  case 0x03: // loads
  case 0x13: // I-format arithmetic
  case 0x67: // jalr
  case 0x73: // system
    d.imm = (int32_t)instr >> 20;
    break;
  case 0x37: // U-format
  case 0x17:
    d.imm = (int32_t)(instr & 0xFFFFF000);
    break;
  case 0x23: { // S-format (stores)
    uint32_t high = bits(instr, 25, 31);
    uint32_t low = bits(instr, 7, 11);
    int32_t imm = (int32_t)((high << 5) | low);
    d.imm = (imm << 20) >> 20; // sign-extends by bit 11.
    break;
  }
  }

  return d;
}
