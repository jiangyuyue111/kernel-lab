#include "plic.h"

#define PLIC_BASE       0x0c000000UL

// 寄存器地址：全部用 基地址+偏移 计算，统一规范
#define PLIC_PRIORITY(id)   (*(volatile unsigned int *)(PLIC_BASE + (id) * 4))
#define PLIC_ENABLE         (*(volatile unsigned int *)(PLIC_BASE + 0x2000))
#define PLIC_THRESHOLD      (*(volatile unsigned int *)(PLIC_BASE + 0x200000))
#define PLIC_CLAIM_COMPLETE (*(volatile unsigned int *)(PLIC_BASE + 0x200004))

// 初始化PLIC
void plic_init(void)
{
    // 1. 设置UART中断(IRQ10)的优先级为1（0=屏蔽，1~7有效）
    PLIC_PRIORITY(UART_IRQ) = 1;

    // 2. 打开IRQ10的使能开关，允许该中断转发给CPU
    PLIC_ENABLE |= (1 << UART_IRQ);

    // 3. 中断阈值设为0：所有优先级>0的中断都放行
    PLIC_THRESHOLD = 0;
}

// 认领中断：读取寄存器，返回当前触发的中断号
unsigned int plic_claim(void)
{
    return PLIC_CLAIM_COMPLETE;
}

// 完成中断：把中断号写回寄存器，通知PLIC处理完毕
void plic_complete(unsigned int irq)
{
    PLIC_CLAIM_COMPLETE = irq;
}
// 从串口读取一个字符，没有数据就一直等
