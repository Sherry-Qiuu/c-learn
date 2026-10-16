/* ============================================================================
 * ex07_struct_union.c —— struct · union · 位域 · 内存对齐
 * ----------------------------------------------------------------------------
 * 这是 C 里「数据结构」的全部家当。没有 class，只有 struct；
 * 没有继承，只有嵌套；没有多态，只有函数指针（见 ex09）。
 * 而 STM32 的寄存器定义，就是「struct + volatile + 固定地址」这三样东西。
 *
 *   make run/ex07
 * ========================================================================== */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

/* ---------------------------------------------------------------------------
 * 一个普通的 struct。typedef 是为了以后能直接写 Point 而不用写 struct Point。
 * 注意：C 里 typedef struct {...} Point; 之后，Point 和 struct Point 是两个
 *       不同的名字（在 C 里可以并存），C++ 里则自动合并。
 * ------------------------------------------------------------------------- */
typedef struct {
    int x;
    int y;
} Point;

/* 嵌套 struct */
typedef struct {
    Point   start;
    Point   end;
    char    label[16];
} Segment;

/* 位域：让编译器帮你把几个小字段塞进同一个字节。
 * 嵌入式里用来解析硬件寄存器、协议帧特别常见（比如 CAN 报文头、IP 头）。 */
typedef struct {
    uint8_t  enable    : 1;
    uint8_t  mode      : 2;   /* 取值 0..3 */
    uint8_t  priority  : 3;   /* 取值 0..7 */
    uint8_t  reserved  : 2;
} CtrlBits;

/* ---------------------------------------------------------------------------
 * TODO 1：向量相加（值传递）
 *   小结构体可以直接按值传，编译器会帮你拷贝。
 * ------------------------------------------------------------------------- */
Point point_add(Point a, Point b)
{
    /* 在这里写你的代码 */
    return a;
}

/* ---------------------------------------------------------------------------
 * TODO 2：求两点距离的平方（避免开方，返回 int）
 *   d² = (x1-x2)² + (y1-y2)²
 * ------------------------------------------------------------------------- */
int point_dist_sq(Point a, Point b)
{
    /* 在这里写你的代码 */
    return 0;
}

/* ---------------------------------------------------------------------------
 * TODO 3：就地平移一个 Segment（通过指针修改，避免拷贝整个结构体）
 *
 *   为什么要指针？因为 Segment 有 40 字节左右，按值传要拷一份。
 *   嵌入式里结构体动辄几百字节，一律传指针。
 *   而且只有传指针才能修改调用方的对象。
 * ------------------------------------------------------------------------- */
void segment_translate(Segment *s, int dx, int dy)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 4：设置位域字段（顺便体会位域的操作方式和普通成员完全一样）
 * ------------------------------------------------------------------------- */
CtrlBits ctrl_make(uint8_t enable, uint8_t mode, uint8_t priority)
{
    CtrlBits c;
    c.enable   = enable ? 1u : 0u;
    c.mode     = 0;
    c.priority = 0;
    c.reserved = 0;
    /* 在这里把你的三行赋值补上（提示：直接赋值即可，编译器负责截断和移位） */
    return c;
}

/* ---------------------------------------------------------------------------
 * TODO 5：用 union 判断当前机器的字节序
 *   返回 1 表示小端（低字节在低地址，x86/ARM 默认都是），0 表示大端。
 *
 *   union 的关键性质：所有成员共享同一块内存，大小 = 最大成员的大小。
 *   把一个 uint16_t 写进去，再读它的两个字节，就能看出字节顺序。
 * ------------------------------------------------------------------------- */
int is_little_endian(void)
{
    /* 在这里写你的代码 */
    return -1;
}

