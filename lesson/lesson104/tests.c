#include "tests.h"
#include "heap.h"
void *debug_heap1 = 0;
void *debug_heap2 = 0;
void *debug_heap3 = 0;
void *debug_heap4 = 0;
int debug_heap_result = -99;
int debug_kfree_result = -99;
int debug_bad_kfree = -99;
void run_memory_tests(void)
{
    debug_heap1 = kmalloc(8);
    debug_heap2 = kmalloc(40);
    debug_heap3 = kmalloc(70);
    //释放第二笔分配
    debug_kfree_result = kfree(debug_heap2);
    // 释放第2块后，再申请50B
    debug_heap4 = kmalloc(50);

    debug_bad_kfree =
        kfree(
            (void *)(
                (unsigned long)debug_heap3
                + 32UL
             )
         );
}