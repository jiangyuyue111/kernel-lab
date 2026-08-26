    .section .text
    .global _start
_start:
    la a0, sbss
    la a1, ebss

bss_loop:
    bgeu a0, a1, bss_done
    sd zero, 0(a0)
    addi a0, a0, 8
    j bss_loop

bss_done:
    jal ra, main
    j .