#include "string.h"

int strcmp(const char *s1, const char *s2)
{
    // 只要两个字符串当前字符一样，
    // 并且还没有走到字符串结尾 '\0'
    // 就继续往后比较
    while (*s1 == *s2 && *s1 != '\0')
    {
        s1++;
        s2++;
    }

    // 如果最终两个当前字符仍然相同，
    // 说明两个字符串完全一样
    //
    // 例如：
    // hello\0
    // hello\0
    //
    // 最后比较的是：
    // '\0' 和 '\0'
    //
    // 结果就是 0
    return (unsigned char)*s1 - (unsigned char)*s2;
}