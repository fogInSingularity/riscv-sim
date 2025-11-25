.section .text
.globl _start

_start:
    addi t0, x0, 0          # error accumulator

    jal  x0, after          # jump; rd = x0 (no link)
    addi t0, t0, 1          # executed only if JAL fails

after:
    addi a0, t0, 0
    addi a7, x0, 93
    ecall
