.section .text
.global _start

_start:
    li   a0, 1          # fd = stdout
    la   a1, msg        # linker resolves this now
    li   a2, 6          # length
    li   a7, 64         # write syscall
    ecall

    li   a0, 0          # exit code 0
    li   a7, 93         # exit syscall
    ecall

.section .rodata
msg:
    .ascii "hello\n"
