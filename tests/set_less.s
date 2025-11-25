.section .text
.globl _start

_start:
    addi t0, x0, 0          # error accumulator

    addi t1, x0, -1         # 0xffffffff
    addi t2, x0, 1

    # slt: -1 < 1 -> 1
    slt  t3, t1, t2
    addi t4, x0, 1
    sub  t4, t3, t4
    add  t0, t0, t4

    # sltu: 0xffffffff < 1 -> 0 (unsigned)
    sltu t3, t1, t2
    addi t4, x0, 0
    sub  t4, t3, t4
    add  t0, t0, t4

    # slt: 1 < -1 (signed) -> 0
    slt  t3, t2, t1
    addi t4, x0, 0
    sub  t4, t3, t4
    add  t0, t0, t4

    # sltu: 1 < 0xffffffff (unsigned) -> 1
    sltu t3, t2, t1
    addi t4, x0, 1
    sub  t4, t3, t4
    add  t0, t0, t4

    # slti: -1 < 0 -> 1
    slti t3, t1, 0
    addi t4, x0, 1
    sub  t4, t3, t4
    add  t0, t0, t4

    # sltiu: 0xffffffff < 0 (unsigned) -> 0
    sltiu t3, t1, 0
    addi t4, x0, 0
    sub  t4, t3, t4
    add  t0, t0, t4

    addi a0, t0, 0
    addi a7, x0, 93
    ecall
