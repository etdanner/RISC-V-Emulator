#ifndef RISCV_TEST_H
#define RISCV_TEST_H

#define TESTNUM gp

#define RVTEST_RV32U

#define RVTEST_CODE_BEGIN   \
  .section .text;           \
  .global _start;           \
_start:

#define RVTEST_CODE_END

#define RVTEST_PASS         \
  li a0, 0x1000;            \
  li a1, 1;                 \
  sw a1, 0(a0);             \
1: j 1b;

#define RVTEST_FAIL         \
  li a0, 0x1000;            \
  slli a1, TESTNUM, 1;      \
  ori a1, a1, 1;            \
  sw a1, 0(a0);             \
1: j 1b;

#define RVTEST_DATA_BEGIN .section .data;
#define RVTEST_DATA_END
#define RVTEST_DATA
#define TEST_DATA

#endif
