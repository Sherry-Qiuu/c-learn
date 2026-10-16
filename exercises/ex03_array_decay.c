/* ============================================================================
 * ex03_array_decay.c —— 数组退化 · sizeof 陷阱 · 二维数组与行指针
 * ----------------------------------------------------------------------------
 * 「数组名在表达式里会退化成指向首元素的指针」——这一条是 C 里最多 bug 的源头。
 * 学完这个练习，你应该能在任何一段 C 代码里一眼看出「这里 a 是数组还是指针」。
 *   make run/ex03
 * ========================================================================== */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

/* ---------------------------------------------------------------------------
 * TODO 1：原地反转数组
 *   期望：{1,2,3,4,5} -> {5,4,3,2,1}；  {1,2,3,4} -> {4,3,2,1}；  空数组不动
 *   要求：O(n)、不用额外数组、用两个指针从两端往中间走
 * ------------------------------------------------------------------------- */
void reverse(int *a, size_t n)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 2：求二维数组所有元素之和
 *
 *   参数写法是重点：int (*m)[4] 表示「指向『4 个 int 的数组』的指针」。
 *   m[1][2] 等价于 *(*(m + 1) + 2)。
 *
 *   调用方式：int grid[3][4] = {...};  sum_2d(grid, 3);
 *   注意 grid 退化后类型正是 int (*)[4]，所以能直接传。
 *   如果你把参数写成 int **，编译能过但运行会崩——想想为什么。
 * ------------------------------------------------------------------------- */
int sum_2d(const int (*m)[4], size_t rows)
{
    /* 在这里写你的代码 */
    return 0;
}

/* ---------------------------------------------------------------------------
 * TODO 3：把一个二维数组「按行打印」，每行格式： [a, b, c, d]
 *   提示：printf("[%d, %d, %d, %d]\n", m[i][0], ...) 就够，别过度设计
 * ------------------------------------------------------------------------- */
void print_2d(const int (*m)[4], size_t rows)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 4：把二维数组「拍平」复制到一维数组
 *   期望：把 grid[3][4] 的 12 个数按行序写进 out[12]
 *   这题的意义：在嵌入式里，二维数组在内存里本来就是连续的一维，
 *   理解这点你才能正确地把 DMA 缓冲、图像数据当一维处理。
 * ------------------------------------------------------------------------- */
void flatten_2d(const int (*m)[4], size_t rows, int *out)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 5：搞清楚数组到底什么时候不退化
 *
 *   写一个函数，接收 int *arr 和 size_t n，返回数组的「字节数」。
 *   注意：在这个函数里你算不出真实字节数，因为 arr 已经退化成指针了。
 *   本任务的「正确实现」是：显式告诉调用者这件事，返回 n * sizeof(int)。
 *
 *   然后回到 main 里观察：main 里的 a 用 sizeof 得到的是整个数组的大小，
 *   传进这个函数之后就变成了 8（一个指针）。
 * ------------------------------------------------------------------------- */
size_t array_bytes(const int *arr, size_t n)
{
    /* 在这里写你的代码 */
    return 0;
}

