/* ============================================================================
 * ex06_malloc_dynarray.c —— 动态内存：malloc / realloc / free + 手写动态数组
 * ----------------------------------------------------------------------------
 * 这个练习的目标是让你彻底搞懂「堆内存的生命周期」，而不是让你在单片机上
 * 用动态数组（相反，嵌入式里基本禁用 malloc，见文件末尾的说明）。
 *
 * 你要实现一个会给 C++ 程序员「似曾相识」感觉的东西：
 *   一个简化版 std::vector<int>，只不过 API 全是 C 风格。
 *
 *   make run/ex06
 *   make san/ex06     <-- 建议跑一次，让编译器帮你查 UB
 * ========================================================================== */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

/* ---------------------------------------------------------------------------
 * 数据结构：这就是 C 版的「类」——一个 struct 装数据，一堆函数当方法。
 * ------------------------------------------------------------------------- */
typedef struct {
    int    *data;
    size_t  size;    /* 已经存了几个 */
    size_t  cap;     /* 最多能存几个 */
} IntVec;

/* ---------------------------------------------------------------------------
 * TODO 1：初始化
 *   - 把 data 设为 NULL，size 设为 0，cap 设为 0
 *   - 但如果你希望一开始就预留空间，也允许 cap 传入非 0
 *   返回 0 成功，-1 失败
 * ------------------------------------------------------------------------- */
int vec_init(IntVec *v, size_t initial_cap)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ---------------------------------------------------------------------------
 * TODO 2：释放（不允许 double free，释放后要把指针置 NULL、size/cap 归零）
 * ------------------------------------------------------------------------- */
void vec_free(IntVec *v)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 3：确保容量至少为 need（不够就扩容）
 *   策略：按 2 倍增长（这样均摊下来 push 是 O(1)）
 *   need == 0 时把 data 释放掉、cap 归零（相当于 clear 内存但不改 size？不，
 *   这里就直接设 cap = 0、data = NULL，size 也归 0）
 *   返回 0 成功，-1 失败
 *
 *   注意：这里必须用「临时指针 + 判 NULL」的写法，参考文件末尾说明。
 * ------------------------------------------------------------------------- */
int vec_reserve(IntVec *v, size_t need)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ---------------------------------------------------------------------------
 * TODO 4：尾部插入，满了就扩容
 *   返回 0 成功，-1 失败
 * ------------------------------------------------------------------------- */
int vec_push(IntVec *v, int value)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ---------------------------------------------------------------------------
 * TODO 5：尾部弹出，弹出的值写进 *out
 *   空的时候返回 -1（且不动 *out）
 * ------------------------------------------------------------------------- */
int vec_pop(IntVec *v, int *out)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ---------------------------------------------------------------------------
 * TODO 6：随机访问，越界返回 -1
 * ------------------------------------------------------------------------- */
int vec_get(const IntVec *v, size_t index, int *out)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ---------------------------------------------------------------------------
 * TODO 7：把 v 里的元素复制进调用方的缓冲区
 *   缓冲区不足（cap < v->size）返回 -1。
 *   这就是 C 里「返回数组」的标准做法：调用方准备空间，我往里填。
 * ------------------------------------------------------------------------- */
int vec_copy_to(const IntVec *v, int *dest, size_t dest_cap)
{
    /* 在这里写你的代码 */
    return -1;
}

