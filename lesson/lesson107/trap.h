#ifndef TRAP_H
#define TRAP_H
void trap_init(void);
void enable_interrupt(void);
void trap_handler(void);
void *alloc_page(void);
//申请一个4kb物理页，成功则返回该页的起始地址，失败则返回空指针
#endif