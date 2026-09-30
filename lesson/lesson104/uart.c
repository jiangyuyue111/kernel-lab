#include "uart.h"
#define UART0 0x10000000UL


#define UART_THR (*(volatile unsigned char *)(UART0+0))//发送寄存器

#define UART_IER (*(volatile unsigned char *)(UART0 + 1)) //中断使能寄存器，是否允许接受中断

#define UART_RHR (*(volatile unsigned char*)(UART0+0))//接受寄存器
#define UART_LSR (*(volatile unsigned char *)(UART0+5))//状态寄存器，用来查看当前状态
#define UART_LSR_THRE (1 << 5)      //发送器空闲
#define UART_LSR_DR (1 << 0)//接受数据就绪标志
void uart_putc(char c)
{
    while ((UART_LSR & UART_LSR_THRE) == 0)//等待发送器空闲
    {
    ;  
    }
    UART_THR =(unsigned char)c;  //把字符写入发送寄存器  
     
}
//不能uart_putc('a')一直写，所以在封装一层
void uart_puts(const char *s)
{
    while (*s!='\0')
    {
        uart_putc(*s);
        s++;
    }
}
//\r = carriage return,回到行首
//\n = newwline,切换到下一行
void uart_interrupt_init(void)
{
    UART_IER |= 1U ;
}
//设置mtvec

char uart_getc(void)
{
    // 循环等待：直到“接收数据就绪”位为1
    while ((UART_LSR & UART_LSR_DR) == 0);
    
    // 读接收寄存器，返回读到的字符
    return UART_RHR;
}

