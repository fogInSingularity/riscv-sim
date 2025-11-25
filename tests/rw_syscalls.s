.section .data
msg:
    .asciz "Hello, world\n"

.section .text
.globl _start

_start:
    la   a1, msg
    addi a0, x0, 1      # fd = 1 (stdout)
    li   a2, 13         # len
    li   a7, 64         # write
    ecall

    addi a0, x0, 0
    li   a7, 93         # exit
    ecall
