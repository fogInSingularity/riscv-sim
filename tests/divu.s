.section .text
.globl _start

_start:
    addi t0, x0, 0

    # 7 / 2 unsigned
    addi t1, x0, 7
    addi t2, x0, 2
    divu t3, t1, t2
    addi t4, x0, 3
    sub  t4, t3, t4
    add  t0, t0, t4

    remu t3, t1, t2
    addi t4, x0, 1
    sub  t4, t3, t4
    add  t0, t0, t4

    # 0x80000000 / 2 -> 0x40000000, rem 0
    lui  t1, 0x80000       # 0x80000000
    addi t2, x0, 2
    divu t3, t1, t2
    lui  t4, 0x40000       # 0x40000000
    sub  t4, t3, t4
    add  t0, t0, t4

    remu t3, t1, t2
    addi t4, x0, 0
    sub  t4, t3, t4
    add  t0, t0, t4

    # division by zero unsigned: 5 / 0 -> 0xffffffff, rem -> 5
    addi t1, x0, 5
    addi t2, x0, 0
    divu t3, t1, t2
    addi t4, x0, -1        # 0xffffffff
    sub  t4, t3, t4
    add  t0, t0, t4

    remu t3, t1, t2
    addi t4, x0, 5
    sub  t4, t3, t4
    add  t0, t0, t4

    addi a0, t0, 0
    addi a7, x0, 93
    ecall
