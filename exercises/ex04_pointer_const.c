/* ============================================================================
 * ex04_pointer_const.c —— const 的四种位置 · 指针数组 vs 数组指针 · 二级指针
 * ----------------------------------------------------------------------------
 * C 里 const 的位置决定「谁不能改」，读法是「从变量名往外、从右往左」。
 * 这个练习做完，你看到 STM32 头文件里那些
 *   volatile uint32_t * const p;
 * 就不会再发愣了。
 *   make run/ex04
 * ========================================================================== */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* ---------------------------------------------------------------------------
 * TODO 1：遍历一个「字符串数组」并打印
 *
 *   names 的类型：const char * const *names
 *     从右往左读：names 是指针 -> 指向 const 指针 -> 那个指针指向 const char
 *     人话：names 自己不能改，names[i] 也不能改，names[i][j] 更读不了只能读
 *
 *   期望输出（每行一个，带序号）：
 *     1) foo
 *     2) bar
 *     3) baz
 * ------------------------------------------------------------------------- */
void print_names(const char *const *names, size_t n)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 2：用二级指针做「输出参数」，分配一块内存并填值
 *
 *   返回 0 表示成功（并且 *out 指向新分配的内存，调用方负责 free）
 *   返回 -1 表示失败（n == 0 或 malloc 失败；此时不许改动 *out）
 *
 *   为什么必须传 int** 而不是 int*？
 *     因为函数要修改「调用方那个指针变量本身」。
 *     传 int* 只能改它指向的内容，改不了它自己。
 *     这和 TODO 1 的 swap_ptr 是同一件事。
 * ------------------------------------------------------------------------- */
int alloc_fill(int **out, size_t n, int value)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ---------------------------------------------------------------------------
 * TODO 3：交换两个指针本身（不是交换它们指向的内容！）
 *   期望：p 和 q 指向的对象互换
 * ------------------------------------------------------------------------- */
void swap_ptr(int **a, int **b)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 4：统计字符串里某个字符出现的次数
 *   注意参数是 const char *——函数承诺不改内容。
 *   空指针要返回 0，别崩。
 * ------------------------------------------------------------------------- */
size_t count_char(const char *s, char c)
{
    /* 在这里写你的代码 */
    return 0;
}

/* ---------------------------------------------------------------------------
 * TODO 5：把一个字符串的「前 n 个字符」反向写进 out（不改原串）
 *   期望：copy_reversed("hello", 5, buf, sizeof buf)  -> buf == "olleh"
 *   要求：out 必须被 '\0' 结尾；n 超过实际长度时按实际长度处理
 * ------------------------------------------------------------------------- */
void copy_reversed(const char *s, size_t n, char *out, size_t outsz)
{
    /* 在这里写你的代码 */
}

