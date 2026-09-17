//加入UART中断,定义PLIC基地址
//platform-level interrupt负责管理UART,网卡等外部设备产生的中断
//QEMU virtual中UART对应PLIC interrupt soucrce
//#define PLIC_PRIORITY(id)(* (volatile unsigned int *)(PLIC_BASE + (id)*4))

extern void trap_entry(void);    //extern的意思是这个函数不在本文件定义，但链接的时候请去其他目标文件寻找
#define UART0 0x10000000UL//UART
//0x100000本身只是一个整数,需要*(volatile unsigned char *)把这个整数转为一个可以指向寄存器的 指针

#define UART_THR (*(volatile unsigned char *)(UART0+0))//发送字符
#define UART_RHR (*(volatile unsigned char *)(UART0 + 0)) //接受寄存器
#define UART_IER (*(volatile unsigned char *)(UART0 + 1)) //中断使能寄存器，是否允许接受中断
#define UART_LSR (*(volatile unsigned char *)(UART0+5))//状态寄存器，用来查看当前状态
#define UART_LSR_THRE (1 << 5)      //发送器空闲

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

//PLIC
#define PLIC_BASE 0x0c000000UL
#define UART_IRQ 10//编号
#define PLIC_ENABLE (*(volatile unsigned int *)0x0c002000UL)//PLICenable寄存器
#define PLIC_THRESHOLD (*(volatile unsigned int *)0x0c200000UL)//threshold
#define PLIC_CLAIM (*(volatile unsigned int *)0x0c200004UL)//* 读取：claim-领取中断号写入；complete-告诉PLIC中断处理完成
//初始化PLIC
void plic_init(void)
{
    //设置IRQ10的优先级
    *(volatile unsigned int *)(PLIC_BASE+UART_IRQ*4)=1;
    //打开IRQE
    PLIC_ENABLE |= (1<<UART_IRQ);
    //允许所有中断threshold=0
    PLIC_THRESHOLD=0;
}
//打开UART接收器中断
void uart_interrupt_init(void)
{
    UART_IER |= 1U ;
}
//设置mtvec
void trap_init(void)
{
    unsigned long addr = (unsigned long)&trap_entry;//得到trao_entry地址
    asm volatile( 
        //然后把这个地址写入mtvec
        "csrw mtvec, %0"    //给mtvec一个地址，a1
        :
        :"r"(addr)    //让gcc找一个普通cpu寄存器，把trap_entry的地址放进去，假如是a1
    );
}
//打开cpu中断
void enable_interrupt(void)
{      //第一层mie.MEIE=1,允许外部中断
    asm volatile(
        "li t0,(1<<11)\n"
        "csrs mie,t0"
        :
        :
        :"t0"
    );
    //第二层mstatus.MIE=1,允许全局中断
    asm volatile(
        "li t0,(1<<3)\n"
        "csrs mstatus,t0"
        :
        :
        :"t0"
    );
}
//中断处理函数
void trap_handler(void)
{   //保存PLIC告诉我们的中断号
     unsigned int irq;
     //读取PLIC_CLAIM寄存器，得到中断号，看是哪个设备产生的
     irq = PLIC_CLAIM;
     //判断是否是UART中断
       if (irq == UART_IRQ)
    {   //读取UART_RHR寄存器，得到接收到的字符
        char c;
        c = UART_RHR;
        //把接收到的字符再发回去
        uart_putc(c);
    }
    //告诉PLIC中断处理完成
    if (irq != 0)
    {
        PLIC_CLAIM = irq;
    }
}
//main函数

int main()
{
    uart_puts(
        "Lsson100 uart Iterrupt\n"
    );
    //初始化中断，告诉cpu中断处理函数的入口地址,mtvec=trap_entry
    trap_init();
    //初始化PLIC,设置UART中断优先级，IRQ10 priority=1,IPQIC enable=1,threshold=0
    plic_init();
    //初始化UART中断，允许UART自己接收中断
    uart_interrupt_init();
    //打开cpu中断，mie.MEIE=1,mstatus.MIE=1
    enable_interrupt(); 
    //我们现在是裸机程序,main后面没有操作系统帮我们接管程序,所以输出完成后让cpu一直停在这里
    while(1);
}

//寄存器的地址=设备基地址+寄存器偏移