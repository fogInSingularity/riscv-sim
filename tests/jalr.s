.section .text
.globl _start

_start:
    addi t0, x0, 0          # error accumulator
    addi s0, x0, 0          # marker: set in func if we really jumped

    lui  t1, %hi(func)
    addi t1, t1, %lo(func)

    jalr ra, t1, 0          # call func

    addi t2, x0, 1
    sub  t2, s0, t2         # t2 = s0 - 1  (0 if s0 == 1)
    add  t0, t0, t2         # accumulate error

    addi a0, t0, 0          # exit code: 0 = success
    addi a7, x0, 93         # Linux bare-metal exit syscall
    ecall

func:
    addi s0, x0, 1          # mark that we reached the function
    jalr x0, ra, 0          # return

