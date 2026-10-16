/* ============================================================================
 * main.c —— 应用层：按键中断翻转 LED
 * ----------------------------------------------------------------------------
 * 这是一个「麻雀虽小五脏俱全」的嵌入式程序骨架：
 *
 *     上电初始化  ->  配置外设  ->  注册中断  ->  while(1) 主循环
 *
 * 真实 STM32 工程的 main.c 结构就是这个样子。
 * 唯一的区别是：真实硬件上 irq_trigger 由硬件自动调用，
 * 我们这里是手动模拟；真实硬件上的 while(1) 永远不会退出，
 * 我们这里跑固定轮数以便测试。
 *
 *   make rebuild && make run
 * ========================================================================== */

#include "gpio.h"
#include "irq.h"

#include <stdio.h>
#include <assert.h>

/* ---------------------------------------------------------------------------
 * 中断和主循环共享的变量 —— 必须加 volatile。
 *
 * 为什么？因为编译器在优化 main 时，看不见「中断会改这个变量」这件事
 * （中断是硬件触发的，编译器视野之外）。
 * 没有 volatile，编译器可能：
 *   - 把变量缓存在寄存器里，主循环永远读到旧值
 *   - 认为循环体什么都不改，直接把整个循环删掉
 *
 * 回忆 ex10 任务 12 讲的内容。
 * ------------------------------------------------------------------------- */
static volatile unsigned g_press_count = 0;

/* ---------------------------------------------------------------------------
 * 观察 LED 当前状态。
 *
 * 为什么不直接用 gpio_read？因为 gpio_read 读的是 **IDR（输入寄存器）**，
 * 而 LED 是**输出**引脚 —— 要看它的状态应该读 **ODR**。
 *
 * 补充（真实 STM32 的细节）：
 *   STM32 上输出引脚的 IDR 也会跟随引脚的实际电平，
 *   所以「读 IDR 看输出状态」在真机上通常是能用的。
 *   但那样做有两个隐患：
 *     1. 开漏输出、引脚被外部拉低时，IDR 和 ODR 会不一致
 *     2. 你其实想知道的是「我输出的是什么」，那就是 ODR
 *   所以我们这套模拟特意把两条路径分清楚：
 *     IDR = 外部世界给的电平（只由 gpio_sim_drive_input 改变）
 *     ODR = 我们想要输出的电平
 * ------------------------------------------------------------------------- */
static int led_is_on(void)
{
    return (int)((GPIOA->ODR >> LED_PIN) & 1u);
}

/* ---------------------------------------------------------------------------
 * 按键中断服务函数。
 *
 * 注意它有多短：只做两件事 —— 计数、翻转 LED。
 * 没有 printf、没有 malloc、没有循环。
 *
 * 这不是偷懒，是硬规矩：ISR 里凡是耗时的、可能重入的、
 * 可能阻塞的操作，都会导致系统随机崩溃或丢中断。
 * 复杂逻辑要扔到主循环里，靠 g_press_count 这样的标志位传递。
 * ------------------------------------------------------------------------- */
static void on_button_pressed(void)
{
    g_press_count++;
    gpio_toggle(GPIOA, LED_PIN);
}

/* 第二个中断：模拟一个 1ms 定时器中断 */
static volatile unsigned g_tick = 0;

static void on_timer_tick(void)
{
    g_tick++;
}

/* ===========================================================================
 * 主程序
 * ========================================================================== */

