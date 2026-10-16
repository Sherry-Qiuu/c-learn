/* ============================================================================
 * ex09_function_pointer.c —— 函数指针：C 里唯一的「多态」
 * ----------------------------------------------------------------------------
 * C 没有虚函数、没有 std::function、没有 lambda。
 * 想在运行时决定「调用哪个函数」，只有一条路：函数指针。
 *
 * 它也是中断向量表、回调机制、驱动注册、状态机的底层实现。
 * 学完这一课，你以后看 STM32 的 HAL 库、FreeRTOS 的源码都不会再发懵。
 *
 *   make run/ex09
 * ========================================================================== */

#include <stdio.h>
#include <stddef.h>
#include <assert.h>

/* ---------------------------------------------------------------------------
 * 先认清函数指针的语法。这可能是 C 里最丑的声明了：
 *
 *   int (*fn)(int, int);
 *   ↑   ↑    ↑
 *   |   |    └── 参数列表：两个 int
 *   |   └─────── 括号：表示 fn 先和 * 结合，fn 是一个指针
 *   └─────────── 返回类型：int
 *
 * 如果没有那个括号：int *fn(int, int) 就变成「返回 int* 的函数声明」了。
 * 记忆法：函数指针的括号，和 ex04 里 int (*p)[4] 的括号是同一个道理。
 * ------------------------------------------------------------------------- */
typedef int (*binop_t)(int, int);

/* ---------------------------------------------------------------------------
 * 四个运算函数。注意它们的签名必须完全一致，才能装进同一个函数指针类型。
 * ------------------------------------------------------------------------- */
static int op_add(int a, int b) { return a + b; }
static int op_sub(int a, int b) { return a - b; }
static int op_mul(int a, int b) { return a * b; }

static int op_div(int a, int b)
{
    if (b == 0) return 0;      /* 除零保护：嵌入式的除法往往没有硬件异常，得自己判 */
    return a / b;
}

/* ---------------------------------------------------------------------------
 * TODO 1：调用传进来的函数
 *   apply(3, 4, op_add) 应该返回 7
 *   apply(10, 3, op_div) 应该返回 3
 * ------------------------------------------------------------------------- */
int apply(int a, int b, binop_t fn)
{
    /* 在这里写你的代码 */
    return 0;
}

/* ---------------------------------------------------------------------------
 * TODO 2：根据名字找到对应的运算函数，找不到返回 NULL
 *   支持 "+" "-" "*" "/"
 *   提示：用一张静态表（名字 + 函数指针），循环查——不要写四个 if。
 *        这叫表驱动（table-driven），是嵌入式里最常用的设计手法：
 *        加新运算只需要往表里加一行。
 * ------------------------------------------------------------------------- */
binop_t op_lookup(const char *name)
{
    /* 在这里写你的代码 */
    return NULL;
}

/* ---------------------------------------------------------------------------
 * TODO 3：回调遍历
 *   对数组每个元素调用一次 fn，注意 fn 收到的是指针，所以它可以修改元素。
 *   要求：n == 0 或 arr == NULL 时安全返回，不做任何事。
 * ------------------------------------------------------------------------- */
void for_each(int *arr, size_t n, void (*fn)(int *elem))
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 4：中断向量表的模拟
 *
 *   表里每一项是「名字 + 处理函数」。dispatch 时按名字找到并调用它。
 *   找不到就调用 fallback（如果 fallback 是 NULL 就什么也不做）。
 *
 *   真实 STM32 里向量表是「按中断号索引的数组」，这里是按名字查，
 *   原理完全一样 —— 都是「运行时通过一个函数指针发起调用」。
 * ------------------------------------------------------------------------- */
typedef struct {
    const char *name;
    void (*handler)(void);
} IrqEntry;

void irq_dispatch(const IrqEntry *table, size_t n, const char *irq,
                  void (*fallback)(void))
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 5：用函数指针实现一个简单状态机
 *
 *   每个状态是一个函数：收到事件，返回下一个状态。
 *   这是嵌入式里「按键处理 / 协议解析 / UI 菜单」的标准写法，
 *   写起来比一长串 switch-case 干净得多，也好扩展。
 * ------------------------------------------------------------------------- */
typedef enum {
    ST_IDLE = 0,
    ST_RUN,
    ST_ERROR
} state_t;

