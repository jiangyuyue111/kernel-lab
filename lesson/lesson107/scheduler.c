#include "scheduler.h"
//创建current_task指针
// //初始化位0，表示尚未指定运行任务
struct task *current_task = 0;
//选择下一个任务
struct task *schedule(void)//调用schedule，返回一个地址，赋值给task,task指向任务
{
    //情况1：当前运行A
    if (current_task == &task_a)
    {
        //如果B已经可以运行
         if (task_b.state == TASK_READY)
        {
            //告诉调用者：下一位选择B
            return &task_b;
        }
    }
     else if (current_task == &task_b)
    {
        //当前是B，需检查A是否就绪
        if (task_a.state == TASK_READY)
        {
            return &task_a;
        }
    }
    //没有找到可切换的任务，就继续当前任务
    return current_task;
}