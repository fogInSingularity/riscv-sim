.section .data
.align 2

float_slot:
    .word 0

    .section .text
    .globl _start
_start:
    addi t0, x0, 0

    # frm = RNE, clear fflags
    csrrwi x0, frm, 0
    csrrwi x0, fflags, 0

    # fcvt round-trip: 42 -> float -> 42
    addi t1, x0, 42
    fcvt.s.w ft0, t1
    fcvt.w.s t2, ft0
    addi t3, x0, 42
    sub  t3, t2, t3
    add  t0, t0, t3

    # fadd.s: 1.0 + 2.0 = 3.0
    addi t1, x0, 1
    fcvt.s.w ft0, t1
    addi t1, x0, 2
    fcvt.s.w ft1, t1
    fadd.s ft2, ft0, ft1
    fcvt.w.s t2, ft2
    addi t3, x0, 3
    sub  t3, t2, t3
    add  t0, t0, t3

    # fmul.s: 2.0 * (-3.0) = -6.0
    addi t1, x0, 2
    fcvt.s.w ft0, t1
    addi t1, x0, -3
    fcvt.s.w ft1, t1
    fmul.s ft2, ft0, ft1
    fcvt.w.s t2, ft2
    addi t3, x0, -6
    sub  t3, t2, t3
    add  t0, t0, t3

    # fsgnj.s: sign from second operand -> -2.0
    addi t1, x0, 2
    fcvt.s.w ft0, t1        # +2.0
    addi t1, x0, -1
    fcvt.s.w ft1, t1        # -1.0
    fsgnj.s ft2, ft0, ft1   # -2.0
    fcvt.w.s t2, ft2
    addi t3, x0, -2
    sub  t3, t2, t3
    add  t0, t0, t3

    # feq.s: equal and not-equal
    addi t1, x0, 5
    fcvt.s.w ft0, t1
    fcvt.s.w ft1, t1
    feq.s t2, ft0, ft1      # 5.0 == 5.0 -> 1
    addi t3, x0, 1
    sub  t3, t2, t3
    add  t0, t0, t3

    addi t1, x0, 7
    fcvt.s.w ft1, t1
    feq.s t2, ft0, ft1      # 5.0 == 7.0 -> 0
    addi t3, x0, 0
    sub  t3, t2, t3
    add  t0, t0, t3

    # flt.s / fle.s
    # ft0 = 5.0, ft1 = 7.0
    flt.s t2, ft0, ft1      # 5.0 < 7.0 -> 1
    addi t3, x0, 1
    sub  t3, t2, t3
    add  t0, t0, t3

    fle.s t2, ft1, ft0      # 7.0 <= 5.0 -> 0
    addi t3, x0, 0
    sub  t3, t2, t3
    add  t0, t0, t3

    fle.s t2, ft0, ft0      # 5.0 <= 5.0 -> 1
    addi t3, x0, 1
    sub  t3, t2, t3
    add  t0, t0, t3

    # fsw/flw + fmv.x.w: bit pattern preserved
    lui  a0, %hi(float_slot)
    addi a0, a0, %lo(float_slot)

    addi t1, x0, 9
    fcvt.s.w ft0, t1
    fsw ft0, 0(a0)
    flw ft1, 0(a0)
    fmv.x.w t2, ft1
    fmv.x.w t3, ft0
    sub  t4, t2, t3
    add  t0, t0, t4

    addi a0, t0, 0
    addi a7, x0, 93
    ecall
