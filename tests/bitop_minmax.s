.section .text
.globl _start

_start:
    addi t0, x0, 0              # error accumulator

    ################################################
    # andn / orn / xnor vs base-I equivalents
    ################################################
    addi t1, x0, 0xCC           # rs1 = 0x000000CC
    addi t2, x0, 0xAA           # rs2 = 0x000000AA

    # andn: rs1 & ~rs2
    andn t3, t1, t2             # Zbb
    xori t4, t2, -1             # t4 = ~rs2 (base)
    and  t4, t1, t4             # t4 = rs1 & ~rs2 (reference)
    sub  t4, t3, t4
    add  t0, t0, t4

    # orn: rs1 | ~rs2
    orn  t3, t1, t2             # Zbb
    xori t4, t2, -1
    or   t4, t1, t4
    sub  t4, t3, t4
    add  t0, t0, t4

    # xnor: ~(rs1 ^ rs2)
    xnor t3, t1, t2             # Zbb
    xor  t4, t1, t2
    xori t4, t4, -1             # ~(rs1 ^ rs2)
    sub  t4, t3, t4
    add  t0, t0, t4

    ################################################
    # max/min (signed)
    ################################################
    addi t1, x0, -5             # rs1 = -5
    addi t2, x0, 3              # rs2 =  3

    # max(-5, 3) = 3
    max  t3, t1, t2
    addi t4, x0, 3
    sub  t4, t3, t4
    add  t0, t0, t4

    # min(-5, 3) = -5
    min  t3, t1, t2
    addi t4, x0, -5
    sub  t4, t3, t4
    add  t0, t0, t4

    ################################################
    # maxu/minu (unsigned vs signed)
    # rs1 = 0xffffffff (-1 signed), rs2 = 1
    ################################################
    addi t1, x0, -1             # 0xffffffff
    addi t2, x0, 1

    # signed max(-1, 1) = 1
    max  t3, t1, t2
    addi t4, x0, 1
    sub  t4, t3, t4
    add  t0, t0, t4

    # signed min(-1, 1) = -1
    min  t3, t1, t2
    addi t4, x0, -1
    sub  t4, t3, t4
    add  t0, t0, t4

    # unsigned maxu(0xffffffff, 1) = 0xffffffff
    maxu t3, t1, t2
    addi t4, x0, -1             # expected 0xffffffff
    sub  t4, t3, t4
    add  t0, t0, t4

    # unsigned minu(0xffffffff, 1) = 1
    minu t3, t1, t2
    addi t4, x0, 1
    sub  t4, t3, t4
    add  t0, t0, t4

    ################################################
    # exit
    ################################################
    addi a0, t0, 0
    addi a7, x0, 93
    ecall
