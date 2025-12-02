.section .text
.globl _start

_start:
    addi t0, x0, 0

    # Build ft0 = 1.5f as 3.0 / 2.0 (exact in binary)
    addi t1, x0, 3
    fcvt.s.w ft0, t1
    addi t2, x0, 2
    fcvt.s.w ft1, t2
    fdiv.s   ft0, ft0, ft1   # 1.5

    # -------- RNE (frm = 0) --------
    csrrwi x0, frm,    0
    csrrwi x0, fflags, 0

    fcvt.w.s t3, ft0         # dyn rounding -> RNE
    addi t4, x0, 2           # expect 2
    sub  t4, t3, t4
    add  t0, t0, t4

    csrrs t5, fflags, x0     # expect NX=1, others 0 -> 0x1
    li   t6, 1
    sub  t6, t5, t6
    add  t0, t0, t6

    # -------- RTZ (frm = 1) --------
    csrrwi x0, fflags, 0
    csrrwi x0, frm,    1

    fcvt.w.s t3, ft0         # dyn rounding -> RTZ
    addi t4, x0, 1           # expect 1
    sub  t4, t3, t4
    add  t0, t0, t4

    csrrs t5, fflags, x0     # NX still 1
    li   t6, 1
    sub  t6, t5, t6
    add  t0, t0, t6

    addi a0, t0, 0
    addi a7, x0, 93
    ecall
