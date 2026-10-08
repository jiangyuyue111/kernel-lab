#ifndef TASK_H
#define TASK_H
//人物尚未使用
#define TASK_UNUSED 0
//任务控制模块已经建立
#define TASK_CREATED 1
//任务已就绪
#define TASK_READY 2
//任务正在cpu上运行
#define TASK_RUNNING 3
//cpu上下文结构体
struct context
{
     unsigned long ra;  // 返回地址
    unsigned long sp;  // 栈指针

    unsigned long s0;  // 保存寄存器s0
    unsigned long s1;  // 保存寄存器s1
    unsigned long s2;  // 保存寄存器s2
    unsigned long s3;  // 保存寄存器s3
    unsigned long s4;  // 保存寄存器s4
    unsigned long s5;  // 保存寄存器s5
    unsigned long s6;  // 保存寄存器s6
    unsigned long s7;  // 保存寄存器s7
    unsigned long s8;  // 保存寄存器s8
    unsigned long s9;  // 保存寄存器s9
    unsigned long s10; // 保存寄存器s10
    unsigned long s11; // 保存寄存器s11
};
//任务控制模块
struct task
{
    int id;//任务编号
    int state;//当前状态
    void *stack_base;//任务栈页的起始地址
    struct context context;//保存cpu上下文
};
//声明两个任务控制块
extern struct task task_a;
extern struct task task_b;
//声明任务初始化函数
void task_init(void);
#endif
//cpu上下文切换函数声明
void context_switch(
    struct context *old_ctx,//当前任务上下文保存位置，旧任务的地址
    struct context *new_ctx//即将恢复的任务上下文位置
);