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
    #BSS清零完成
    #调用C语言的main（）
    #jal会同时把返回地址保存到ra
    jal ra, main
halt:
    #如果main函数返回，就永远停在这里
    j halt
