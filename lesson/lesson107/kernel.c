#include "kernel.h"
#include "uart.h"
#include "task.h"//引入任务初始化接口
#include "scheduler.h"
// 保存kernel_main被切换出去时的CPU现场
static struct context kernel_context;
void kernel_main(void)
{
    uart_puts("\n[Kernel]Entered S-mode kernel_main!\n");
    task_init();//创建任务AB，配置各自的栈和初始入口
    uart_puts("[TASK] PCB initialized.\n");
    if (task_a.state != TASK_READY || task_b.state != TASK_READY)
    {
        uart_puts("[TASK] PCB state error!\n");
        while (1)
        {
        };
    }
    //接下来运行A
    task_a.state = TASK_RUNNING;
    //等级即将启动的第一个任务A
    current_task = &task_a;
    //让调度器试着选择下一位
    struct task *next = schedule();
    //检查调度结果
    if (next == &task_b)
    {
        uart_puts("[SCHED] next task is B\n");
    }
    uart_puts("[Kernel] Switching to task A...\n");
    //保存当前内核现场，并恢复任务A的现场
    context_switch(
         &kernel_context,
         &task_a.context
    );
    //只有将来切换回kenel_context时，才会执行到这里
     uart_puts("[Kernel] Returned from task A.\n");

    while(1)
    {
    
    }    
}
