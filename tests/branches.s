.section .text
.globl _start

_start:
    addi t0, x0, 0          # error accumulator

    addi t1, x0, 5
    addi t2, x0, 5

    # beq taken
    beq  t1, t2, L_beq_taken
    addi t0, t0, 1
L_beq_taken:

    # beq not taken
    addi t1, x0, 5
    addi t2, x0, 7
    beq  t1, t2, L_beq_not_taken_fail
    beq  x0, x0, L_beq_not_taken_done
L_beq_not_taken_fail:
    addi t0, t0, 1
L_beq_not_taken_done:

    # bne taken
    addi t1, x0, 5
    addi t2, x0, 7
    bne  t1, t2, L_bne_taken
    addi t0, t0, 1
L_bne_taken:

    # bne not taken
    addi t1, x0, 9
    addi t2, x0, 9
    bne  t1, t2, L_bne_not_taken_fail
    beq  x0, x0, L_bne_not_taken_done
L_bne_not_taken_fail:
    addi t0, t0, 1
L_bne_not_taken_done:

    # signed comparisons
    addi t1, x0, -1
    addi t2, x0, 1

    # blt: -1 < 1 -> taken
    blt  t1, t2, L_blt_taken
    addi t0, t0, 1
L_blt_taken:

    # blt: 1 < -1 -> not taken
    blt  t2, t1, L_blt_not_taken_fail
    beq  x0, x0, L_blt_not_taken_done
L_blt_not_taken_fail:
    addi t0, t0, 1
L_blt_not_taken_done:

    # bge: -1 >= 1 -> not taken
    bge  t1, t2, L_bge_not_taken_fail
    beq  x0, x0, L_bge_not_taken_done
L_bge_not_taken_fail:
    addi t0, t0, 1
L_bge_not_taken_done:

    # bge: 1 >= -1 -> taken
    bge  t2, t1, L_bge_taken
    addi t0, t0, 1
L_bge_taken:

    # unsigned comparisons (0xffffffff vs 1)
    # bltu: 1 < 0xffffffff -> taken
    bltu t2, t1, L_bltu_taken
    addi t0, t0, 1
L_bltu_taken:

    # bltu: 0xffffffff < 1 -> not taken
    bltu t1, t2, L_bltu_not_taken_fail
    beq  x0, x0, L_bltu_not_taken_done
L_bltu_not_taken_fail:
    addi t0, t0, 1
L_bltu_not_taken_done:

    # bgeu: 0xffffffff >= 1 -> taken
    bgeu t1, t2, L_bgeu_taken
    addi t0, t0, 1
L_bgeu_taken:

    # bgeu: 1 >= 0xffffffff -> not taken
    bgeu t2, t1, L_bgeu_not_taken_fail
    beq  x0, x0, L_bgeu_not_taken_done
L_bgeu_not_taken_fail:
    addi t0, t0, 1
L_bgeu_not_taken_done:

    addi a0, t0, 0
    addi a7, x0, 93
    ecall
