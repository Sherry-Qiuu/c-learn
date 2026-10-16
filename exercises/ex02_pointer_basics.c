/* ============================================================================
 * ex02_pointer_basics.c —— 指针基础：取地址 · 解引用 · 步长
 * ----------------------------------------------------------------------------
 * 目标（C++ 程序员最容易在这一步翻车，因为 C 里没有任何东西替你兜底）：
 *   1. 明白「指针就是一个装着地址的整数」，*p 和 p 是两回事
 *   2. 用指针写输出参数（C 没有引用，全靠这个）
 *   3. 亲眼看到「指针 +1 到底加多少字节」由类型决定
 *
 *   make run/ex02
 * ========================================================================== */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

/* ---------------------------------------------------------------------------
 * TODO 1：交换两个 int
 *   C++ 里你写 void swap(int& a, int& b)，C 里必须传地址。
 * ------------------------------------------------------------------------- */
void swap_int(int *a, int *b)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 2：返回指向数组中最大元素的指针（不是最大值，是「它的地址」）
 *
 *   期望：传入 {3, 9, 2, 9, 1}，返回的指针解引用后是 9
 *   要求：n == 0 时返回 NULL
 *   要求：如果有多个相同的最大值，返回第一个的地址
 * ------------------------------------------------------------------------- */
int *find_max(int *arr, size_t n)
{
    /* 在这里写你的代码 */
    return NULL;
}

/* ---------------------------------------------------------------------------
 * TODO 3：把数组每个元素都 +1
 *
 *   要求：必须用「指针遍历」的写法，不许用 a[i] 下标。
 *         指针版本：for (int *p = arr; p < arr + n; p++) ...
 *   想一想：p < arr + n 这个比较，为什么能成立（提示：同一个数组内的指针比较是合法的）
 * ------------------------------------------------------------------------- */
void increment_all(int *arr, size_t n)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 4：用「输出参数」模拟返回多个值
 *
 *   把数组里的最小值和最大值分别写进 *out_min 和 *out_max。
 *   要求：n == 0 时不做任何修改；n >= 1 时两个输出都有效。
 *
 *   这就是 C 里唯一的「多返回值」手段——传地址进来。
 *   STM32 的 HAL 库里到处都是这种写法，比如：
 *     HAL_StatusTypeDef HAL_GPIO_ReadPin(GPIO_TypeDef *port, uint16_t pin);
 *     HAL_StatusTypeDef HAL_ADC_GetValue(...);
 * ------------------------------------------------------------------------- */
void min_max(const int *arr, size_t n, int *out_min, int *out_max)
{
    /* 在这里写你的代码 */
}

