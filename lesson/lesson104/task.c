#include "task.h"//引入任务结构体定义
#include "page.h"//引入页表接口
//定义任务A的控制块
struct task task_a;
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
    (unsigned long)task_a.stack_base + PAGE_SIZE;(unsigned long)task_a.stack_base + PAGE_SIZE;
task_b.context.sp =
    (unsigned long)task_b.stack_base + PAGE_SIZE;
}