int main(void)
{
    printf("========== 任务 1：原地反转 ==========\n");
    {
        int a[5] = {1, 2, 3, 4, 5};
        reverse(a, 5);
        for (int i = 0; i < 5; i++) assert(a[i] == 5 - i);
        printf("  奇数长度  OK\n");

        int b[4] = {1, 2, 3, 4};
        reverse(b, 4);
        for (int i = 0; i < 4; i++) assert(b[i] == 4 - i);
        printf("  偶数长度  OK\n");

        reverse(b, 0);   /* 空数组不能崩 */
        printf("  空数组    OK\n");
    }

    printf("\n========== 任务 2/3/4：二维数组 ==========\n");
    {
        int grid[3][4] = {
            { 1,  2,  3,  4},
            { 5,  6,  7,  8},
            { 9, 10, 11, 12},
        };

        assert(sum_2d(grid, 3) == 78);
        printf("  求和 = %d  OK\n", sum_2d(grid, 3));

        printf("  按行打印：\n");
        print_2d(grid, 3);

        int flat[12];
        flatten_2d(grid, 3, flat);
        for (int i = 0; i < 12; i++) assert(flat[i] == i + 1);
        printf("  拍平后验证通过  OK\n");

        printf("  另外验证：二维数组在内存里本来就是连续的\n");
        assert((char *)&grid[0][4] == (char *)&grid[1][0]);
        printf("    &grid[0][4] == &grid[1][0] 成立\n");
    }

    printf("\n========== 任务 5：sizeof 的真相 ==========\n");
    {
        int a[10];

        printf("  main 里 sizeof(a)          = %zu   <-- 整个数组\n", sizeof(a));
        printf("  main 里 sizeof(a[0])       = %zu   <-- 第一个元素\n", sizeof(a[0]));
        printf("  main 里 sizeof(a)/sizeof(a[0]) = %zu   <-- 元素个数\n",
               sizeof(a) / sizeof(a[0]));
        printf("  main 里 sizeof(void*)      = %zu   <-- 指针本身\n", sizeof(void *));

        int *p = a;   /* 这一行发生了「数组到指针的退化」 */
        printf("  退化成指针之后 sizeof(p)    = %zu   <-- 只剩一个指针了！\n", sizeof(p));

        assert(sizeof(a) == 10 * sizeof(int));
        assert(sizeof(p) == sizeof(void *));

        printf("  array_bytes(a, 10) = %zu  （显式传长度才算得对）\n",
               array_bytes(a, 10));
        assert(array_bytes(a, 10) == 10 * sizeof(int));

        printf("\n  三条铁律：\n");
        printf("    1. sizeof(数组名)      = 整个数组的字节数\n");
        printf("    2. sizeof(指针)        = 8（64 位平台）\n");
        printf("    3. 数组作为函数参数时，一律当成指针，长度必须另外传\n");
    }

    printf("\n========== 任务 6：不退化 vs 取地址 ==========\n");
    {
        int a[5];
        printf("  a      的类型是 int[5]，sizeof = %zu\n", sizeof(a));
        printf("  a + 0  已退化成 int*，sizeof = %zu（对指针取 sizeof）\n",
               sizeof(a + 0));
        printf("  &a     类型是 int(*)[5]（指向整个数组的指针）\n");
        printf("  &a + 1 会跳过整个数组 = %td 字节\n",
               (char *)(&a + 1) - (char *)&a);
        assert((char *)(&a + 1) - (char *)&a == (ptrdiff_t)sizeof(a));
        printf("  这就是 (a + 1) 和 (&a + 1) 差 4 字节和 20 字节的原因\n");
    }

    printf("\n>>> ex03 全部通过。下一站：ex04_pointer_const.c\n");
    return 0;
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

void reverse(int *a, size_t n)
{
    if (n < 2) return;
    int *lo = a;
    int *hi = a + n - 1;
    while (lo < hi) {
        int t = *lo;
        *lo = *hi;
        *hi = t;
        lo++;
        hi--;
    }
}

int sum_2d(const int (*m)[4], size_t rows)
{
    int total = 0;
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < 4; c++) {
            total += m[r][c];
        }
    }
    return total;
}

void print_2d(const int (*m)[4], size_t rows)
{
    for (size_t r = 0; r < rows; r++) {
        printf("    [%d, %d, %d, %d]\n",
               m[r][0], m[r][1], m[r][2], m[r][3]);
    }
}

void flatten_2d(const int (*m)[4], size_t rows, int *out)
{
    size_t k = 0;
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < 4; c++) {
            out[k++] = m[r][c];
        }
    }
}

size_t array_bytes(const int *arr, size_t n)
{
    (void)arr;                  // 数组已经退化成指针，真实大小只能靠 n 推
    return n * sizeof(int);
}

  为什么参数必须是 int (*m)[4] 而不是 int **m ？
    int **m 的意思是「m 是一个指针，指向一个 int*」，
      要按 m[r][c] 访问，就必须先读 m[r] 得到一个 int*，再解引用。
      而二维数组 int grid[3][4] 的内存里根本没有存放任何指针，
      它就是一整块连续的 12 个 int。
      于是 m[r] 会把「第 r 行的第一个 int 的数值」当成一个地址去解引用 —— 直接崩。
    正确类型 int (*m)[4] 才能让编译器算出 m + r 应该跳 4*sizeof(int) 字节。

  为什么反转要用 lo < hi 而不是 lo != hi ？
    对偶数个元素，lo 永远不可能等于 hi，用 != 就死循环了。
    指针比较一律用 < / <=（同一数组内合法），别用 == / !=。

  为什么 reverse 要先判 n < 2 ？
    n == 0 时算 a + n - 1 会得到 a - 1，虽然不解引用就没 UB，
    但这是「越界指针算术」，标准上已经踩线了。提前返回最干净。
    嵌入式代码的健壮性就是这么一行一行堆出来的。
 * ========================================================================== */
