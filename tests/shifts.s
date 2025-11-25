.section .text
.globl _start

_start:
    addi t0, x0, 0          # error accumulator

    # slli: 1 << 3 = 8
    addi t1, x0, 1
    slli t2, t1, 3
    addi t3, x0, 8
    sub  t3, t2, t3
    add  t0, t0, t3

    # srli: 8 >> 2 = 2 (logical)
    srli t2, t2, 2
    addi t3, x0, 2
    sub  t3, t2, t3
    add  t0, t0, t3

    # srai: -16 >> 2 = -4 (arithmetic)
    addi t1, x0, -16
    srai t2, t1, 2
    addi t3, x0, -4
    sub  t3, t2, t3
    add  t0, t0, t3

    addi a0, t0, 0
    addi a7, x0, 93
    ecall
