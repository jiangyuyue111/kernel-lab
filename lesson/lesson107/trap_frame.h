#ifndef TRAP_FRAME_H
#define TRAP_FRAME_H
// 提供offsetof宏
#include <stddef.h>

// CPU中断现场的数据结构
// 每个unsigned long在RV64中占8字节
struct trap_frame
{
    unsigned long ra;       // 偏移0
    unsigned long gp;       // 偏移8
    unsigned long tp;       // 偏移16

    unsigned long t0;       // 偏移24
    unsigned long t1;       // 偏移32
    unsigned long t2;       // 偏移40

    unsigned long s0;       // 偏移48
    unsigned long s1;       // 偏移56

    unsigned long a0;       // 偏移64
    unsigned long a1;       // 偏移72
    unsigned long a2;       // 偏移80
    unsigned long a3;       // 偏移88
    unsigned long a4;       // 偏移96
    unsigned long a5;       // 偏移104
    unsigned long a6;       // 偏移112
    unsigned long a7;       // 偏移120

    unsigned long s2;       // 偏移128
    unsigned long s3;       // 偏移136
    unsigned long s4;       // 偏移144
    unsigned long s5;       // 偏移152
    unsigned long s6;       // 偏移160
    unsigned long s7;       // 偏移168
    unsigned long s8;       // 偏移176
    unsigned long s9;       // 偏移184
    unsigned long s10;      // 偏移192
    unsigned long s11;      // 偏移200

    unsigned long t3;       // 偏移208
    unsigned long t4;       // 偏移216
    unsigned long t5;       // 偏移224
    unsigned long t6;       // 偏移232

    unsigned long mepc;     // 偏移240
    unsigned long mstatus;  // 偏移248
};

// 编译时检查结构体是否恰好256字节
_Static_assert(
    sizeof(struct trap_frame) == 256,
    "trap_frame size must be 256 bytes"
);
// 检查RV64下unsigned long的大小
_Static_assert(
    sizeof(unsigned long) == 8,
    "unsigned long must be 8 bytes"
);

// 检查几个关键成员的内存偏移
_Static_assert(
    offsetof(struct trap_frame, ra) == 0,
    "ra offset error"
);

_Static_assert(
    offsetof(struct trap_frame, a0) == 64,
    "a0 offset error"
);

_Static_assert(
    offsetof(struct trap_frame, t6) == 232,
    "t6 offset error"
);

_Static_assert(
    offsetof(struct trap_frame, mepc) == 240,
    "mepc offset error"
);

_Static_assert(
    offsetof(struct trap_frame, mstatus) == 248,
    "mstatus offset error"
);
#endif