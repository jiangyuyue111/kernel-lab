//加入UART中断,定义PLIC基地址
//platform-level interrupt负责管理UART,网卡等外部设备产生的中断
//QEMU virtual中UART对应PLIC interrupt soucrce
//#define PLIC_PRIORITY(id)(* (volatile unsigned int *)(PLIC_BASE + (id)*4))

extern void trap_entry(void);    //extern的意思是这个函数不在本文件定义，但链接的时候请去其他目标文件寻找
//调接口
#include "uart.h"
#include "plic.h"
#include "trap.h"
#include "console.h"
#include "string.h"
#include "mmu.h"
#include "page.h"
#include "heap.h"
#include "kernel.h"
//保存heap_init()的执行结果
#include "timer.h"

  



//main函数

int main()
{
    //初始化UART中断，允许UART自己接收中断
    uart_interrupt_init();

    //初始化PLIC,设置UART中断优先级，IRQ10 priority=1,IPQIC enable=1,threshold=0
    plic_init();

     //初始化中断，告诉cpu中断处理函数的入口地址,mtvec=trap_entry
    trap_init();


     //打开cpu中断，mie.MEIE=1,mstatus.MIE=1
    
    console_init();

    uart_puts("minios" );

    mmu_init();
    page_init();
//初始化内核堆，heap_init内部会调用alloc_page()
//从16个物理页中拿出一页4KB
    heap_init();
   //早期内核初始化完成后
   // 内存相关初始化函数已返回
    uart_puts("[BOOT] Memory initialization completed.\n");
   
    timer_init();
     //Timer初始化函数已返回
    uart_puts("[BOOT] Timer initialized.\n");
    enable_interrupt();
     // 即将从main返回
    uart_puts("[BOOT] Entering S-mode...\n");
    // 返回到riscv.S中的call main下一条指令
    // 即boot_enter_s，随后由mret进入kernel_main
    return 0;

   
  
}

//寄存器的地址=设备基地址+寄存器偏移