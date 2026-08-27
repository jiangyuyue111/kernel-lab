    .option nopic
    .section .text
    .global _start
_start:
    la a0, sbss
    la a1, ebss

bss_clear:
    bgeu    a0, a1, bss_done
    sd      zero, 0(a0)
    addi    a0, a0, 8
    j       bss_clear

bss_done:
    la      sp, stack_top
    jal     ra, main
loop:
    j       loop
