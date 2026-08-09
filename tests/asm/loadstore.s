addi x1, x0, 42
sw   x1, 256(x0)
lw   x2, 256(x0)
addi x3, x0, -128
sb   x3, 260(x0)
lb   x4, 260(x0)
lbu  x5, 260(x0)
ebreak
