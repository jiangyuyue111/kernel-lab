.global _start
_start:
auipc   a0,%hi(sbss)
addi   a0,a0,%lo(ebss)

auipc a1,%hi(ebss)
addi a1,a1,%lo(sbss)

bss_clear:
bgeu  a0,a1,bss_done
sd  zero,0(a0)
addi  a0,a0,8
j  bss_clear
bss_done:
jal  ra,main
loop:
    j  loop
