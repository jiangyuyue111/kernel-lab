//最简单的UART驱动封装

//QEMU virt机器中,UART0的基地址是0x10000000(UART这组寄存器的起始地址)
//cpu在访问这个地址时,是在访问uart硬件寄存器
//这种用内存地址访问硬件的方式叫“MMIO(内存映射I/O)”
#define UART0 0x10000000UL
//把字符c写入这个硬件寄存器
//0x100000本身只是一个整数,需要*(volatile unsigned char *)把这个整数转为一个可以指向寄存器的 指针

#define UART_THR (*(volatile unsigned char *)(UART0+0))
//定义状态寄存器
#define UART_LSR (*(volatile unsigned char *)(UART0+5))
//定义状态
#define UART_LSR_THRE (1 << 5)
void uart_putc(char c)
{
    while ((UART_LSR & UART_LSR_THRE) == 0)
    {
    ;
    }
    UART_THR =(unsigned char)c;    
     
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
void main(void)
{
    uart_puts("Hello World!\r\n");
    //我们现在是裸机程序,main后面没有操作系统帮我们接管程序,所以输出完成后让cpu一直停在这里
    while(1);
}

//寄存器的地址=设备基地址+寄存器偏移