int main(void)
{
    printf("========== 任务 1：字符串数组 ==========\n");
    {
        const char *names[] = { "foo", "bar", "baz" };
        print_names(names, 3);
    }

    printf("\n========== 任务 1b：const 四种位置，逐个说清楚 ==========\n");
    {
        char       buf[]  = "hello";
        char       buf2[] = "world";
        char *const p1 = buf;              /* p1 不能改指向，内容能改 */
        const char *p2 = buf;              /* p2 能改指向，内容不能改 */
        const char *const p3 = buf;        /* 两个都不能改 */
        char *p4 = buf;                    /* 两个都能改 */

        p1[0] = 'H';                       /* OK：改的是内容 */
        p2 = buf2;                         /* OK：改的是指向 */

        (void)p3;
        (void)p4;

        /* 下面两行如果取消注释，编译器会拦住你——这正是 const 的价值： */
        /* p1 = buf2;       */            /* error: assignment of read-only variable 'p1' */
        /* p2[0] = 'W';     */            /* error: assignment of read-only location */

        printf("  p1 指向的内容改成了 \"%s\"\n", p1);
        printf("  p2 现在指向        \"%s\"\n", p2);
        printf("  const 的位置 = 「谁不能改」，编译器帮你站岗\n");
    }

    printf("\n========== 任务 2：二级指针输出参数 ==========\n");
    {
        int *buf = NULL;
        assert(alloc_fill(&buf, 5, 7) == 0);
        assert(buf != NULL);
        for (int i = 0; i < 5; i++) assert(buf[i] == 7);
        printf("  alloc_fill(&buf, 5, 7) 成功，buf[0..4] == 7  OK\n");
        free(buf);

        int sentinel = 42;
        int *untouched = &sentinel;       /* 用一个已知的地址，验证「不许动它」 */
        assert(alloc_fill(&untouched, 0, 1) == -1);
        assert(untouched == &sentinel);
        printf("  n=0 时返回 -1 且不改动 *out  OK\n");
    }

    printf("\n========== 任务 3：交换指针本身 ==========\n");
    {
        int x = 1, y = 2;
        int *p = &x, *q = &y;
        swap_ptr(&p, &q);
        assert(*p == 2 && *q == 1);
        assert(p == &y && q == &x);
        printf("  swap_ptr(&p, &q) -> *p=%d *q=%d  OK\n", *p, *q);
    }

    printf("\n========== 任务 4：只读字符串遍历 ==========\n");
    {
        const char *s = "the quick brown fox jumps over the lazy dog";
        assert(count_char(s, 'o') == 4);
        assert(count_char(s, 'z') == 1);
        assert(count_char(s, 'Q') == 0);
        assert(count_char(NULL, 'x') == 0);
        assert(count_char("", 'x') == 0);
        printf("  count_char 全部通过  OK\n");
    }

    printf("\n========== 任务 5：反向复制 ==========\n");
    {
        char buf[16];
        copy_reversed("hello", 5, buf, sizeof buf);
        assert(strcmp(buf, "olleh") == 0);
        printf("  \"hello\" -> \"%s\"  OK\n", buf);

        copy_reversed("hello", 100, buf, sizeof buf);   /* n 超长 */
        assert(strcmp(buf, "olleh") == 0);
        printf("  n 超长时按实际长度处理    OK\n");

        copy_reversed("abc", 3, buf, sizeof buf);
        assert(strcmp(buf, "cba") == 0);
        printf("  \"abc\" -> \"%s\"  OK\n", buf);
    }

    printf("\n========== 任务 6：指针数组 vs 数组指针（最容易搞混的一对） ==========\n");
    {
        int  a = 1, b = 2, c = 3, d = 4;

        int  *pa[4] = { &a, &b, &c, &d };   /* 指针数组：4 个 int*    */
        int   m[3][4] = {{0}};
        int (*pb)[4] = m;                   /* 数组指针：1 个指针，指向 int[4] */

        printf("  int *pa[4]    : sizeof = %zu  （4 个指针 = 32 字节）\n", sizeof(pa));
        printf("  int (*pb)[4]  : sizeof = %zu  （就是 1 个指针）\n", sizeof(pb));
        printf("  pa + 1 跳 %td 字节  （跳过一个指针）\n",
               (char *)(pa + 1) - (char *)pa);
        printf("  pb + 1 跳 %td 字节  （跳过一整行 4 个 int）\n",
               (char *)(pb + 1) - (char *)pb);

        assert(sizeof(pa) == 4 * sizeof(int *));
        assert(sizeof(pb) == sizeof(int *));
        assert((char *)(pb + 1) - (char *)pb == (ptrdiff_t)(4 * sizeof(int)));

        printf("\n  记忆法：\n");
        printf("    int *pa[4]    先看 [] —— 先是数组，装的东西是 int*  -> 指针数组\n");
        printf("    int (*pb)[4]  先看 () —— 先是指针，指向 int[4]      -> 数组指针\n");
    }

    printf("\n>>> ex04 全部通过。下一站：ex05_string_manual.c（手写字符串库）\n");
    return 0;
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

void print_names(const char *const *names, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        printf("  %zu) %s\n", i + 1, names[i]);
    }
}

int alloc_fill(int **out, size_t n, int value)
{
    if (out == NULL || n == 0) return -1;
    int *p = malloc(n * sizeof(*p));     // 注意 sizeof(*p)，不是 sizeof(int)
    if (p == NULL) return -1;
    for (size_t i = 0; i < n; i++) p[i] = value;
    *out = p;                            // 这一行才是「改调用方的指针」
    return 0;
}

void swap_ptr(int **a, int **b)
{
    int *t = *a;
    *a = *b;
    *b = t;
}

size_t count_char(const char *s, char c)
{
    if (s == NULL) return 0;
    size_t n = 0;
    for (const char *p = s; *p != '\0'; p++) {
        if (*p == c) n++;
    }
    return n;
}

void copy_reversed(const char *s, size_t n, char *out, size_t outsz)
{
    if (out == NULL || outsz == 0) return;
    if (s == NULL) { out[0] = '\0'; return; }

    size_t len = 0;
    while (s[len] != '\0') len++;        // 相当于 strlen，先手写一遍
    if (n > len) n = len;

    if (n > outsz - 1) n = outsz - 1;    // 防止写爆 out

    for (size_t i = 0; i < n; i++) {
        out[i] = s[n - 1 - i];
    }
    out[n] = '\0';                       // 千万别忘了这个
}

  为什么 malloc 里写 sizeof(*p) 而不是 sizeof(int) ？
    以后你把 p 的类型从 int* 改成 long*，这行代码一个字都不用动。
    这种写法叫「以变量为准」，是 C 社区公认的好习惯，
    在嵌入式里尤其重要——因为嵌入式经常要换平台、换类型宽度。

  为什么 alloc_fill 要先判断 malloc 是否返回 NULL ？
    PC 上内存充足，几乎不会失败；单片机上堆只有几 KB，失败是常态。
    养成「凡是 malloc 必判 NULL」的习惯，能省掉未来无数个半夜调试。

  为什么 copy_reversed 要同时判 n > outsz - 1 ？
    这是防御式编程：调用方给了一个小缓冲区怎么办？
    strcpy / sprintf 的悲剧就是它们不检查目标大小。
    你自己写的函数，要有「不信任调用方」的自觉。

  为什么遍历字符串用 const char *p = s 而不是 size_t i ？
    两种都对。指针版本少一次「s + i」的加法，也更贴近硬件思维。
    但要注意：字符串遍历必须找到 '\0'，所以是 O(n)，别放在循环条件里反复算。
 * ========================================================================== */