typedef enum {
    EV_START = 0,
    EV_STOP,
    EV_FAIL
} event_t;

static state_t st_idle_handler(event_t ev);
static state_t st_run_handler(event_t ev);
static state_t st_error_handler(event_t ev);

/* 状态表：把「状态」映射到「处理函数」，这就是表驱动状态机 */
static state_t (*const STATE_TABLE[])(event_t) = {
    st_idle_handler,
    st_run_handler,
    st_error_handler,
};

static state_t st_idle_handler(event_t ev)
{
    if (ev == EV_START) return ST_RUN;
    return ST_IDLE;
}

static state_t st_run_handler(event_t ev)
{
    if (ev == EV_STOP)  return ST_IDLE;
    if (ev == EV_FAIL)  return ST_ERROR;
    return ST_RUN;
}

static state_t st_error_handler(event_t ev)
{
    if (ev == EV_STOP) return ST_IDLE;
    return ST_ERROR;
}

/* TODO：用 STATE_TABLE 实现「喂一个事件，返回新状态」 */
state_t sm_feed(state_t cur, event_t ev)
{
    /* 在这里写你的代码 */
    return cur;
}

/* --- 任务 4 用到的两个假中断处理函数 --- */
static int g_uart_count = 0;
static int g_tim_count  = 0;

static void on_uart(void) { g_uart_count++; printf("    [ISR] UART 中断，第 %d 次\n", g_uart_count); }
static void on_tim(void)  { g_tim_count++;  printf("    [ISR] TIM  中断，第 %d 次\n", g_tim_count); }
static void on_default(void){ printf("    [ISR] 未知中断，走兜底处理\n"); }

/* --- 任务 3 用的回调 --- */
static void cb_times_two(int *elem) { *elem *= 2; }
static void cb_add_ten(int *elem)   { *elem += 10; }

/* 前置声明：atoi_simple 的实现在文件末尾，这里先告诉编译器它长什么样。
 * C 里「先声明、后使用」是硬规则（C99 起不再允许隐式声明）。 */
static int atoi_simple(const char *s);

