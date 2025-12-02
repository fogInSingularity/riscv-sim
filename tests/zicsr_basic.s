.section .text
.globl _start

_start:
    addi t0, x0, 0          # error accumulator

    # ---------- fflags with csrrw/csrrs/csrrc ----------

    li   t1, 0x1
    csrrw t2, fflags, t1    # fflags = 0x1

    csrrs t3, fflags, x0    # read
    li   t4, 0x1
    sub  t4, t3, t4
    add  t0, t0, t4

    li   t1, 0x14           # set bits 2 and 4
    csrrs t3, fflags, t1    # old should be 0x1
    li   t4, 0x1
    sub  t4, t3, t4
    add  t0, t0, t4

    csrrs t3, fflags, x0    # now expect 0x15
    li   t4, 0x15
    sub  t4, t3, t4
    add  t0, t0, t4

    li   t1, 0x1            # clear bit 0
    csrrc t3, fflags, t1    # old should be 0x15
    li   t4, 0x15
    sub  t4, t3, t4
    add  t0, t0, t4

    csrrs t3, fflags, x0    # now expect 0x14
    li   t4, 0x14
    sub  t4, t3, t4
    add  t0, t0, t4

    # ---------- frm with csrrwi/csrrsi/csrrci ----------

    csrrwi t3, frm, 3       # frm = 3
    csrrs  t4, frm, x0
    li     t5, 3
    sub    t5, t4, t5
    add    t0, t0, t5

    csrrwi x0, frm, 0       # frm = 0

    csrrsi t3, frm, 1       # frm = 1, old should be 0
    li     t4, 0
    sub    t4, t3, t4
    add    t0, t0, t4

    csrrs  t4, frm, x0
    li     t5, 1
    sub    t5, t4, t5
    add    t0, t0, t5

    csrrwi x0, frm, 7       # frm = 7
    csrrci t3, frm, 1       # clear bit 0 -> frm = 6, old 7
    li     t4, 7
    sub    t4, t3, t4
    add    t0, t0, t4

    csrrs  t4, frm, x0
    li     t5, 6
    sub    t5, t4, t5
    add    t0, t0, t5

    # ---------- fcsr composition (frm + fflags) ----------

    li   t1, 0x0B           # fflags = 0x0B
    csrrw x0, fflags, t1
    csrrwi x0, frm, 5       # frm = 5

    csrrs t3, fcsr, x0
    andi t3, t3, 0xFF       # low 8 bits: (5<<5) | 0x0B = 0xAB
    li   t4, 0xAB
    sub  t4, t3, t4
    add  t0, t0, t4

    li   t1, 0x55           # 0b0101_0101 -> frm=2, fflags=0x15
    csrrw x0, fcsr, t1

    csrrs t3, frm, x0
    li   t4, 2
    sub  t4, t3, t4
    add  t0, t0, t4

    csrrs t3, fflags, x0
    li   t4, 0x15
    sub  t4, t3, t4
    add  t0, t0, t4

    addi a0, t0, 0
    addi a7, x0, 93
    ecall