int main(void)
{
    printf("========== 任务 1/2：struct 基础 ==========\n");
    {
        Point a = { 3, 4 };
        Point b = { 0, 0 };

        Point c = point_add(a, b);
        assert(c.x == 3 && c.y == 4);
        printf("  point_add  OK\n");

        Point p1 = { 0, 0 }, p2 = { 3, 4 };
        assert(point_dist_sq(p1, p2) == 25);
        assert(point_dist_sq(p1, p1) == 0);
        printf("  point_dist_sq  OK\n");

        /* 结构体可以直接赋值（=），编译器做的是「按成员逐字节拷贝」，
         * 这叫浅拷贝。如果结构体里有指针，指针会被原样复制 ——
         * 两个结构体就指向同一块内存了，这就是浅拷贝的坑。 */
        Point d = a;
        d.x = 99;
        assert(a.x == 3);      /* a 没被影响，因为 int 是值语义 */
        printf("  结构体赋值是浅拷贝，值成员互不影响  OK\n");
    }

    printf("\n========== 任务 3：指针传参 ==========\n");
    {
        Segment s = { {0, 0}, {10, 10}, "seg" };
        segment_translate(&s, 5, -3);
        assert(s.start.x == 5 && s.start.y == -3);
        assert(s.end.x == 15 && s.end.y == 7);
        printf("  segment_translate -> start(%d,%d) end(%d,%d)  OK\n",
               s.start.x, s.start.y, s.end.x, s.end.y);
    }

    printf("\n========== 任务 4：位域 ==========\n");
    {
        CtrlBits c = ctrl_make(1, 2, 5);
        assert(c.enable == 1);
        assert(c.mode == 2);
        assert(c.priority == 5);

        printf("  enable=%u mode=%u priority=%u  占 %zu 字节\n",
               c.enable, c.mode, c.priority, sizeof(CtrlBits));
        assert(sizeof(CtrlBits) == 1);
        printf("  三个字段 + 2 位保留，全部塞进了 1 个字节  OK\n");

        /* 位域的限制：不能取地址，不能直接比较两个位域结构体 */
        /* &c.mode;   <-- 编译错误：不能对位域取地址 */
        printf("  注意：位域成员不可取地址（&c.mode 编译不过）\n");
    }

    printf("\n========== 任务 5：union 与字节序 ==========\n");
    {
        union {
            uint16_t u16;
            uint8_t  bytes[2];
        } probe;

        probe.u16 = 0x0102;
        printf("  0x0102 写进 uint16_t，两个字节读出来是 0x%02X 0x%02X\n",
               probe.bytes[0], probe.bytes[1]);

        if (probe.bytes[0] == 0x02) {
            printf("  -> 低字节在低地址 = 小端（x86 / ARM 默认）\n");
        } else {
            printf("  -> 高字节在低地址 = 大端\n");
        }

        assert(is_little_endian() == 1);
        printf("  is_little_endian() 通过  OK\n");

        /* union 的另一个经典用途：把 float 的位模式当 uint32_t 看 */
        union {
            float    f;
            uint32_t u;
        } magic;
        magic.f = 1.0f;
        printf("\n  float 1.0f 的二进制位 = 0x%08X\n", magic.u);
        printf("  按 IEEE 754：符号 0 | 指数 01111111 | 尾数 全 0 -> 0x3F800000\n");
        assert(magic.u == 0x3F800000u);
        printf("  这种「同一块内存，两种读法」就是类型双关（type punning）\n");
    }

    printf("\n========== 任务 6：内存对齐（嵌入式必知）==========\n");
    {
        typedef struct { uint8_t a; uint32_t b; uint8_t c; } Unaligned;
        typedef struct { uint32_t b; uint8_t a; uint8_t c; } Aligned;

        printf("  struct { uint8_t a; uint32_t b; uint8_t c; }  sizeof = %zu\n",
               sizeof(Unaligned));
        printf("  struct { uint32_t b; uint8_t a; uint8_t c; }  sizeof = %zu\n",
               sizeof(Aligned));
        printf("  成员顺序不同，大小从 12 变成 8 —— 因为编译器会插入填充字节\n\n");

        printf("  各成员的偏移：\n");
        printf("    Unaligned.a 偏移 = %zu\n", offsetof(Unaligned, a));
        printf("    Unaligned.b 偏移 = %zu   <- 前面填了 3 个字节\n", offsetof(Unaligned, b));
        printf("    Unaligned.c 偏移 = %zu\n", offsetof(Unaligned, c));

        assert(sizeof(Unaligned) == 12);
        assert(sizeof(Aligned) == 8);

        printf("\n  规则：每个成员的偏移必须是它自身对齐要求的整数倍；\n");
        printf("        结构体总大小必须是「最大成员对齐值」的整数倍。\n");
        printf("  嵌入式影响：协议帧、Flash 存储结构体一定要手动排布，\n");
        printf("              或者用 #pragma pack(1) 关闭填充（但会牺牲访问速度，\n");
        printf("              在某些 ARM 平台上非对齐访问会直接 HardFault）。\n");

        /* 内存对齐的另一个坑：malloc 出来的内存是对齐的，
         * 但如果你自己搞一个 byte 缓冲区再强转成结构体指针，就可能不对齐。 */
        uint8_t raw[64];
        printf("\n  (uint8_t*)raw 的地址对齐情况：%%16 = %zu\n",
               (size_t)((uintptr_t)raw % 16));
        printf("  手动缓冲区转结构体指针时，务必确认地址对齐\n");
    }

    printf("\n========== 任务 7：结构体数组与「数据 + 操作」分离 ==========\n");
    {
        Point poly[4] = { {0,0}, {4,0}, {4,3}, {0,3} };
        int total = 0;
        for (size_t i = 0; i < 4; i++) {
            total += point_dist_sq(poly[i], (Point){0, 0});
        }
        printf("  四个顶点到原点距离平方和 = %d\n", total);
        assert(total == 0 + 16 + 25 + 9);

        printf("  C 里没有成员函数，所以「数据」和「操作」是分开的：\n");
        printf("    数据：Point poly[4]\n");
        printf("    操作：point_add / point_dist_sq / segment_translate\n");
        printf("  这就是 C 和 C++ 最根本的差别。习惯它。\n");
    }

    printf("\n>>> ex07 全部通过。下一站：ex08_bitops.c（嵌入式核心）\n");
    return 0;
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

Point point_add(Point a, Point b)
{
    Point r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    return r;
}

int point_dist_sq(Point a, Point b)
{
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return dx * dx + dy * dy;
}

void segment_translate(Segment *s, int dx, int dy)
{
    if (s == NULL) return;
    s->start.x += dx;
    s->start.y += dy;
    s->end.x   += dx;
    s->end.y   += dy;
}

CtrlBits ctrl_make(uint8_t enable, uint8_t mode, uint8_t priority)
{
    CtrlBits c;
    c.enable   = enable ? 1u : 0u;
    c.mode     = mode & 0x3u;
    c.priority = priority & 0x7u;
    c.reserved = 0;
    return c;
}

int is_little_endian(void)
{
    union {
        uint16_t u16;
        uint8_t  bytes[2];
    } probe;
    probe.u16 = 0x0102;
    return probe.bytes[0] == 0x02 ? 1 : 0;
}

  --- 几个要点 ---

  1. ->  和  .  的区别

       s->start.x  等价于  (*s).start.x
       左边是「指针」，右边是「对象」。
       这只是语法糖，编译器生成的代码完全一样。
       但 *s.start.x 是错的：它会被解析成 *(s.start.x)，先对指针取成员 —— 编译不过。

  2. 为什么 point_add 返回 r 这样一个局部变量是安全的？

       因为返回的是「结构体的值」，不是它的地址。
       编译器会把整个结构体拷进调用方的临时空间，栈帧销毁不影响这个副本。
       对比 ex02 任务 6 里 return &local; —— 那是返回地址，就非法了。
       记住这条分界线：返回值安全，返回栈上地址不安全。

  3. 位域的实际内存布局是「实现定义」的

       CtrlBits 在本机是 1 字节，但标准并没有强制编译器必须这么排。
       不同编译器、不同平台的位域分配顺序可能不同（尤其涉及大小端和
       位域跨越存储单元时）。
       所以：位域适合「在本平台内部使用」，不适合用来解析跨平台协议。
       要做协议解析，用移位和掩码（ex08 的方法）才可移植。

  4. 为什么 union 能看字节序？

       union 的所有成员起始地址相同。
       写 probe.u16 = 0x0102：
         小端机器内存布局： [0x02][0x01]
         大端机器内存布局： [0x01][0x02]
       所以读 bytes[0] 就知道是哪种。

  5. 内存对齐为什么在嵌入式里是大事？

       两条硬约束：
         - 某些 ARM 内核（尤其 Cortex-M0）对「非对齐访问」零容忍，直接 HardFault
         - 对齐访问比非对齐访问快得多（一次总线周期 vs 两次）

       所以嵌入式结构体的成员顺序是个「性能/空间设计问题」，
       而不是随便写写。规律：把大的成员放前面，把同类型的放一起。

  6. 结构体比较为什么不能用 ==

       if (a == b) 编译不过。C 不做成员级比较。
       要比较要么自己写函数逐成员比，要么：
         if (memcmp(&a, &b, sizeof a) == 0)
       但 memcmp 会把「填充字节」里的垃圾也拿来比，结果可能不稳定。
       所以正解是自己写成员比较函数。这是嵌入式代码 review 的高频考点。
 * ========================================================================== */
