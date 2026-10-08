    #告诉汇编器不要按照PIC的某些方式处理，因为裸机没有linux对应的加载etf功能
    .option nopic   #等价于Makefile里的-mcmodel=medany
    .section .text    #告诉汇编机器：下面内容放入.tex代码段
    .global _start    #把_start声明为全局符号，连接器可以找到它
_start:
    #初始化栈指针
    #stack_top由linker.ld定义
    # la = load address,把stack_lop的地址装入sp
    la sp, stack_top
    #准备清空BSS
    #a0保存BSS起始地址
    #a1保存BSS结束地址
    la a0, sbss
    la a1, ebss

bss_loop:
    #如果 a0 >=a1,说明已经清到BSS末尾
    #跳转到bss_done
    bgeu a0, a1, bss_done
    #把zero寄存器中的0写入a0指向的内存
    #sd = store doubleworld,一次性写入8个字节
    sd zero, 0(a0)
    #向后移8个字节
    #准备清理下一块内存
    addi a0, a0, 8
    #回到循环开头
    j bss_loop

bss_done:
   call main
.global boot_enter_s
boot_enter_s:
li t0, -1
srli t0, t0, 10
csrw pmpaddr0, t0
li t0, 0x0f
csrw pmpcfg0, t0
la t0, kernel_main
csrw mepc, t0
csrr t0, mstatus
li t1, 0x1800
not t1, t1
and t0, t0, t1
li t1, 0x800
or t0, t0, t1
csrw mstatus, t0
mret

boot_halt:
    #如果main函数返回，就永远停在这里
    j boot_halt
.section .text
.align 2    #让trap_entry放在满足指令地址对齐要求的位置
.global trap_entry

trap_entry:
#在栈上创建256字节trao frame
    addi sp, sp, -256
    #保存通用寄存器，ra= return aaddress
    sd ra, 0(sp)
    sd gp, 8(sp)
    sd tp, 16(sp)


    #临时寄存器
    sd t0, 24(sp)
    sd t1, 32(sp)
    sd t2, 40(sp)
    #saved regisers
    sd s0, 48(sp)
    sd s1, 56(sp)


    #函数参数/返回值寄存器
    sd a0, 64(sp)
    sd a1, 72(sp)
    sd a2, 80(sp)
    sd a3, 88(sp)
    sd a4, 96(sp)
    sd a5, 104(sp)
    sd a6, 112(sp)
    sd a7, 120(sp)


    # saved registers
    sd s2, 128(sp)
    sd s3, 136(sp)
    sd s4, 144(sp)
    sd s5, 152(sp)
    sd s6, 160(sp)
    sd s7, 168(sp)
    sd s8, 176(sp)
    sd s9, 184(sp)
    sd s10, 192(sp)
    sd s11, 200(sp)


    # 临时寄存器
    sd t3, 208(sp)
    sd t4, 216(sp)
    sd t5, 224(sp)
    sd t6, 232(sp)

    #调用c的trap handler
    call trap_handler
    #恢复现场
     ld ra,   0(sp)
    ld gp,   8(sp)
    ld tp,  16(sp)

    ld t0,  24(sp)
    ld t1,  32(sp)
    ld t2,  40(sp)

    ld s0,  48(sp)
    ld s1,  56(sp)

    ld a0,  64(sp)
    ld a1,  72(sp)
    ld a2,  80(sp)
    ld a3,  88(sp)
    ld a4,  96(sp)
    ld a5, 104(sp)
    ld a6, 112(sp)
    ld a7, 120(sp)

    ld s2, 128(sp)
    ld s3, 136(sp)
    ld s4, 144(sp)
    ld s5, 152(sp)
    ld s6, 160(sp)
    ld s7, 168(sp)
    ld s8, 176(sp)
    ld s9, 184(sp)
    ld s10,192(sp)
    ld s11,200(sp)

    ld t3, 208(sp)
    ld t4, 216(sp)
    ld t5, 224(sp)
    ld t6, 232(sp)

    #删除trao frame，恢复到原来的sp
    addi sp, sp, 256
    #从machine trap 返回值寄存器
    mret



    .section .text              # 放入代码段

    .align 2                   # 入口按4字节边界对齐

    .global context_switch      # 允许C代码引用此符号

    .type context_switch, @function  # 告诉工具这是函数


    context_switch:

    # -------------------------------------
    # 第一阶段：保存当前任务的CPU寄存器
    #
    # a0 = old_ctx
    # 即当前任务context的起始地址
    # -------------------------------------

    sd ra,   0(a0)              # 保存旧任务ra，偏移0
    sd sp,   8(a0)              # 保存旧任务sp，偏移8

    sd s0,  16(a0)              # 保存旧任务s0
    sd s1,  24(a0)              # 保存旧任务s1
    sd s2,  32(a0)              # 保存旧任务s2
    sd s3,  40(a0)              # 保存旧任务s3
    sd s4,  48(a0)              # 保存旧任务s4
    sd s5,  56(a0)              # 保存旧任务s5
    sd s6,  64(a0)              # 保存旧任务s6
    sd s7,  72(a0)              # 保存旧任务s7
    sd s8,  80(a0)              # 保存旧任务s8
    sd s9,  88(a0)              # 保存旧任务s9
    sd s10, 96(a0)              # 保存旧任务s10
    sd s11,104(a0)              # 保存旧任务s11


    # -------------------------------------
    # 第二阶段：恢复目标任务的CPU寄存器
    #
    # a1 = new_ctx
    # 即目标任务context的起始地址
    # -------------------------------------

    ld ra,   0(a1)              # 恢复新任务ra
    ld sp,   8(a1)              # 恢复新任务sp

    ld s0,  16(a1)              # 恢复新任务s0
    ld s1,  24(a1)              # 恢复新任务s1
    ld s2,  32(a1)              # 恢复新任务s2
    ld s3,  40(a1)              # 恢复新任务s3
    ld s4,  48(a1)              # 恢复新任务s4
    ld s5,  56(a1)              # 恢复新任务s5
    ld s6,  64(a1)              # 恢复新任务s6
    ld s7,  72(a1)              # 恢复新任务s7
    ld s8,  80(a1)              # 恢复新任务s8
    ld s9,  88(a1)              # 恢复新任务s9
    ld s10, 96(a1)              # 恢复新任务s10
    ld s11,104(a1)              # 恢复新任务s11


    # -------------------------------------
    # 第三阶段：进入目标任务
    # -------------------------------------

    ret                          # 跳到恢复后的ra地址


    .size context_switch, .-context_switch
