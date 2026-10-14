#include "trap.h"
#include "plic.h"
#include "uart.h"
#include "console.h"
#include "timer.h"
#include "scheduler.h"//为了访问current_task和schedule()
// 【新增】Machine Timer Interrupt的mcause值
#define MCAUSE_TIMER 0x8000000000000007UL

// 【新增】Machine External Interrupt的mcause值
#define MCAUSE_EXTERNAL 0x800000000000000BUL
//临时保存调度器选中的任务
struct task *timer_candidate = 0;
//task.c中定义的调度启动标志
extern volatile unsigned long scheduling_started;
extern void trap_entry(void);



void trap_init(void)
{
    unsigned long addr = (unsigned long)&trap_entry;//得到trao_entry地址
    asm volatile( 
        //然后把这个地址写入mtvec
        "csrw mtvec, %0"    //给mtvec一个地址，a1
        :
        :"r"(addr)    //让gcc找一个普通cpu寄存器，把trap_entry的地址放进去，假如是a1
    );
}
void enable_interrupt(void)
{      //第一层mie.MEIE=1,允许外部中断
    asm volatile(
        "li t0,(1<<11)\n"
        "csrs mie,t0"
        :
        :
        :"t0"
    );
    //第二层mstatus.MIE=1,允许全局中断
    asm volatile(
        "li t0,(1<<3)\n"
        "csrs mstatus,t0"
        :
        :
        :"t0"
    );
}

unsigned long trap_handler(unsigned long frame_sp)
{   //用来保存mcause寄存器的值，看看是哪个中断
    unsigned long cause;
     //保存PLIC告诉我们的中断号
     unsigned int irq;
//读取mcause寄存器，看看是哪个中断
    asm volatile(
        "csrr %0, mcause"
        : "=r"(cause)
    );


    if(cause == MCAUSE_TIMER)
    {
        timer_set_next();

        //只有当前任务存在，才进行任务调度
       if (scheduling_started == 0 || current_task == 0)
       {
        return frame_sp;
       
       }
       current_task->trap_sp = frame_sp;
        timer_candidate = schedule();
      
        if (timer_candidate != 0 &&
            timer_candidate != current_task &&
            timer_candidate->trap_sp != 0)
            {
                struct task *old_task = current_task;
                old_task->state = TASK_READY;
                timer_candidate->state = TASK_RUNNING;
                current_task = timer_candidate;
                return current_task->trap_sp;
            }
            return frame_sp;
    }


    if (cause == MCAUSE_EXTERNAL)
    {
     //读取PLIC_CLAIM寄存器，得到中断号，看是哪个设备产生的
     irq = plic_claim();
     //判断是否是UART中断
       if (irq == UART_IRQ)
    {   //读取UART_RHR寄存器，得到接收到的字符
        char c;
        c = uart_getc();
        //把接收到的字符再发回去
        if(c =='\b' || c == 127)
        {
            uart_putc('\b');
            uart_putc(' ');
            uart_putc('\b');
        }
        else
        {
            uart_putc(c);
        }
        //把接收到的字符传给console_put_char()，让它保存到输入缓冲区
        console_put_char(c);
    }
    else if(irq == 0)
    {
        uart_puts("unexpected interrupt!\n");
    }
    
    //告诉PLIC中断处理完成
    if (irq != 0)
    {
        plic_complete (irq);
    }
    return frame_sp;

}
    uart_puts("[TRAP] unhandled trap!\n");
    while (1)
    {

    }
}