.section .text
.globl _start

_start:
    addi t0, x0, 0          # error accumulator

    # addi: 0 -> 1 -> 3 -> 0
    addi t1, x0, 0
    addi t1, t1, 1          # 1
    addi t1, t1, 2          # 3
    addi t1, t1, -3         # 0
    add  t0, t0, t1         # if sequence correct, no change

    # addi sign extension: -1 + 1 = 0
    addi t2, x0, -1         # 0xffffffff
    addi t2, t2, 1          # 0
    add  t0, t0, t2

    # x0 must ignore writes
    addi x0, t2, 7          # attempt to write x0
    addi t3, x0, 0          # should still read 0
    add  t0, t0, t3         # if x0 != 0, error accumulates

    addi a0, t0, 0          # exit code = error accumulator
    addi a7, x0, 93
    ecall
