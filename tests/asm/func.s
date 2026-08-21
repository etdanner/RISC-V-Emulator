    jal  x1, func
    addi x5, x0, 99     # this runs AFTER func returns
    ebreak
func:
    addi x6, x0, 7
    jalr x0, x1, 0