int main(void)
{
    printf("=====================================================\n");
    printf("  GPIO + 中断 模拟程序\n");
    printf("  结构：reset -> config -> attach -> while(1)\n");
    printf("=====================================================\n");

    /* -----------------------------------------------------------------------
     * 第 0 步：上电复位（真实硬件上由硬件自动完成）
     * --------------------------------------------------------------------- */
    gpio_sim_reset();
    printf("\n[0] 上电复位完成\n");

    /* -----------------------------------------------------------------------
     * 第 1 步：配置外设
     * --------------------------------------------------------------------- */
    assert(gpio_config(GPIOA, LED_PIN,    GPIO_MODE_OUTPUT) == GPIO_OK);
    assert(gpio_config(GPIOA, BUTTON_PIN, GPIO_MODE_INPUT)  == GPIO_OK);
    printf("[1] 配置 PA%u = 输出（LED），PA%u = 输入（按键）\n",
           LED_PIN, BUTTON_PIN);

    /* 验证 MODER 的编码 */
    {
        unsigned led_shift = LED_PIN * 2u;
        unsigned btn_shift = BUTTON_PIN * 2u;
        assert(((GPIOA->MODER >> led_shift) & 0x3u) == (unsigned)GPIO_MODE_OUTPUT);
        assert(((GPIOA->MODER >> btn_shift) & 0x3u) == (unsigned)GPIO_MODE_INPUT);
        printf("    MODER 校验通过（两个引脚互不干扰）\n");
    }

    /* -----------------------------------------------------------------------
     * 第 2 步：注册中断处理函数
     * --------------------------------------------------------------------- */
    irq_init();
    assert(irq_attach(BUTTON_IRQ, on_button_pressed) == IRQ_OK);
    assert(irq_attach(0,          on_timer_tick)     == IRQ_OK);

    assert(irq_attach(IRQ_MAX,  on_button_pressed) == IRQ_ERR_INVALID);
    assert(irq_attach(BUTTON_IRQ, NULL)            == IRQ_ERR_INVALID);
    printf("[2] 注册中断：%s <- on_button_pressed，IRQ0 <- on_timer_tick\n",
           irq_name(BUTTON_IRQ));
    printf("    越界中断号 / NULL 处理函数 都被拒绝  OK\n");

    /* -----------------------------------------------------------------------
     * 第 3 步：初始状态检查
     * --------------------------------------------------------------------- */
    assert(led_is_on() == 0);
    assert(gpio_read(GPIOA, BUTTON_PIN) == 0);
    assert(g_press_count == 0);
    printf("[3] 初始状态：LED 灭，按键未按下\n");
    gpio_sim_dump(GPIOA, "GPIOA");

    /* -----------------------------------------------------------------------
     * 第 4 步：模拟按键按下 5 次
     * --------------------------------------------------------------------- */
    printf("\n[4] 模拟按键 5 次（每次：外部拉高 -> 触发中断 -> 拉低）\n");
    for (int i = 0; i < 5; i++) {
        gpio_sim_drive_input(GPIOA, BUTTON_PIN, 1);   /* 按键按下，硬件把 IDR 拉高 */
        irq_trigger(BUTTON_IRQ);                      /* 硬件触发中断 */
        gpio_sim_drive_input(GPIOA, BUTTON_PIN, 0);   /* 松手 */

        printf("    第 %d 次：press_count=%u  LED=%d\n",
               i + 1, g_press_count, led_is_on());
    }

    assert(g_press_count == 5);
    assert(led_is_on() == 1);           /* 5 次翻转，奇数 -> 亮 */
    assert(irq_dispatch_count() == 5);
    assert(irq_unhandled_count() == 0);
    printf("    5 次翻转后 LED 应为「亮」，实际 = %d  OK\n",
           led_is_on());

    /* -----------------------------------------------------------------------
     * 第 5 步：模拟 while(1) 主循环
     * --------------------------------------------------------------------- */
    printf("\n[5] 模拟主循环（真实固件里这里是 while(1)，永不退出）\n");

    gpio_set(GPIOA, LED_PIN);          /* 主循环里也可以操作 LED（走 ODR） */
    assert(led_is_on() == 1);

    /* 用 BSRR 的原子路径关掉它。
     *
     * 注意这里有两步：先 gpio_write 写 BSRR，再 gpio_sim_apply_bsrr「扮演硬件」。
     * 在真实 STM32 上只有第一步 —— 硬件会自动把 ODR 改掉。
     * 很多新手会问「我写了 BSRR 怎么 ODR 没变」，答案就是这个。 */
    gpio_write(GPIOA, LED_PIN, 0);                 /* 只写了 BSRR */
    printf("    写 BSRR 之后、硬件生效之前：ODR bit%u = %d（还是 1）\n",
           LED_PIN, led_is_on());
    assert(led_is_on() == 1);                      /* 还没生效 */

    gpio_sim_apply_bsrr(GPIOA);                    /* 模拟硬件生效 */
    assert(led_is_on() == 0);
    assert(GPIOA->BSRR == 0u);                     /* 硬件已自动清零 */
    printf("    硬件生效之后：ODR bit%u = %d，BSRR 已自动清零  OK\n",
           LED_PIN, led_is_on());
    printf("    主循环里用 ODR / BSRR 两条路径都能控制 LED  OK\n");

    /* 模拟 10 个 1ms tick */
    for (int ms = 0; ms < 10; ms++) {
        irq_trigger(0);
    }
    assert(g_tick == 10);
    assert(irq_dispatch_count() == 15);
    printf("    模拟 10 个定时器 tick，g_tick = %u  OK\n", g_tick);

    /* 主循环检查中断留下的标志位 —— 这就是「上半部/下半部」模式 */
    if (g_press_count >= 5) {
        printf("    主循环读到 press_count = %u（volatile 保证读到的是最新值）\n",
               g_press_count);
    }

    /* -----------------------------------------------------------------------
     * 第 6 步：异常路径
     * --------------------------------------------------------------------- */
    printf("\n[6] 异常路径检查\n");

    /* 摘掉中断，再触发 -> 计入 unhandled，不能崩 */
    assert(irq_detach(BUTTON_IRQ) == IRQ_OK);
    unsigned before_unhandled = irq_unhandled_count();
    irq_trigger(BUTTON_IRQ);
    irq_trigger(BUTTON_IRQ);
    assert(irq_unhandled_count() == before_unhandled + 2);
    printf("    摘掉中断后触发 2 次 -> unhandled = %u  OK\n", irq_unhandled_count());

    /* 越界中断号 */
    irq_trigger(999);
    assert(irq_unhandled_count() == before_unhandled + 3);
    printf("    越界中断号 999 被安全忽略  OK\n");

    /* 非法引脚号（模拟 ex12 那个 unsigned 陷阱） */
    assert(gpio_config(GPIOA, 99, GPIO_MODE_OUTPUT) == GPIO_ERR_INVALID);
    assert(gpio_set(GPIOA, 99) == GPIO_ERR_INVALID);
    assert(gpio_read(GPIOA, 99) == -1);
    assert(gpio_read(NULL, 0) == -1);
    printf("    非法引脚号 / 空端口指针 都被拒绝  OK\n");

    /* 注意：如果把引脚号参数写成 int 并且传 -1 进来，
     *       1u << (unsigned)(-1) 会变成 UB（移位 4294967295 位）。
     *       这就是为什么接口签名用 unsigned。 */

    printf("\n=====================================================\n");
    gpio_sim_dump(GPIOA, "GPIOA");
    printf("  全部通过。\n");
    printf("  你完成了一个完整的驱动 + 中断 + 主循环骨架。\n");
    printf("  把它和真实 STM32 工程对照着看，你会发现结构完全一样。\n");
    printf("=====================================================\n");

    /* 真实固件在这里应该是 while (1) { ... } —— 永不返回。
     * 如果 main 真的返回了，启动文件会跳进一个死循环，
     * 这不是「程序结束」，而是「系统卡住了」。 */
    return 0;
}
