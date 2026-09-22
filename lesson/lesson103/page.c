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
int free_page(void *page)
//返回：0：成功；-1：地址不合法；-2：重复释放
{
    //取得物理页池第0页起始地址
    unsigned long start =
    (unsigned long)&page_pool[0][0];
    //取得整个物理页池的结束边界
    unsigned long end =
    start + PAGE_COUNT + PAGE_SIZE;
    //把调用者传入的地址转换成整数
    unsigned long addr =
    (unsigned long)page;
    //检查地址是否属于我们的页池
    if (addr < start || addr >= end)
    {
        return -1;
    }
    //算出传入地址距离物理页池多远
    unsigned long offset = 
        addr - start;
    // 检查地址是不是一个物理页的起始地址。
    // 如果不能被4096整除，就不是合法的页起点。    
    if (offset % PAGE_SIZE != 0)
    {
        return -1;
    }
    //根据地址差值，计算它是第几个物理页。
     unsigned long i =
        offset / PAGE_SIZE;
    // 8. 检查这个物理页是不是已经空闲。
    if (page_used[i] == 0)
    {
        return -2;
    }
    // 9. 把当前物理页重新标记为空闲
    page_used[i] = 0;
    return 0;
}
