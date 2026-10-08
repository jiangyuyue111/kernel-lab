#include "timer.h"
#define CLINT_MTIMECMP 0x2004000UL//系统往这里写下一次闹钟时间
#define CLINT_MTIME 0x200bff8UL//系统往这里读取当前时间
#define MTIME_INTERVAL 10000000UL//每次定时器中断的时间间隔
void timer_set_next(void)
{
    //把硬件地址转换成指针
    volatile unsigned long *mtime=
        (volatile unsigned long *)CLINT_MTIME;
    volatile unsigned long *mtimecmp=
        (volatile unsigned long *)CLINT_MTIMECMP;
    //当前时间+一个时间间隔+下一次闹钟时间
    *mtimecmp=*mtime + MTIME_INTERVAL;
}
void timer_init(void)
{
    //第一步，先把闹钟设置到未来
    timer_set_next();
    //第二步，打开mie.MTIE,也就i是bit7
    unsigned long mtie = (1UL << 7);
    //cers:只把指定的位设置成为1，不破坏mie中原来已存在的MTIE位的值
    __asm__ volatile(
        "csrs mie,%0"
        :
        : "r"(mtie)
    );
}
