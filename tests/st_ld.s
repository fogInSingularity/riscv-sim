.section .data
word_data:
    .word 0
half_data:
    .half 0
    .half 0
byte_data:
    .byte 0
    .byte 0
    .byte 0
    .byte 0

.section .text
.globl _start
_start:
    addi t0, x0, 0          # error accumulator

    ################################################
    # LW / SW
    ################################################
    lui  t1, %hi(word_data)
    addi t1, t1, %lo(word_data)

    addi t2, x0, 1234       # value fits in 12-bit imm
    sw   t2, 0(t1)
    lw   t3, 0(t1)

    sub  t4, t3, t2
    add  t0, t0, t4

    ################################################
    # LH / SH / LHU
    ################################################
    lui  t1, %hi(half_data)
    addi t1, t1, %lo(half_data)

    addi t2, x0, -1234      # 16-bit negative value
    sh   t2, 0(t1)

    lh   t3, 0(t1)          # signed
    sub  t4, t3, t2         # should be 0 if sign-extended correctly
    add  t0, t0, t4

    lhu  t4, 0(t1)          # unsigned
    # For negative 16-bit N, lhu - lh = 2^16
    sub  t5, t4, t3         # should be 65536
    addi t6, x0, 1
    slli t6, t6, 16         # 1 << 16 = 65536
    sub  t6, t5, t6
    add  t0, t0, t6

    ################################################
    # LB / SB / LBU
    ################################################
    lui  t1, %hi(byte_data)
    addi t1, t1, %lo(byte_data)

    addi t2, x0, -5         # 8-bit negative
    sb   t2, 0(t1)

    lb   t3, 0(t1)          # signed -> -5
    sub  t4, t3, t2         # should be 0
    add  t0, t0, t4

    lbu  t4, 0(t1)          # unsigned -> 251
    # For negative 8-bit N, lbu - lb = 2^8
    sub  t5, t4, t3         # should be 256
    addi t6, x0, 1
    slli t6, t6, 8          # 1 << 8 = 256
    sub  t6, t5, t6
    add  t0, t0, t6

    ################################################
    # Exit
    ################################################
    addi a0, t0, 0
    addi a7, x0, 93
    ecall
