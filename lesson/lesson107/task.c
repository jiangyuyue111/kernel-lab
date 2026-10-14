#include "task.h"//引入任务结构体定义
#include "task.h"//任务控制块定义
#include "page.h"//物理页分配接口
#include "uart.h"//串口输出接口
#include "trap_frame.h"
//定义任务A的控制块
struct task task_a;
struct task task_b;
volatile unsigned long scheduling_started = 0;

//任务A的入口函数
//staic表示这个函数只在本文件中可见，其他文件无法调用
static void task_a_entry(void)
{
    //证明任务A已经开始执行
    uart_puts("[TASK A] started!\n");
    scheduling_started = 1;
    //A不主动切换，等待Timer抢占
    while(1)
    {

    }
}
static void task_b_entry(void)
{
    uart_puts("[TASK B] started!\n");
    while(1)
    {

    }
}


//定义任务B的控制块
struct task task_b;
//初始化两个任务，当前值建立任务档案，不分配独立栈。不执行任务切换
void task_init(void)
{
    task_a.id = 1;//A的编号
    task_a.state = TASK_CREATED;//A已创建
    task_b.id = 2;//B的编号
    task_b.state = TASK_CREATED;//B已创建
//给任务分配一个4KB的栈空间，分配失败则返回
    task_a.stack_base = alloc_page();//返回栈页的起始地址
    if(task_a.stack_base == 0)
    {
        return;//如果没有空闲页则停止初始化，提前返回
    }
    task_b.stack_base = alloc_page();
    //如果B的任务栈分配失败，则释放A的栈页，避免内存泄漏
    if(task_b.stack_base == 0)
    {
        free_page(task_a.stack_base);//释放刚分配给B的栈页，避免内存泄漏
        task_a.stack_base = 0;//清除已经失效的栈页地址
        return;//停止初始化
    }
//设置任务A和任务B的栈指针，栈指针指向栈顶
task_a.context.sp =
    (unsigned long)task_a.stack_base + PAGE_SIZE;
task_b.context.sp =
    (unsigned long)task_b.stack_base + PAGE_SIZE;
//把任务A入口函数的地址保存到A的context.ra
task_a.context.ra = (unsigned long)task_a_entry;
task_b.context.ra = (unsigned long)task_b_entry;
//任务A现在具备初始化运行条件
task_a.state = TASK_READY;
task_b.state = TASK_READY;
// A还没有保存过Timer中断现场
task_a.trap_sp = 0;

// B还没有保存过Timer中断现场
task_b.trap_sp = 0;




 // ---------- 构造B的初始Trap Frame ----------

 // 计算任务B的栈顶地址
    unsigned long b_top =
        (unsigned long)task_b.stack_base + PAGE_SIZE;

    // 从B栈顶往下预留256字节
    // frame指向这块内存
    struct trap_frame *frame =
        (struct trap_frame *)(b_top - sizeof(struct trap_frame));
      // 将frame转换成unsigned long指针
    // 方便每次操作8字节
    unsigned long *words = (unsigned long *)frame;

    // 遍历整个Trap Frame
    for (unsigned long i = 0;
         i < sizeof(struct trap_frame) / sizeof(unsigned long);
         i++)
    {
        // 把对应8字节清零
        words[i] = 0;
    }


    // ---------- 7. 初始化gp、tp寄存器值 ----------

    // 定义两个变量保存寄存器内容
    unsigned long gp_value;
    unsigned long tp_value;

    // 读取当前CPU的gp
    asm volatile("mv %0, gp" : "=r"(gp_value));

    // 读取当前CPU的tp
    asm volatile("mv %0, tp" : "=r"(tp_value));

    // 将gp写入B的Trap Frame
    frame->gp = gp_value;

    // 将tp写入B的Trap Frame
    frame->tp = tp_value;
    // ---------- 8. 初始化B的执行地址 ----------

    // B第一次运行，从task_b_entry的入口开始执行
    frame->mepc = (unsigned long)task_b_entry;


    // ---------- 9. 初始化B的Trap返回权限状态 ----------

    // bit11设置MPP为S-mode
    // bit7设置MPIE为1
    frame->mstatus = (1UL << 11) | (1UL << 7);


    // ---------- 10. 让B的PCB记录现场地址 ----------

    // 将Trap Frame的起始地址保存到PCB
    task_b.trap_sp = (unsigned long)frame;
}
