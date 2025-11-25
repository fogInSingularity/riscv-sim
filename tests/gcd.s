.section .text
.globl _start

_start:
    addi t0, x0, 0          # error accumulator

    addi s0, x0, 1071       # a
    addi s1, x0, 462        # b

gcd_loop:
    beq  s0, s1, gcd_done

    blt  s0, s1, gcd_a_less

    # a > b: a = a - b
    sub  s0, s0, s1
    jal  x0, gcd_loop

gcd_a_less:
    # b > a: b = b - a
    sub  s1, s1, s0
    jal  x0, gcd_loop

gcd_done:
    add  s2, x0, s0         # gcd in s2

    addi t1, x0, 21         # expected gcd
    sub  t1, s2, t1
    add  t0, t0, t1

    addi a0, t0, 0
    addi a7, x0, 93
    ecall