int main(void)
{
    printf("========== 任务 1：把函数当参数传 ==========\n");
    {
        assert(apply(3, 4, op_add) == 7);
        assert(apply(3, 4, op_sub) == -1);
        assert(apply(3, 4, op_mul) == 12);
        assert(apply(10, 3, op_div) == 3);
        assert(apply(10, 0, op_div) == 0);      /* 除零被保护 */
        printf("  apply 四种运算全部通过  OK\n");

        /* 换个角度看：函数名本身就是一个地址 */
        printf("\n  op_add 的地址 = %p\n", (void *)(size_t)&op_add);
        printf("  函数指针存的就是这个地址，所以它也是「指针」\n");
    }

    printf("\n========== 任务 2：表驱动查函数 ==========\n");
    {
        assert(op_lookup("+") == op_add);
        assert(op_lookup("-") == op_sub);
        assert(op_lookup("*") == op_mul);
        assert(op_lookup("/") == op_div);
        assert(op_lookup("%") == NULL);
        assert(op_lookup("") == NULL);
        printf("  查表全部通过  OK\n");

        const char *exprs[][2] = {
            { "12", "+" }, { "5", "*" }, { "3", "-" }, { "4", "/" },
        };
        printf("\n  用查表做一个迷你计算器：\n");
        for (size_t i = 0; i < 4; i++) {
            binop_t fn = op_lookup(exprs[i][1]);
            if (fn == NULL) { printf("    不认识的运算符 %s\n", exprs[i][1]); continue; }
            printf("    10 %s %s = %d\n", exprs[i][1], exprs[i][0], apply(10, atoi_simple(exprs[i][0]), fn));
        }
        printf("  注意：这里没有一行 if-else 用来选运算符，全靠一张表。\n");
        printf("  以后加一个幂运算，只要往表里加一行 op_pow。\n");
    }

    printf("\n========== 任务 3：回调遍历 ==========\n");
    {
        int a[5] = {1, 2, 3, 4, 5};
        for_each(a, 5, cb_times_two);
        for (int i = 0; i < 5; i++) assert(a[i] == (i + 1) * 2);
        printf("  cb_times_two -> {2,4,6,8,10}  OK\n");

        for_each(a, 5, cb_add_ten);
        for (int i = 0; i < 5; i++) assert(a[i] == (i + 1) * 2 + 10);
        printf("  cb_add_ten   -> {12,14,16,18,20}  OK\n");

        for_each(NULL, 5, cb_times_two);   /* 空指针保护 */
        for_each(a, 0, cb_times_two);      /* 空数组保护 */
        printf("  空指针 / 空数组不崩  OK\n");
    }

    printf("\n========== 任务 4：中断向量表 ==========\n");
    {
        const IrqEntry vector_table[] = {
            { "UART", on_uart },
            { "TIM",  on_tim  },
        };
        const size_t n = sizeof vector_table / sizeof vector_table[0];

        printf("  模拟触发几次「中断」：\n");
        irq_dispatch(vector_table, n, "UART", on_default);
        irq_dispatch(vector_table, n, "TIM",  on_default);
        irq_dispatch(vector_table, n, "UART", on_default);
        irq_dispatch(vector_table, n, "I2C",  on_default);
        irq_dispatch(vector_table, n, NULL,   on_default);

        assert(g_uart_count == 2);
        assert(g_tim_count == 1);
        printf("  UART=%d 次, TIM=%d 次, 未知走 fallback  OK\n", g_uart_count, g_tim_count);

        printf("\n  这就是中断的本质：硬件填一个编号，CPU 查表跳过去。\n");
        printf("  真实 STM32 的向量表在启动文件 startup_stm32xxxx.s 里，\n");
        printf("  每个中断号对应一个入口地址，本质就是这份 const IrqEntry。\n");
    }

    printf("\n========== 任务 5：函数指针状态机 ==========\n");
    {
        state_t s = ST_IDLE;

        s = sm_feed(s, EV_START);  assert(s == ST_RUN);
        printf("  IDLE --START--> RUN\n");

        s = sm_feed(s, EV_START);  assert(s == ST_RUN);   /* RUN 下 START 无效果 */
        printf("  RUN  --START--> RUN（无效果）\n");

        s = sm_feed(s, EV_FAIL);   assert(s == ST_ERROR);
        printf("  RUN  --FAIL --> ERROR\n");

        s = sm_feed(s, EV_FAIL);   assert(s == ST_ERROR);
        printf("  ERROR--FAIL --> ERROR（无效果）\n");

        s = sm_feed(s, EV_STOP);   assert(s == ST_IDLE);
        printf("  ERROR--STOP --> IDLE\n");

        printf("\n  这个状态机只用了一张函数指针表就叫起来了。\n");
        printf("  换成 8 个状态、6 个事件？加函数、加表项，switch 一行都不用改。\n");
    }

    printf("\n========== 任务 6：对比一下 C++ 的写法 ==========\n");
    {
        printf("  C++ 里你会写：\n");
        printf("    std::function<int(int,int)> fn = op_add;\n");
        printf("    fn = [](int a, int b){ return a + b; };   // lambda\n");
        printf("    还有 std::map<std::string, std::function<...>> 做表驱动\n\n");

        printf("  C 里只有函数指针：\n");
        printf("    binop_t fn = op_add;                       // 不能捕获，没有 lambda\n");
        printf("    表驱动要自己搭一个 struct 数组\n\n");

        printf("  C 的一个隐性优势：函数指针没有额外开销，\n");
        printf("  就是一条间接跳转指令；lambda 和 std::function 会有闭包分配。\n");
        printf("  在只有几十 KB RAM 的单片机上，这个差别很重要。\n");
    }

    printf("\n========== 任务 7：函数指针的坑 ==========\n");
    {
        printf("  【坑 1】typedef 少了括号：\n");
        printf("     typedef int (*binop_t)(int,int);   // 对：函数指针\n");
        printf("     typedef int *binop_t(int,int);     // 错：返回 int* 的函数类型\n\n");

        printf("  【坑 2】调用时忘了括号：\n");
        printf("     binop_t fn = op_add;\n");
        printf("     fn(1,2);      // 对\n");
        printf("     (*fn)(1,2);   // 也可以，但没必要——C 允许省略 *\n");
        printf("     *fn(1,2);     // 错：会被解析成 *(fn(1,2))，拿返回值当指针解引用\n\n");

        printf("  【坑 3】调用 NULL 函数指针：\n");
        printf("     和空指针解引用一样是 UB，在 STM32 上直接 HardFault。\n");
        printf("     所以查表之后一定要判 NULL，就像任务 2 里做的那样。\n\n");

        printf("  【坑 4】const 的位置：\n");
        printf("     const IrqEntry *table        表内容和指针都可只读\n");
        printf("     和 ex04 学的规则一模一样。\n");
    }

    printf("\n>>> ex09 全部通过。下一站：ex10_mmio_volatile.c（STM32 预演）\n");
    return 0;
}

