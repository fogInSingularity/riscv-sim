.section .text
.globl _start

_start:
addi t0, x0, 0

# 7 / 2 = 3
addi t1, x0, 7
addi t2, x0, 2
div  t3, t1, t2
addi t4, x0, 3
sub  t4, t3, t4
add  t0, t0, t4

# 7 % 2 = 1
rem  t3, t1, t2
addi t4, x0, 1
sub  t4, t3, t4
add  t0, t0, t4

# -7 / 2 = -3
addi t1, x0, -7
addi t2, x0, 2
div  t3, t1, t2
addi t4, x0, -3
sub  t4, t3, t4
add  t0, t0, t4

# -7 % 2 = -1 (round toward zero)
rem  t3, t1, t2
addi t4, x0, -1
sub  t4, t3, t4
add  t0, t0, t4

# division by zero: 5 / 0 -> -1, 5 % 0 -> 5
addi t1, x0, 5
addi t2, x0, 0
div  t3, t1, t2
addi t4, x0, -1
sub  t4, t3, t4
add  t0, t0, t4

rem  t3, t1, t2
addi t4, x0, 5
sub  t4, t3, t4
add  t0, t0, t4

# overflow: min_int / -1 -> min_int, remainder 0
lui  t1, 0x80000       # 0x80000000
addi t2, x0, -1
div  t3, t1, t2
sub  t4, t3, t1        # expect 0
add  t0, t0, t4

rem  t3, t1, t2
addi t4, x0, 0
sub  t4, t3, t4
add  t0, t0, t4

addi a0, t0, 0
addi a7, x0, 93
ecall

