#ifndef CONSOLE_H
#define CONSOLE_H

void console_init(void);
void console_put_char(char c);
int console_readline(char *buf,int max_len);
#endif
 