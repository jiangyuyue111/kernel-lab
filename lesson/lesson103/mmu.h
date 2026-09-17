#ifndef MMU_H
#define MMU_H

//初始化我们的三级页表
void mmu_init(void);
//返回根页表地址
//下一刻设置sap会用到
unsigned long mmu_root_table_adress(void);
#endif