/* 小工具：把 "123" 转成 int。ex05 学过，这里直接内联一份用 */
static int atoi_simple(const char *s)
{
    int v = 0;
    while (*s >= '0' && *s <= '9') { v = v * 10 + (*s - '0'); s++; }
    return v;
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

int apply(int a, int b, binop_t fn)
{
    if (fn == NULL) return 0;
    return fn(a, b);
}

binop_t op_lookup(const char *name)
{
    static const struct {
        const char *name;
        binop_t     fn;
    } table[] = {
        { "+", op_add },
        { "-", op_sub },
        { "*", op_mul },
        { "/", op_div },
    };

    if (name == NULL) return NULL;

    for (size_t i = 0; i < sizeof table / sizeof table[0]; i++) {
        const char *a = table[i].name;
        const char *b = name;
        while (*a != '\0' && *a == *b) { a++; b++; }
        if (*a == '\0' && *b == '\0') return table[i].fn;
    }
    return NULL;
}

void for_each(int *arr, size_t n, void (*fn)(int *elem))
{
    if (arr == NULL || fn == NULL) return;
    for (size_t i = 0; i < n; i++) {
        fn(&arr[i]);
    }
}

void irq_dispatch(const IrqEntry *table, size_t n, const char *irq,
                  void (*fallback)(void))
{
    if (table != NULL && irq != NULL) {
        for (size_t i = 0; i < n; i++) {
            const char *a = table[i].name;
            const char *b = irq;
            while (*a != '\0' && *a == *b) { a++; b++; }
            if (*a == '\0' && *b == '\0') {
                table[i].handler();
                return;
            }
        }
    }
    if (fallback != NULL) fallback();
}

state_t sm_feed(state_t cur, event_t ev)
{
    size_t n = sizeof STATE_TABLE / sizeof STATE_TABLE[0];
    if ((size_t)cur >= n) return cur;
    return STATE_TABLE[cur](ev);
}

  --- 几个要点 ---

  1. 为什么状态表要写成 static state_t (*const STATE_TABLE[])(event_t) ？

       从里往外读：
         STATE_TABLE 是数组 []
         数组的元素是「指向函数的指针」 (*...)
         那个函数接收 event_t，返回 state_t
       加 const 是告诉编译器「这张表运行时不会变」，
       于是它可以直接被放在 ROM（Flash）里，不占 SRAM。
       嵌入式里凡是常量表都应该加 const —— 这能省下珍贵的 RAM。

  2. 为什么用表驱动而不是 switch-case ？

       表驱动的好处：
         - 加新功能只加数据，不改逻辑（开闭原则）
         - 状态和行为的对应关系一眼可见
         - 不会有「漏写一个 case」的问题
       代价：多一次内存访问，可能多一点点 cache 不命中。
       在单片机上这点开销完全可以接受，可读性的收益更大。

  3. op_add 这种小函数为什么写成 static ？

       static 修饰函数 = 只在本文件可见，不会污染全局符号表。
       嵌入式工程里所有的「私有函数」都该是 static，
       这也是为了让链接器能报出「定义了两个同名函数」这类错误。

  4. 函数指针的地址能打印吗？

       可以，但要先转成 void*：
         printf("%p", (void *)&op_add);
       直接把函数指针传给 %p 严格来说是 UB（函数指针和对象指针不保证同宽）。
       本练习里我多转了一层 (size_t) 是为了避免 MinGW 的类型警告。

   === 嵌入式关联 ===
   FreeRTOS 里每个任务就是一个函数指针；STM32 的 HAL 里
   HAL_UART_RegisterCallback 注册的也是函数指针；
   LVGL 的事件回调、FatFs 的磁盘 IO 接口，全是这一套。
   可以说：不懂函数指针，就读不懂任何嵌入式库。
 * ========================================================================== */
