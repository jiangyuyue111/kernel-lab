#ifndef HEAP_H
#define HEAP_H
//初始化内核堆
//返回0：成功申请到一页内存，返回-1：申请失败
int heap_init(void);
void *kmalloc(unsigned long size);
int kfree(void *ptr);
#endif