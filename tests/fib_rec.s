.section .bss
.align 4

stack_area:
    .space 256
stack_top:

    .section .text
    .globl _start
_start:
    addi t0, x0, 0          # error accumulator

    lui  sp, %hi(stack_top)
    addi sp, sp, %lo(stack_top)

    addi a0, x0, 10         # n = 10
    jal  ra, fib

    addi t1, x0, 55         # expected = 55
    sub  t1, a0, t1         # a0 - expected
    add  t0, t0, t1

    addi a0, t0, 0
    addi a7, x0, 93
    ecall

# int fib(int n) in a0, returns in a0
fib:
    addi sp, sp, -16
    sw   ra, 12(sp)
    sw   a0, 8(sp)

    addi t1, x0, 1
    blt  t1, a0, fib_recurse   # if 1 < n, recurse

    # base case: n <= 1, a0 already holds n
    lw   ra, 12(sp)
    addi sp, sp, 16
    jalr x0, ra, 0

fib_recurse:
    # compute fib(n-1)
    addi a0, a0, -1
    jal  ra, fib
    sw   a0, 4(sp)          # store fib(n-1)

    # compute fib(n-2)
    lw   a0, 8(sp)          # reload original n
    addi a0, a0, -2
    jal  ra, fib

    lw   t2, 4(sp)          # fib(n-1)
    add  a0, a0, t2         # fib(n) = fib(n-1) + fib(n-2)

    lw   ra, 12(sp)
    addi sp, sp, 16
    jalr x0, ra, 0
