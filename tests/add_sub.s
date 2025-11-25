.section .text
.globl _start

_start:
    addi t0, x0, 0

    addi t1, x0, 10
    addi t2, x0, 3

    add  t3, t1, t2
    addi t4, x0, 13
    sub  t4, t3, t4
    add  t0, t0, t4

    sub  t5, t1, t2
    addi t6, x0, 7
    sub  t6, t5, t6
    add  t0, t0, t6

    # 3 - 10 = -7
    sub  s0, t2, t1       # s0 = actual
    addi s1, x0, -7       # s1 = expected
    sub  s1, s0, s1       # s1 = actual - expected
    add  t0, t0, s1

    addi a0, t0, 0
    addi a7, x0, 93
    ecall

