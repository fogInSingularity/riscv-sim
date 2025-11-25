.section .text
.globl _start

_start:
    addi t0, x0, 0          # error accumulator

    addi s0, x0, 10         # n = 10
    addi s1, x0, 0          # a = 0
    addi s2, x0, 1          # b = 1
    addi s3, x0, 1          # i = 1 (we are at fib(1) = 1)

    # handle n <= 1 fast path (not strictly needed for n=10, but good test)
    addi t1, x0, 1
    blt  s0, t1, fib_iter_done_n0_or_n1   # if n < 1
    beq  s0, t1, fib_iter_done_n1         # if n == 1

fib_iter_loop:
    add  t1, s1, s2        # t1 = a + b
    add  s1, x0, s2        # a = b
    add  s2, x0, t1        # b = t1
    addi s3, s3, 1         # i++

    blt  s3, s0, fib_iter_loop

    # result for n>=2 is in s2
    add  t1, x0, s2
    jal  x0, fib_iter_check

fib_iter_done_n0_or_n1:
    # n = 0 -> fib = 0
    addi t1, x0, 0
    jal  x0, fib_iter_check

fib_iter_done_n1:
    # n = 1 -> fib = 1
    addi t1, x0, 1

fib_iter_check:
    # t1 holds fib(n), n=10 in our main case -> expect 55
    addi t2, x0, 55
    sub  t2, t1, t2
    add  t0, t0, t2

    addi a0, t0, 0
    addi a7, x0, 93
    ecall
