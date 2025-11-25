.section .text
.globl _start

_start:
    addi t0, x0, 0

    # MUL basics: 3*4=12, -2*7=-14
    addi t1, x0, 3
    addi t2, x0, 4
    mul  t3, t1, t2
    addi t4, x0, 12
    sub  t4, t3, t4
    add  t0, t0, t4

    addi t1, x0, -2
    addi t2, x0, 7
    mul  t3, t1, t2
    addi t4, x0, -14
    sub  t4, t3, t4
    add  t0, t0, t4

    # 0x00010000 * 0x00020000 -> 0x00000002_00000000
    lui  t1, 0x10          # 0x00010000
    lui  t2, 0x20          # 0x00020000

    mul  t3, t1, t2        # low 32 bits -> 0
    addi t4, x0, 0
    sub  t4, t3, t4
    add  t0, t0, t4

    mulh t3, t1, t2        # high 32 bits -> 2
    addi t4, x0, 2
    sub  t4, t3, t4
    add  t0, t0, t4

    mulhu t3, t1, t2       # high 32 bits -> 2
    addi t4, x0, 2
    sub  t4, t3, t4
    add  t0, t0, t4

    mulhsu t3, t1, t2      # high 32 bits -> 2
    addi t4, x0, 2
    sub  t4, t3, t4
    add  t0, t0, t4

    # sign behaviour: -1 * 0x80000000
    addi t1, x0, -1        # 0xffffffff
    lui  t2, 0x80000       # 0x80000000

    # mulh: signed(-1) * signed(0x80000000) = 2^31 -> high = 0
    mulh t3, t1, t2
    addi t4, x0, 0
    sub  t4, t3, t4
    add  t0, t0, t4

    # mulhu: unsigned(0xffffffff)*unsigned(0x80000000) -> high = 0x7fffffff
    mulhu t3, t1, t2
    lui  t4, 0x80000       # 0x80000000
    addi t4, t4, -1        # 0x7fffffff
    sub  t4, t3, t4
    add  t0, t0, t4

    # mulhsu: signed(-1) * unsigned(0x80000000) = -2^31 -> high = 0xffffffff
    mulhsu t3, t1, t2
    addi  t4, x0, -1       # 0xffffffff
    sub   t4, t3, t4
    add   t0, t0, t4

    addi a0, t0, 0
    addi a7, x0, 93
    ecall
