asm(".global main");

#define UART0 0x10000000

void uart_putc(char c)
{
    *(volatile unsigned char *)UART0 = c;
}

void uart_puts(const char *s)
{
    while (*s!='\0')
    {
        uart_putc(*s);
        s++;
    }
}

void main(void)
{
    uart_puts("Hello World!\r\n");
    while(1);
}