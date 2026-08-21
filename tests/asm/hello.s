    li   a0, 1          # fd = stdout
      li a1, 0x20     # address of msg (known from layout)
    li   a2, 6          # length = 6 bytes ("hello\n")
    li   a7, 64         # syscall = write
    ecall

    li   a0, 0          # exit code 0
    li   a7, 93         # syscall = exit
    ecall

msg:
    .ascii "hello\n"
