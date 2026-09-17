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



//main函数

int main()
{
    char line[64];
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
   
    console_init();

    shell_run();
   
    //我们现在是裸机程序,main后面没有操作系统帮我们接管程序,所以输出完成后让cpu一直停在这里
  while (1)
    {
       

            
        
    }
}

//寄存器的地址=设备基地址+寄存器偏移