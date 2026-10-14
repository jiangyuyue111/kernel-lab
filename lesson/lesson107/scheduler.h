#ifndef SCHEDULER_H//如果还没有定义
#define SCHEDULER_H//就定义它，防止重复包含
//引入结构体定义
#include "task.h"
//当前正在运行的任务指针
//真正变量定义在scheduler.c
extern struct task *current_task;//exter是声明，这个变量存在别的.c文件中，请编译器允许我使用它
//根据当前任务，选择下一位任务
//返回值：下一个任务的地址
struct task *schedule(void);
#endif//结束最开始的#ifndef