int main(void)
{
    printf("========== 任务 1：交换 ==========\n");
    {
        int x = 10, y = 20;
        swap_int(&x, &y);
        assert(x == 20 && y == 10);
        printf("  swap_int(&x, &y) -> x=%d y=%d  OK\n", x, y);

        /* 再体会一次：下面这样写什么都不会发生（C++ 里的经典 bug） */
        /* swap_int(x, y);   <-- 这里连编译都会报错，比 C++ 里更难写错 */
    }

    printf("\n========== 任务 2：返回指针 ==========\n");
    {
        int a[5] = {3, 9, 2, 9, 1};
        int *p = find_max(a, 5);
        assert(p != NULL);
        assert(*p == 9);
        assert(p == &a[1]);   /* 要的是第一个最大值的地址 */
        printf("  find_max 返回偏移 = %td，值 = %d  OK\n", p - a, *p);

        assert(find_max(a, 0) == NULL);
        printf("  n=0 时返回 NULL  OK\n");
    }

    printf("\n========== 任务 3：指针遍历 ==========\n");
    {
        int a[5] = {1, 2, 3, 4, 5};
        increment_all(a, 5);
        for (int i = 0; i < 5; i++) assert(a[i] == i + 2);
        printf("  increment_all 通过  OK\n");
    }

    printf("\n========== 任务 4：输出参数 ==========\n");
    {
        const int a[6] = {7, -3, 42, 0, 19, -8};
        int lo = 0, hi = 0;
        min_max(a, 6, &lo, &hi);
        assert(lo == -8 && hi == 42);
        printf("  min = %d, max = %d  OK\n", lo, hi);

        int unchanged = 123;
        min_max(a, 0, &unchanged, &unchanged);
        assert(unchanged == 123);
        printf("  n=0 时不修改输出  OK\n");
    }

    printf("\n========== 任务 5：指针 +1 到底加多少 ==========\n");
    {
        int  ai[4];
        char ac[4];
        double ad[4];

        printf("  int*    +1 = %td 字节   (sizeof(int)    = %zu)\n",
               (char *)&ai[1] - (char *)&ai[0], sizeof(int));
        printf("  char*   +1 = %td 字节   (sizeof(char)   = %zu)\n",
               (char *)&ac[1] - (char *)&ac[0], sizeof(char));
        printf("  double* +1 = %td 字节   (sizeof(double) = %zu)\n",
               (char *)&ad[1] - (char *)&ad[0], sizeof(double));

        assert((char *)&ai[1] - (char *)&ai[0] == (ptrdiff_t)sizeof(int));
        assert((char *)&ac[1] - (char *)&ac[0] == (ptrdiff_t)sizeof(char));
        assert((char *)&ad[1] - (char *)&ad[0] == (ptrdiff_t)sizeof(double));
        printf("  「步长由类型决定」得证  OK\n");

        /* 顺便：指针相减得到的是「元素个数」，不是字节数 */
        int arr[10];
        int *q = &arr[7], *r = &arr[2];
        printf("  &arr[7] - &arr[2] = %td（元素个数，不是字节数）\n", q - r);
        assert(q - r == 5);
    }

    printf("\n========== 任务 6：悬垂指针（看着，别模仿） ==========\n");
    {
        /* 危险示范：函数返回后，栈帧消失，返回的地址就失效了。
         * 下面这行是 UB，可能打印垃圾、可能崩，也可能「看起来正常」——
         * 后者最可怕，因为它在单片机上会在半夜随机发作。
         *
         *   int *bad(void) { int local = 42; return &local; }   // 永远别这么写
         *
         * 正确做法：返回 malloc 的内存，或者让调用方传缓冲区进来。
         */
        int heap_value = 99;
        int *ok = &heap_value;     /* 指向仍然活着的对象，安全 */
        assert(*ok == 99);
        printf("  悬垂指针示例已写在注释里，记住结论：不返回局部变量地址\n");
    }

    printf("\n>>> ex02 全部通过。下一站：ex03_array_decay.c\n");
    return 0;
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

void swap_int(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int *find_max(int *arr, size_t n)
{
    if (n == 0 || arr == NULL) return NULL;
    int *best = arr;
    for (size_t i = 1; i < n; i++) {
        if (arr[i] > *best) best = &arr[i];
    }
    return best;
}

void increment_all(int *arr, size_t n)
{
    for (int *p = arr; p < arr + n; p++) {
        (*p)++;
    }
}

void min_max(const int *arr, size_t n, int *out_min, int *out_max)
{
    if (n == 0 || arr == NULL) return;
    int lo = arr[0], hi = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < lo) lo = arr[i];
        if (arr[i] > hi) hi = arr[i];
    }
    *out_min = lo;
    *out_max = hi;
}

  几个要点：

  1. increment_all 里为什么写 (*p)++ 而不是 *p++ ？
       *p++   等价于 *(p++)：先取 p 指向的值，再把 p 往后移一格，值没变
       (*p)++ 才是「把 p 指向的那个 int 加一」
     这一对在笔试里出现频率极高，也是嵌入式里最常见的低级 bug。

  2. p < arr + n 为什么合法？
     C 标准保证：指向同一数组内的指针可以比较大小。
     arr + n 指向「末尾后一格」，是合法的「past-the-end 指针」，
     可以比较、可以参与算术，但不允许解引用（*（arr+n) 是 UB）。

  3. 为什么要传 const int *arr 而不是 int *arr？
     函数不打算修改数组，加 const 有三大好处：
       - 调用者可以传 const 数组进来
       - 编译器能发现你手滑写了 arr[i] = ...
       - 读代码的人一眼知道这个参数是只读的
     嵌入式里读寄存器的地址永远是 const，写寄存器才不是。

  4. 输出参数的写法为什么是 *out_min = lo 而不是 out_min = lo ？
     因为 out_min 是 int*，它的值是一个地址。
     写 out_min = lo 会去改「指针本身」，而且类型不匹配编译器会拦。
     *out_min = lo 才是「写进它指的那块内存」。
 * ========================================================================== */
