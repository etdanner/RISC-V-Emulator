.section .text
.global _start
_start:
    li   t0, 1          # pass code
    li   t1, 0x1000     # TOHOST_ADDR (match your #define)
    sw   t0, 0(t1)
