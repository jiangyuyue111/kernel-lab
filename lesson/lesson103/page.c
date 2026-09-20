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