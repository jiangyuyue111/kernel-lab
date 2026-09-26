#include"heap.h"
#include"page.h"

#define HEAP_BLOCK_SIZE 32UL
#define HEAP_BLOCK_COUNT \
    (PAGE_SIZE / HEAP_BLOCK_SIZE)

//保存堆内存的起始地址
 //初始化围殴0，表示还没有申请到内存
void *heap_base = 0;

static unsigned char heap_used[HEAP_BLOCK_COUNT];
//如果某一个位置是一次分配的起点，就记录这次分配占了多少个32B块
//例如kmalloc(70)从第三块开始，占3块
//那么heap_alloc_blocks[3] = 3;
static unsigned char heap_alloc_blocks[HEAP_BLOCK_COUNT];

//初始化内核堆
int heap_init(void)
{
    int i;
    //如果已经初始化，就不用重复申请
    if(heap_base != 0)
    {
        return 0;
    }
    //向物理页分配器申请一整页
    void *page = alloc_page();
    //检查有没有申请成功
    if(page == 0)
    {
        //没有空闲页申请失败，初始化失败
        return -1;
    }
    //保存申请到的物理页起始地址
    heap_base = page;

    //初始化128个小块的两张状态表
    for(i = 0;i < HEAP_BLOCK_COUNT; i++)
    {
        //空闲
        heap_used[i] = 0;
        //分配起点
        heap_alloc_blocks[i] = 0;
    }
    //返回0，表示初始化成功
    return 0;
}
void *kmalloc(unsigned long size)
{
    unsigned long blocks_needed;
    unsigned long start;
    unsigned long j;
    //不能申请0字节
    if(size == 0)
    {
        return (void *)0;
    }
    //计算需要多少个32B块
    blocks_needed =
        (size + HEAP_BLOCK_SIZE -1)
        / HEAP_BLOCK_SIZE;
        //比整个heap还大，直接失败
        if (blocks_needed > HEAP_BLOCK_COUNT)
        {
            return (void *)0;
        }
        
    if (heap_base == 0)
    {
        return (void*)0;
    }
    //尝试每一个可能的起始位置
    for (start = 0;
            start + blocks_needed <= HEAP_BLOCK_COUNT;
            start ++)
        {
            //检查从start开始是否有足够连续空闲块
            for(j = 0;j < blocks_needed;j++)
            {
                if(heap_used[start + j] != 0)
                {
                    break;
                }
            }
            //j走完了全部需要的块，说明这一段可以用
            if(j == blocks_needed)
            {
                //把这一段全部标记为已使用
                for(j = 0;j <blocks_needed;j++)
                {
                    heap_used[start + j]=1;
                }
                //记录这次分配占了多少块
                heap_alloc_blocks[start] =
                    (unsigned char)blocks_needed;
                    //返回这一段连续空闲的起始地址
                    return(void *)(
                        (unsigned long)heap_base
                        + start *HEAP_BLOCK_SIZE
                    );
            }
        }
        //没有找到足够连续空间
        return(void *)0;
}
int kfree(void *ptr)
{
    unsigned long addr;
    unsigned long base;
    unsigned long offset;
    unsigned long start;
    unsigned long blocks;
    unsigned long j;
    //空指针不能释放
    if (ptr == 0)
    {
        return -1;
    }
   // heap还没初始化当然也没有东西可以释放
    if(heap_base == 0)
    {
        return -1;
    }
    //把指针转化成整数方便进行地址运算
    addr = (unsigned long)ptr;
    base = (unsigned long)heap_base;
    //*ptr必须位于这一页heap内部
    if (addr < base)
    {
        return -1;
    }
    if (addr >= base + PAGE_SIZE)
    {
        return -1;
    }
    //算ptr距离heap起点多少字节
    offset = addr - base;
   // 必须正好指向某个32B块的起点
    if(offset % HEAP_BLOCK_SIZE != 0)
    {
        return -1;
    }
    //算这是第几个32B块
    start = offset / HEAP_BLOCK_SIZE;
    //查这笔分配当初占了多少块
    blocks = heap_alloc_blocks[start];
   
    if(blocks == 0)
    {
        return -2;
    }
     //把这笔分配占用的所有快释放掉
    for (j = 0;j < blocks;j++)
    {
        heap_used[start + j]=0;
    }
    //清楚这笔分配的记录
    heap_alloc_blocks[start] = 0;
    return 0;
}