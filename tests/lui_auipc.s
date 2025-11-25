.section .text
.globl _start

_start:
    addi t0, x0, 0          # error accumulator

    # LUI: compare with (1 << 12)
    addi t1, x0, 1
    slli t1, t1, 12         # expected = 0x00001000
    lui  t2, 0x1            # actual   = 0x00001000
    sub  t3, t2, t1
    add  t0, t0, t3

    # AUIPC: two consecutive AUIPC 0 should differ by 4
    auipc t4, 0
    auipc t5, 0
    sub  t6, t5, t4         # t6 = actual difference
    addi s0, x0, 4          # s0 = expected difference = 4
    sub  s0, t6, s0         # s0 = actual - expected
    add  t0, t0, s0

    addi a0, t0, 0
    addi a7, x0, 93
    ecall

