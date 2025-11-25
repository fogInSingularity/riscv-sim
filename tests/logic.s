.section .text
.globl _start

_start:
    addi t0, x0, 0          # error accumulator

    addi t1, x0, 5          # 0b0101
    addi t2, x0, 3          # 0b0011

    # and: 5 & 3 = 1
    and  t3, t1, t2
    addi t4, x0, 1
    sub  t4, t3, t4
    add  t0, t0, t4

    # or: 5 | 3 = 7
    or   t3, t1, t2
    addi t4, x0, 7
    sub  t4, t3, t4
    add  t0, t0, t4

    # xor: 5 ^ 3 = 6
    xor  t3, t1, t2
    addi t4, x0, 6
    sub  t4, t3, t4
    add  t0, t0, t4

    # andi: 5 & 1 = 1
    andi t3, t1, 1
    addi t4, x0, 1
    sub  t4, t3, t4
    add  t0, t0, t4

    # ori: 3 | 4 = 7
    ori  t3, t2, 4
    addi t4, x0, 7
    sub  t4, t3, t4
    add  t0, t0, t4

    # xori: 3 ^ 6 = 5
    xori t3, t2, 6
    addi t4, x0, 5
    sub  t4, t3, t4
    add  t0, t0, t4

    addi a0, t0, 0
    addi a7, x0, 93
    ecall
