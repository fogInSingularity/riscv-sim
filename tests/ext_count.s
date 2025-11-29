.section .text
.globl _start

_start:
    addi t0, x0, 0              # error accumulator

    ################################################
    # sext.b
    ################################################
    addi t1, x0, 0x7F           # 0x0000007F
    sext.b t2, t1               # expect 0x0000007F
    addi t3, x0, 0x7F
    sub  t3, t2, t3
    add  t0, t0, t3

    addi t1, x0, 0x80           # 0x00000080 -> 0xFFFFFF80
    sext.b t2, t1
    addi t3, x0, -128           # 0xFFFFFF80
    sub  t3, t2, t3
    add  t0, t0, t3

    ################################################
    # sext.h
    ################################################
    lui  t1, 0x8                # 0x00008000
    addi t2, t1, -1             # 0x00007FFF
    sext.h t3, t2               # positive; should be unchanged
    sub  t4, t3, t2
    add  t0, t0, t4

    sext.h t3, t1               # 0x00008000 -> 0xFFFF8000
    lui  t4, 0xFFFF8            # 0xFFFF8000
    addi t4, t4, 0
    sub  t4, t3, t4
    add  t0, t0, t4

    ################################################
    # zext.h : 0xFFFF8001 -> 0x00008001
    ################################################
    lui  t1, 0xFFFF8            # 0xFFFF8000
    addi t1, t1, 1              # 0xFFFF8001
    zext.h t2, t1               # expect 0x00008001
    lui  t3, 0x8                # 0x00008000
    addi t3, t3, 1              # 0x00008001
    sub  t3, t2, t3
    add  t0, t0, t3

    ################################################
    # clz tests
    ################################################
    addi t1, x0, 1              # 0x00000001
    clz  t2, t1                 # expect 31
    addi t3, x0, 31
    sub  t3, t2, t3
    add  t0, t0, t3

    lui  t1, 0x80000            # 0x80000000
    clz  t2, t1                 # expect 0
    addi t3, x0, 0
    sub  t3, t2, t3
    add  t0, t0, t3

    addi t1, x0, 0              # clz(0) = XLEN = 32
    clz  t2, t1
    addi t3, x0, 32
    sub  t3, t2, t3
    add  t0, t0, t3

    ################################################
    # ctz tests
    ################################################
    addi t1, x0, 1              # 0x...0001 -> ctz = 0
    ctz  t2, t1
    addi t3, x0, 0
    sub  t3, t2, t3
    add  t0, t0, t3

    lui  t1, 0x80000            # 0x80000000 -> ctz = 31
    ctz  t2, t1
    addi t3, x0, 31
    sub  t3, t2, t3
    add  t0, t0, t3

    addi t1, x0, 0              # ctz(0) = 32
    ctz  t2, t1
    addi t3, x0, 32
    sub  t3, t2, t3
    add  t0, t0, t3

    ################################################
    # cpop tests
    ################################################
    addi t1, x0, 0              # popcnt(0) = 0
    cpop t2, t1
    addi t3, x0, 0
    sub  t3, t2, t3
    add  t0, t0, t3

    addi t1, x0, -1             # 0xffffffff -> popcnt = 32
    cpop t2, t1
    addi t3, x0, 32
    sub  t3, t2, t3
    add  t0, t0, t3

    lui  t1, 0xF0F0F            # 0xF0F0F000
    addi t1, t1, 0xF0           # 0xF0F0F0F0 (4 bits set per byte -> 16 ones total)
    cpop t2, t1
    addi t3, x0, 16
    sub  t3, t2, t3
    add  t0, t0, t3

    ################################################
    # exit
    ################################################
    addi a0, t0, 0
    addi a7, x0, 93
    ecall
