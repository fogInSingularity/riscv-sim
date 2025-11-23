.section .text
.global _start

_start:
    # Manually set a0 to 0 to indicate success.
    # We use 'addi' because it is the most fundamental I-type instruction.
    # x10 = x0 + 0
    # addi x10, x0, 0
    
    # Halt the simulator
    ebreak
