/* ============================================================================
 * irq.c —— 中断分发器实现
 * ----------------------------------------------------------------------------
 * 这张「向量表」就是 STM32 启动文件里那张表的可测试版本。
 * 你在 ex09 里手写过一次，现在是把它做成一个真正的模块。
 *
 *   make rebuild && make run
 * ========================================================================== */

#include "irq.h"

/* ---------------------------------------------------------------------------
 * 向量表本体。
 *
 * 注意 static：外部无法直接访问这张表，只能通过 irq_attach / irq_trigger。
 * 这叫「封装」——在 C 里靠 static 实现，在 C++ 里靠 private 实现。
 *
 * 表里存的是函数指针，初始全为 NULL（还没人挂处理函数）。
 * ------------------------------------------------------------------------- */
static isr_t  s_vector[IRQ_MAX];

/* 中断号的助记名。真实 STM32 里这些名字在启动文件里定义为弱符号，
 * 比如 EXTI0_IRQHandler / TIM2_IRQHandler。这里用字符串只是为了好打印。 */
static const char *const s_names[IRQ_MAX] = {
    "IRQ0",
    "IRQ1",
    "IRQ2",
    "IRQ3",
    "IRQ4",
    "IRQ5",
    "EXTI6",     /* 6 —— 我们的按键中断 */
    "IRQ7",
};

/* 统计计数 */
static unsigned s_dispatched;
static unsigned s_unhandled;

/* ---------------------------------------------------------------------------
 * 参数检查
 * ------------------------------------------------------------------------- */
static int num_valid(unsigned num)
{
    return num < IRQ_MAX;
}

void irq_init(void)
{
    /* TODO 1：清空整张向量表和两个计数器
     *   要求：用循环把每个槽位置成 NULL（别用 memset —— 函数指针不是字节数组，
     *         虽然实际上等价，但显式循环更清楚，也避免类型双关的争议） */
    /* 在这里写你的代码 */
}

irq_status_t irq_attach(unsigned num, isr_t handler)
{
    /* TODO 2：
     *   num 越界 或 handler == NULL -> IRQ_ERR_INVALID
     *   否则挂上去（覆盖旧值），返回 IRQ_OK
     *
     *   为什么 handler 为 NULL 要当成错误？
     *     因为 irq_detach 才是「摘掉」的语义。传 NULL 进来多半是写错了，
     *     与其静默接受，不如在开发期就报出来。
     */
    /* 在这里写你的代码 */
    return IRQ_ERR_INVALID;
}

irq_status_t irq_detach(unsigned num)
{
    /* TODO 3：num 越界 -> IRQ_ERR_INVALID，否则把槽位置 NULL 并返回 IRQ_OK */
    /* 在这里写你的代码 */
    return IRQ_ERR_INVALID;
}

void irq_trigger(unsigned num)
{
    /* TODO 4：
     *   num 越界 -> 什么都不做（或者计入 unhandled，自己定，注释说明清楚）
     *   槽位是 NULL -> s_unhandled++，什么都不调用
     *   槽位有函数 -> 调用它，s_dispatched++
     *
     *   关键：调用之前必须判 NULL。调用 NULL 函数指针在 STM32 上是 HardFault，
     *         而「某个中断没人处理」是完全正常的运行状态。
     */
    /* 在这里写你的代码 */
}

unsigned irq_dispatch_count(void)
{
    return s_dispatched;
}

unsigned irq_unhandled_count(void)
{
    return s_unhandled;
}

const char *irq_name(unsigned num)
{
    return num_valid(num) ? s_names[num] : "?";
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

void irq_init(void)
{
    for (unsigned i = 0; i < IRQ_MAX; i++) {
        s_vector[i] = NULL;
    }
    s_dispatched = 0;
    s_unhandled  = 0;
}

irq_status_t irq_attach(unsigned num, isr_t handler)
{
    if (!num_valid(num) || handler == NULL) return IRQ_ERR_INVALID;
    s_vector[num] = handler;
    return IRQ_OK;
}

irq_status_t irq_detach(unsigned num)
{
    if (!num_valid(num)) return IRQ_ERR_INVALID;
    s_vector[num] = NULL;
    return IRQ_OK;
}

void irq_trigger(unsigned num)
{
    if (!num_valid(num)) { s_unhandled++; return; }

    isr_t fn = s_vector[num];
    if (fn == NULL) { s_unhandled++; return; }

    fn();
    s_dispatched++;
}

  --- 几个要点 ---

  1. 为什么 isr_t 要 typedef 而不是直接写 void (*)(void) ？

       因为函数指针的原生语法太丑，到处都是 void (*)(void) 会读不下去。
       typedef 之后类型名就是 isr_t，写起来和普通类型一样。
       这也是为什么 STM32 的头文件里有大量 typedef（__IO、uint32_t 都是）。

  2. 为什么 s_vector 和 s_names 都是 static ？

       这样链接器就不会把它们导出到全局符号表。
       好处：
         - 别人不能直接改你的向量表
         - 同名的 s_vector 可以出现在别的模块里（比如你再写一个 irq2.c）
       在 C++ 里这就是 private 成员；C 里没有 class，靠 static 完成同样的隔离。

  3. 为什么真实 STM32 的中断处理函数名字不能随便改？

       因为启动文件里那张向量表是**按名字**引用这些函数的，
       函数名对不上，链接时会报 undefined reference to 'EXTI9_5_IRQHandler'。
       所以你在 CubeMX 生成的工程里看到的那些
       `void EXTI9_5_IRQHandler(void)` 是固定名字，不是随便起的。

  4. 真实中断里绝对不能做的事

       中断处理函数必须**极短**。以下都会导致系统卡死或随机崩溃：
         - printf / malloc / 文件操作（慢，而且大多不可重入）
         - 长时间的循环或延时
         - 访问可能被主循环同时访问、又没有做保护的大结构体
       标准做法：ISR 里只「置一个 volatile 标志」或「往队列里丢一个字节」，
                真正的处理放到主循环（或 RTOS 任务）里做。
       这个模式叫「上半部 / 下半部」（top half / bottom half），
       是 RTOS 那一阶段的核心概念之一。
 * ========================================================================== */
