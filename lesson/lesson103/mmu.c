#include "mmu.h"


// ============================================================
// 1. Sv39 基础参数
// ============================================================

// 一张页表有512个PTE
#define PT_ENTRIES 512

// 一页大小是4096字节 = 4KB
#define PAGE_SIZE 4096UL


// ============================================================
// 2. PTE 权限位
// ============================================================
//
// PTE最低几位保存各种标志：
//
// bit0 = V
// bit1 = R
// bit2 = W
// bit3 = X
//

#define PTE_V (1UL << 0)   // Valid：有效
#define PTE_R (1UL << 1)   // Read：可读
#define PTE_W (1UL << 2)   // Write：可写
#define PTE_X (1UL << 3)   // Execute：可执行


// ============================================================
// 3. 创建三级页表
// ============================================================
//
// 一张页表：
//
// 512个PTE × 每个8字节
// = 4096字节
// = 4KB
//
// aligned(4096)
// 表示要求页表地址4KB对齐
//

// 第一层页表
// 使用 VPN2 作为索引

static unsigned long root_page_table[PT_ENTRIES]
    __attribute__((aligned(PAGE_SIZE)));


// 第二层页表
// 使用 VPN1 作为索引
static unsigned long level1_table[PT_ENTRIES]
    __attribute__((aligned(PAGE_SIZE)));


// 第三层页表
// 使用 VPN0 作为索引
static unsigned long level0_table[PT_ENTRIES]
    __attribute__((aligned(PAGE_SIZE)));


// ============================================================
// 4. 创建“指向下一层页表”的PTE
// ============================================================
//
// 输入：
// next_table_addr
// 下一张页表的地址
//
// 输出：
// 一个非叶子PTE
//
static unsigned long make_table_pte(
    unsigned long next_table_addr)
{
    // 地址右移12位
    //
    // 去掉4KB页内offset
    //
    // 得到物理页号PPN
    unsigned long ppn =
        next_table_addr >> 12;


    // PTE中：
    //
    // bit0~9 留给各种flag
    //
    // PPN从bit10开始
    //
    // 所以PPN要左移10位
    unsigned long pte =
        ppn << 10;


    // 设置V=1
    //
    // R/W/X仍然是0
    //
    // 表示：
    // 这是一个有效PTE，
    // 但它指向的是下一层页表
    pte = pte | PTE_V;


    return pte;
}


// ============================================================
// 5. 创建最终叶子PTE
// ============================================================
//
// pa：
// 最终要映射到的物理页地址
//
// flags：
// R/W/X权限
//
static unsigned long make_leaf_pte(
    unsigned long pa,
    unsigned long flags)
{
    // 去掉物理地址最低12位
    // 得到物理页号PPN
    unsigned long ppn =
        pa >> 12;


    // 把PPN放到PTE的bit10及以上
    unsigned long pte =
        ppn << 10;


    // 加入：
    //
    // R/W/X权限
    //
    // 同时设置V=1
    pte =
        pte | flags | PTE_V;


    return pte;
}

static unsigned long get_vpn2(unsigned long va)
{
    return (va >> 30) & 0x1ff;
}
static unsigned long get_vpn1(unsigned long va)
{
    return (va >> 21) & 0x1ff;
}
static unsigned long get_vpn0(unsigned long va)
{
    return (va >> 12) & 0x1ff;
}


static void map_page(
    unsigned long va,
    unsigned long pa,
    unsigned long flags)
{    
    unsigned long vpn2 = get_vpn2(va);
    unsigned long vpn1 = get_vpn1(va);
    unsigned long vpn0 = get_vpn0(va);
    root_page_table[vpn2] = 
    make_table_pte((unsigned long)level1_table);
    level1_table[vpn1] =
    make_table_pte((unsigned long)level0_table);
    level0_table[vpn0] =
    make_leaf_pte(pa,flags);
}


// ============================================================
// 6. 初始化三级页表
// ============================================================

void mmu_init(void)
{
    int i;


    // --------------------------------------------------------
    // 第一步：清空三张页表
    // --------------------------------------------------------
    //
    // PTE = 0
    //
    // 意味着：
    // V = 0
    //
    // 当前映射无效
    //
    for (i = 0; i < PT_ENTRIES; i++)
    {
        root_page_table[i] = 0;
        level1_table[i] = 0;
        level0_table[i] = 0;
    }
    map_page(
        0x80000000UL,
        0x80000000UL,
        PTE_R | PTE_W | PTE_X
    );
    map_page(
        0x80001000UL,
        0X80001000UL,
        PTE_R | PTE_W |PTE_X
    );
}

   


    


// ============================================================
// 7. 返回根页表地址
// ============================================================
//
// 下一课设置satp时，CPU需要知道：
//
// “第一张页表在哪里？”
//
unsigned long mmu_root_table_address(void)
{
    return (unsigned long)root_page_table;
}