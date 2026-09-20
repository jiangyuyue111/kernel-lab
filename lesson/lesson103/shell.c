#include "shell.h"      // 声明 shell_run()
#include "uart.h"       // 使用 uart_puts() 输出字符
#include "console.h"    // 使用 console_readline() 获取用户输入
#include "string.h"     // 使用我们自己实现的 strcmp()



static void cmd_hello(const char *argument)
{
    // hello 不需要使用 argument。
    //
    // 写这一行的作用只是告诉编译器：
    // “这个参数我知道存在，只是暂时不用。”
    (void)argument;

    // 输出 hello 命令执行结果
    uart_puts("Hello RISC-V\n");
}

static void cmd_help(const char *argument)
{
    // help 目前也不需要参数
    (void)argument;

    uart_puts("Commands:\n");
    uart_puts("hello\n");
    uart_puts("help\n");
    uart_puts("echo <text>\n");
}



static void cmd_echo(const char *argument)
{
    // argument == 0
    // 后面没有参数。
    if (argument == 0)
    {
        // 没有内容就只输出一个换行
        uart_puts("\n");

        // 函数结束
        return;
    }

    // 有参数，例如：
    // argument -> "hello"
    // 就直接打印它
    uart_puts(argument);

    // 打印结束后换行
    uart_puts("\n");
}

struct command
{
    // 命令名称
    //
    // 例如：
    // name -> "hello"
    const char *name;


   
    void (*func)(const char *argument);
};



static struct command cmd_table[] =
{
    { "hello", cmd_hello },
    { "help",  cmd_help  },
    { "echo",  cmd_echo  }
};

static void parse_command(
    char *line,
    char **command,
    char **argument)
{
    // i 用来一个字符一个字符检查 line
    int i = 0;


   
    while (line[i] != '\0' && line[i] != ' ')
    {
        // 查看下一个字符
        i++;
    }


    
    if (line[i] == '\0')
    {
       
        // argument 设置成 0，
        // 表示“没有参数”。
        *argument = 0;

        return;
    }
    // 情况2：找到了空格
    
    // 把空格改成 '\0'
    line[i] = '\0';
    i++;  // 跳过空格，指向参数的第一个字符
    while (line[i] == ' ')
    {
        // 如果参数前面还有空格，就继续跳过
        i++;
    }
    if (line[i] == '\0')
    {
        // 如果参数前面全是空格，
        // 最后走到 '\0'，
        // 也当作“没有参数”。
        *argument = 0;
        return;
    }   

    *argument = &line[i];
    if (**argument == '\0')
    {
        *argument = 0;
    }
}
// shell_run() 一旦进入以后，就会一直：
//
// 显示提示符
// ↓
// 等用户输入
// ↓
// 解析命令
// ↓
// 查命令表
// ↓
// 执行
// ↓
// 再显示提示符
//
void shell_run(void)
{
    char line[64];
    
    
   
    char *command;


    // 保存解析后的参数。
    //
    // 例如：
    //
    // argument -> "hello"
    char *argument;


    // 遍历命令表用
    int i;
    uart_puts("MiniOS> ");


    // Shell 永远运行
    while (1)
    {
        
        
        // 用户还没按 Enter：
        // console_readline() 返回 0
        // 用户按 Enter：
        // console_readline() 把输入复制到 line，
        // 然后返回 1。
        if (!console_readline(line, sizeof(line)))
        {
            // 没有完整输入
            // 继续等待
            continue;
        }
        
        // 用户刚按了 Enter，
        // Shell 的执行结果从下一行显示。
        uart_puts("\n");
        
        if (line[0] == '\0')
        {
            // 用户什么都没输入，只按了 Enter

            uart_puts("MiniOS> ");

            // 回去继续等待下一次输入
            continue;
        }

        
        
        // &command
        // 表示 command 变量自己的地址
        // &argument
        // 表示 argument 变量自己的地址
        parse_command(line, &command, &argument);


        // 默认认为：
        // 还没有找到对应命令。
        //
        // 0 = 没找到
        // 1 = 找到了
        int found = 0;

        // 当前有3个命令：
        //
        // 0 hello
        // 1 help
        // 2 echo
        for (i = 0; i < 3; i++)
        {
            // 比较：
            // strcmp() 返回0表示完全相同。
            if (strcmp(command, cmd_table[i].name) == 0)
            {
                // argument -> "hello"
                // 实际就是
                // cmd_echo("hello");
                cmd_table[i].func(argument);


                // 标记：
                // 已经成功找到命令。
                found = 1;


                // 已经找到了，
                // 不需要继续检查后面的命令。
                break;
            }
        }

        // 如果整个命令表都检查完，
        // found 仍然是0，
        // 说明用户输入了不存在的命令。
        if (!found)
        {
            uart_puts("Unknown command: ");

            // 把用户输入的命令打印出来
            uart_puts(command);

            uart_puts("\n");
        }

        // 当前命令已经处理完。
        // 等待下一条命令。
        uart_puts("MiniOS> ");
    }
}