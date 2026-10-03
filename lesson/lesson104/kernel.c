#include "kernel.h"
#include "uart.h"
#include "task.h"//引入任务初始化接口
void kernel_main(void)
{
    uart_puts("\n[Kernel]Entered S-mode kernel_main!\n");
    task_init();//初始化任务
    uart_puts("[TASK] PCB initialized.\n");
while(1)
{
    
}    
}
