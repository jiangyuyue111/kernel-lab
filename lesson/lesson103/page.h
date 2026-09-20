#ifndef PAGE_H
#define PAGE_H

#define PAGE_SIZE 4096UL
#define PAGE_COUNT 16
void page_init(void);
#endif
//负责告诉其他C文件：我们的物理叶有多大，一共有多少也
//以及有哪些函数可以调用