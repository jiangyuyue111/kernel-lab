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
int debug_bad_kfree = -99;
int debug_kfree_result = -99;
//保存heap_init()的执行结果
int debug_heap_result = -99;
void *debug_heap4 = 0;
void *debug_heap1 = 0;
void *debug_heap2 = 0;
void *debug_heap3 = 0;

static void s_mode_test(void)
{
  //如果进入S-MODE后还能执行这句话，说明内核代码的页表能够工作
  uart_puts("\n[S-mode] Sv39 MMU works!\n");
  //测试完成后停在这里
  while (1)
  {

  }
}

  static void enter_s_mode(void (*entry)(void))
{
    unsigned long mstatus_value;


    unsigned long pmpaddr =
        0x3FFFFFFFFFFFFFUL;

    unsigned long pmpcfg =
        0xFUL;

    
    asm volatile(
        "csrw pmpaddr0, %0"
        :
        : "r"(pmpaddr)
        : "memory"
    );

    
    asm volatile(
        "csrw pmpcfg0, %0"
        :
        : "r"(pmpcfg)
        : "memory"
    );

    asm volatile(
        "csrr %0, mstatus"
        : "=r"(mstatus_value)
    );



    mstatus_value &= ~(3UL << 11);

    // 再设置成 01
    mstatus_value |= (1UL << 11);

    // 写回 mstatus
    asm volatile(
        "csrw mstatus, %0"
        :
        : "r"(mstatus_value)
        : "memory"
    );

    asm volatile(
        "csrw mepc, %0"
        :
        : "r"((unsigned long)entry)
        : "memory"
    );

    asm volatile("mret");
    while (1)
    {
    }
  }




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
    enable_interrupt();
    console_init();

    uart_puts("minios" );

    mmu_init();
    page_init();
//初始化内核堆，heap_init内部会调用alloc_page()
//从16个物理页中拿出一页4KB
    debug_heap_result = heap_init();
    debug_heap1 = kmalloc(8);
    debug_heap2 = kmalloc(40);
    debug_heap3 = kmalloc(70);
    //释放第二笔分配
    debug_kfree_result = kfree(debug_heap2);
    // 释放第2块后，再申请50B
    debug_heap4 = kmalloc(50);

    debug_bad_kfree =
        kfree(
            (void *)(
                (unsigned long)debug_heap3
                + 32UL
             )
         );


  enter_s_mode(s_mode_test);

    //我们现在是裸机程序,main后面没有操作系统帮我们接管程序,所以输出完成后让cpu一直停在这里
  while (1)
    {




    }
}

//寄存器的地址=设备基地址+寄存器偏移