int main(void)
{
    printf("========== 任务 1/2：初始化与释放 ==========\n");
    {
        IntVec v;
        assert(vec_init(&v, 0) == 0);
        assert(v.size == 0);
        assert(v.cap == 0);

        vec_free(&v);
        assert(v.data == NULL && v.size == 0 && v.cap == 0);

        /* 反复 init/free 不泄漏——跑完用任务 6 的检查确认 */
        for (int i = 0; i < 100; i++) {
            IntVec t;
            vec_init(&t, 4);
            vec_free(&t);
            vec_free(&t);        /* 二次释放也不能崩 —— 因为已经置 NULL */
        }
        printf("  初始化 / 释放 / 二次释放  OK\n");
    }

    printf("\n========== 任务 3：扩容策略 ==========\n");
    {
        IntVec v;
        vec_init(&v, 0);

        assert(vec_reserve(&v, 8) == 0);
        assert(v.cap >= 8);
        printf("  reserve(8)  -> cap = %zu\n", v.cap);

        size_t before = v.cap;
        assert(vec_reserve(&v, 5) == 0);         /* 已经够大，不该动 */
        assert(v.cap == before);
        printf("  reserve(5)  -> cap 不变 = %zu（容量够就不该重新分配）\n", v.cap);

        vec_free(&v);
    }

    printf("\n========== 任务 4/6：插入与访问 ==========\n");
    {
        IntVec v;
        vec_init(&v, 0);

        for (int i = 0; i < 1000; i++) {
            assert(vec_push(&v, i * 3) == 0);
        }
        assert(v.size == 1000);
        printf("  push 1000 个   -> size = %zu, cap = %zu\n", v.size, v.cap);

        assert(v.cap >= 1000);
        assert(v.cap <= 2048);            /* 2 倍增长，不该超过 2*size */

        for (int i = 0; i < 1000; i++) {
            int got = 0;
            assert(vec_get(&v, (size_t)i, &got) == 0);
            assert(got == i * 3);
        }
        printf("  逐个读取验证     OK\n");

        int dummy = -1;
        assert(vec_get(&v, 1000, &dummy) == -1);
        assert(dummy == -1);
        printf("  越界访问返回 -1  OK\n");

        vec_free(&v);
    }

    printf("\n========== 任务 5：弹出 ==========\n");
    {
        IntVec v;
        vec_init(&v, 0);

        for (int i = 1; i <= 5; i++) vec_push(&v, i);

        int out = 0;
        for (int expect = 5; expect >= 1; expect--) {
            assert(vec_pop(&v, &out) == 0);
            assert(out == expect);
        }
        assert(v.size == 0);

        out = 12345;
        assert(vec_pop(&v, &out) == -1);
        assert(out == 12345);           /* 失败时不许改 out */
        printf("  先进后出 + 空栈保护  OK\n");

        vec_free(&v);
    }

    printf("\n========== 任务 7：复制到调用方缓冲区 ==========\n");
    {
        IntVec v;
        vec_init(&v, 0);
        for (int i = 0; i < 6; i++) vec_push(&v, i * i);

        int buf[6];
        assert(vec_copy_to(&v, buf, 6) == 0);
        for (int i = 0; i < 6; i++) assert(buf[i] == i * i);
        printf("  缓冲区正好够   OK\n");

        int small[3];
        assert(vec_copy_to(&v, small, 3) == -1);
        printf("  缓冲区不够时返回 -1  OK\n");

        vec_free(&v);
    }

    printf("\n========== 任务 8：两个经典错误，看清它们 ==========\n");
    {
        printf("  【错误 1】直接 p = realloc(p, n)：\n");
        printf("      realloc 失败会返回 NULL，原来的 p 就被覆盖丢了 —— 内存泄漏。\n");
        printf("      正确写法永远是：先存临时变量，判 NULL 之后再赋回去。\n\n");

        printf("  【错误 2】free 之后继续用：\n");
        printf("      p 指向的内存已经还给系统，再读写就是 use-after-free。\n");
        printf("      PC 上可能「看起来正常」，在单片机上是随机崩溃。\n");
        printf("      习惯：free(p); p = NULL;   —— 然后任何一次误用都会立刻崩在 NULL 上，\n");
        printf("            崩得早比崩得晚好，这是嵌入式调试的第一原则。\n\n");

        printf("  【错误 3】malloc 不判 NULL：\n");
        printf("      PC 上几乎不会失败，单片机上堆只有几 KB，失败是常态。\n\n");

        printf("  【错误 4】忘了 free：\n");
        printf("      长跑程序（服务器、单片机）内存会慢慢耗光。\n");
    }

    printf("\n========== 任务 9：看看编译器怎么说 ==========\n");
    printf("  跑一次：make san/ex06\n");
    printf("  它会用 UBSan 检查未定义行为。如果你的代码里有符号溢出、\n");
    printf("  空指针解引用之类的问题，会在运行时直接报错并指出行号。\n");

    printf("\n========== 嵌入式提醒（很重要）==========\n");
    printf("  STM32F103 一共只有 20 KB SRAM。堆能分到的往往只有几 KB。\n");
    printf("  更重要的是：反复 malloc/free 会产生内存碎片，\n");
    printf("  跑几个小时后可能「总共还有空间，但就是分配不出来」。\n");
    printf("  所以工业级嵌入式代码里，动态分配基本是禁止的。\n");
    printf("  替代方案：静态数组 + 最大容量宏，见 notes/stm32_prep.md 第 4 节。\n");
    printf("  你写这个动态数组的价值在于「理解内存」，不在于「以后这么用」。\n");

    printf("\n>>> ex06 全部通过。下一站：ex07_struct_union.c\n");
    return 0;
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

int vec_init(IntVec *v, size_t initial_cap)
{
    if (v == NULL) return -1;
    v->data = NULL;
    v->size = 0;
    v->cap  = 0;
    if (initial_cap > 0) {
        return vec_reserve(v, initial_cap);
    }
    return 0;
}

void vec_free(IntVec *v)
{
    if (v == NULL) return;
    free(v->data);
    v->data = NULL;
    v->size = 0;
    v->cap  = 0;
}

int vec_reserve(IntVec *v, size_t need)
{
    if (v == NULL) return -1;

    if (need <= v->cap) return 0;      // 已经够大，什么都不做

    size_t newcap = (v->cap == 0) ? 8 : v->cap;
    while (newcap < need) newcap *= 2; // 2 倍增长

    int *tmp = realloc(v->data, newcap * sizeof(*tmp));
    if (tmp == NULL) return -1;        // 失败时 v->data 仍然有效！

    v->data = tmp;
    v->cap  = newcap;
    return 0;
}

int vec_push(IntVec *v, int value)
{
    if (v == NULL) return -1;
    if (v->size == v->cap) {
        if (vec_reserve(v, v->size + 1) != 0) return -1;
    }
    v->data[v->size] = value;
    v->size++;
    return 0;
}

int vec_pop(IntVec *v, int *out)
{
    if (v == NULL || v->size == 0) return -1;
    v->size--;
    if (out != NULL) *out = v->data[v->size];
    return 0;
}

int vec_get(const IntVec *v, size_t index, int *out)
{
    if (v == NULL || out == NULL) return -1;
    if (index >= v->size) return -1;
    *out = v->data[index];
    return 0;
}

int vec_copy_to(const IntVec *v, int *dest, size_t dest_cap)
{
    if (v == NULL || dest == NULL) return -1;
    if (dest_cap < v->size) return -1;
    for (size_t i = 0; i < v->size; i++) dest[i] = v->data[i];
    return 0;
}

  --- 必须理解的三件事 ---

  1. realloc 的正确写法为什么必须用临时变量？

       错误：v->data = realloc(v->data, newcap * sizeof(int));
         如果 realloc 失败，它返回 NULL，原来的内存块「还在」但地址已经没人记得了
         —— 这块内存永远回收不了，这就是内存泄漏。

       正确：int *tmp = realloc(v->data, ...);
             if (tmp == NULL) return -1;   // v->data 原封不动，调用方还能继续用
             v->data = tmp;

     这是 C 面试必考、也是嵌入式代码 review 必查的一条。

  2. 为什么扩容要「2 倍」而不是「每次 +1」？

       +1 策略：插第 n 个元素要搬 n 次，总共搬 1+2+...+n = O(n²) 次。
       2 倍策略：总共搬 1+2+4+...+n ≈ 2n 次，均摊到每次 push 是 O(1)。
       代价是平均浪费不到一半的空间。这是时间换空间的经典权衡。
       （所以 C++ 的 std::vector、Java 的 ArrayList 都是这么做的。）

  3. 为什么 vec_free 之后要把 data 置 NULL、size/cap 归零？

       因为 C 里没有任何机制阻止你「二次释放」。
          free(p); free(p);        // 第二次是 UB，可能损坏堆结构
       但如果 free 之后立刻 p = NULL，那么 free(NULL) 是标准保证的安全操作，
       二次释放就变成了无害的空操作。
       这叫「让错误尽早暴露」——宁可立刻崩在 NULL 上，也不要三小时后随机崩。
 * ========================================================================== */
