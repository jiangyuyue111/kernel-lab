#include"page.h"
static unsigned char page_pool[PAGE_COUNT][PAGE_SIZE]
    __attribute__((aligned(PAGE_SIZE)));

    
static unsigned char page_used[PAGE_COUNT];
void page_init(void)
{
    int i;
    for(i = 0;i < PAGE_COUNT;i++)
    {
        page_used[i] = 0;
    }
}
void *alloc_page(void)
{
    unsigned long i;
    for (i = 0;i < PAGE_COUNT;i++)
    {
        //检查当前物理页是否分配
        if(page_used[i] == 0)
        {
            //找到空闲叶后，立即标记已分配
            page_used[i] = 1;
            //返回当前物理页起始地址
            return page_pool[i];
        }
    }
    return (void *)0;
}