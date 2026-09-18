#include "mmu.h"

extern char kernel_end[];
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


// UART 使用的第1级页表
static unsigned long uart_level1_table[PT_ENTRIES]
    __attribute__((aligned(PAGE_SIZE)));

// UART 使用的第0级页表
static unsigned long uart_level0_table[PT_ENTRIES]
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


    if(root_page_table[vpn2] == 0)
    {
        root_page_table[vpn2] =
        make_table_pte((unsigned long)level1_table);
    }
    if(level1_table[vpn1]== 0)
    {
        level1_table[vpn1] =
        make_table_pte((unsigned long)level0_table);
    }



    level0_table[vpn0] =
    make_leaf_pte(pa,flags);
}


// ============================================================
// 6. 初始化三级页表
// ============================================================


static unsigned long make_satp_value(void)
{
    // 1. 取得根页表的物理地址
    unsigned long root_addr =
        (unsigned long)root_page_table;

    // 2. 物理地址 >> 12，得到根页表 PPN
    unsigned long root_ppn =
        root_addr >> 12;

    // 3. Sv39 MODE = 8
    unsigned long mode =
        8UL << 60;

    // 4. MODE + 根页表PPN
    return mode | root_ppn;
}

 static void enable_mmu(unsigned long satp_value)
{
    // 把我们准备好的值写进 satp CSR
    asm volatile(
        "csrw satp, %0"
        :
        : "r"(satp_value)
        : "memory"
    );

    // 刷新地址转换相关缓存
    asm volatile(
        "sfence.vma"
        :
        :
        : "memory"
    );
}



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
        uart_level1_table[i] = 0;
        uart_level0_table[i] = 0;
    }
    unsigned long addr;

// 连续映射 3 个 4KB 页面
    for (addr = 0x80000000UL;
    addr < (unsigned long)kernel_end;
    addr += PAGE_SIZE)
{
    map_page(
        addr,
        addr,
        PTE_R | PTE_W | PTE_X
    );
}

// UART: VA 0x10000000 → PA 0x10000000

// VPN2 = 0
// root[0] 告诉 MMU：UART 的第1级页表在哪里
root_page_table[0] =
    make_table_pte(
        (unsigned long)uart_level1_table
    );

// VPN1 = 128
// uart_level1[128] 告诉 MMU：UART 的第0级页表在哪里
uart_level1_table[128] =
    make_table_pte(
        (unsigned long)uart_level0_table
    );

// VPN0 = 0
// 最后一层直接指向 UART 的物理地址
uart_level0_table[0] =
    make_leaf_pte(
        0x10000000UL,
        PTE_R | PTE_W
    );
   // 计算 satp 应该写入的值
unsigned long satp_value =
    make_satp_value();

// 把这个值真正写入 CPU 的 satp 寄存器
enable_mmu(satp_value);
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