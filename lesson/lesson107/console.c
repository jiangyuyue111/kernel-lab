#include "console.h"
#define INPUT_BUF_SIZE 64
static char input_buf[INPUT_BUF_SIZE];
static int input_len = 0;
static int line_ready = 0;
void console_init(void)
{
    input_len = 0;
    line_ready = 0;
}
void console_put_char(char c)
{
    if(line_ready)
    {
        return;
    }
    if(c == '\r'|| c =='\n')
    {
        input_buf[input_len]='\0';
        line_ready = 1;
        return;
    }
    if(c == '\b' || c == 127)
    {
        if(input_len > 0)
        {
            input_len--;
        }
        return;
    }   
    if(input_len < INPUT_BUF_SIZE -1)
    {
        input_buf[input_len]=c;
        input_len++;
    }
}
int console_readline(char *buf,int max_len)
{
    int i;
    if(!line_ready)
    {
        return 0;
    }
    for(i=0;i<input_len && i<max_len-1;i++)
    {
        buf[i]=input_buf[i];
    }
    buf[i]='\0';
    input_len=0;
    line_ready=0;
    return 1;
}