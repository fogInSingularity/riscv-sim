.section .text
.globl _start

_start:
    addi a0, zero, 0      # exit code = 0
    addi a7, zero, 93     # syscall: exit
    ecall

