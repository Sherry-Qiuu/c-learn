/* ============================================================================
 * ex10_mmio_volatile.c —— 内存映射 I/O + volatile：写 STM32 代码前的最后一次预演
 * ----------------------------------------------------------------------------
 * 这个练习把 ex07（struct）、ex08（位运算）、ex09（函数指针）三样东西合起来用，
 * 做出一个「在 PC 上跑的假 GPIO 外设」。
 *
 * 你在 PC 上学到的这套写法，一字不改就能搬到 STM32 上 —— 唯一的区别是
 * 这里是 &g_gpioa，STM32 上是 (GPIO_TypeDef *)0x40020000。
 *
 *   make run/ex10
 * ========================================================================== */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

/* ---------------------------------------------------------------------------
 * 第 1 步：用 struct 描述硬件寄存器组
 *
 * 真实的 STM32F103 里，这一段的原文几乎一模一样（在 stm32f103xb.h 里）：
 *
 *   typedef struct {
 *     __IO uint32_t CRL;
 *     __IO uint32_t CRH;
 *     __IO uint32_t IDR;
 *     ...
 *   } GPIO_TypeDef;
 *
 * __IO 就是 volatile 的宏。为什么每个字段都必须加 volatile？
 * 因为寄存器的值会被「硬件」改变，编译器完全不知情，
 * 不加 volatile 它就会把你的读操作优化掉、把写操作合并掉。
 * ------------------------------------------------------------------------- */
typedef struct {
    volatile uint32_t MODER;   /* 每引脚 2 位。00=输入 01=输出 10=复用 11=模拟 */
    volatile uint32_t OTYPER;  /* 每引脚 1 位。0=推挽 1=开漏 */
    volatile uint32_t ODR;     /* 每引脚 1 位。输出数据寄存器（可以读回） */
    volatile uint32_t IDR;     /* 每引脚 1 位。输入数据寄存器（只读，硬件写） */
    volatile uint32_t BSRR;    /* 低 16 位写 1 = 置位，高 16 位写 1 = 清位（原子操作） */
} GPIO_TypeDef;

/* ---------------------------------------------------------------------------
 * 第 2 步：给「寄存器组」一个地址
 *
 * STM32 上你会这样写：
 *   #define GPIOA_BASE 0x40020000UL
 *   #define GPIOA ((GPIO_TypeDef *) GPIOA_BASE)
 *
 * PC 上没有那一段地址空间，所以我们用一个静态结构体来冒充。
 * 效果完全等价：拿到一个指针，通过它读写「寄存器」。
 * ------------------------------------------------------------------------- */
static GPIO_TypeDef g_gpioa;   /* 冒充 GPIOA 的寄存器组（静态存储，自动清零） */
static GPIO_TypeDef g_gpiob;   /* 冒充 GPIOB */

#define GPIOA (&g_gpioa)
#define GPIOB (&g_gpiob)

/* 引脚定义。真实项目里这种宏通常在 board.h 里，描述硬件连接 */
#define LED_PIN    5u
#define BUTTON_PIN 13u

/* 位宽常量 */
#define MODER_BITS_PER_PIN 2u
#define BSRR_SET_OFFSET    0u
#define BSRR_RESET_OFFSET  16u

/* ---------------------------------------------------------------------------
 * TODO 1：把某个引脚配置成「推挽输出」
 *   MODER 里每引脚占 2 位，值 01 表示通用输出。
 *   OTYPER 里每引脚占 1 位，0 表示推挽。
 *
 *   提示：复用 ex08 里写的字段读写思路。
 *         引脚 n 对应的 MODER 字段在 shift = n * 2，宽度 2。
 * ------------------------------------------------------------------------- */
void gpio_config_output(GPIO_TypeDef *port, unsigned pin)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 2：把某个引脚配置成「浮空输入」
 *   MODER 值 00 表示输入。
 * ------------------------------------------------------------------------- */
void gpio_config_input(GPIO_TypeDef *port, unsigned pin)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 3：输出高电平（点亮 LED）
 *   直接操作 ODR 的第 pin 位。
 * ------------------------------------------------------------------------- */
void gpio_set(GPIO_TypeDef *port, unsigned pin)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 4：输出低电平（熄灭 LED）
 * ------------------------------------------------------------------------- */
void gpio_clear(GPIO_TypeDef *port, unsigned pin)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 5：翻转输出电平
 * ------------------------------------------------------------------------- */
void gpio_toggle(GPIO_TypeDef *port, unsigned pin)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 6：读输入电平，返回 0 或 1（读 IDR）
 * ------------------------------------------------------------------------- */
int gpio_read(GPIO_TypeDef *port, unsigned pin)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ---------------------------------------------------------------------------
 * TODO 7：用 BSRR 寄存器实现「原子」的置位 / 清位
 *
 *   on != 0 时，往 BSRR 的低 16 位对应位写 1（置位）
 *   on == 0 时，往 BSRR 的高 16 位对应位写 1（清位）
 *
 *   为什么 BSRR 这么重要？
 *     用 ODR 改一位要「读-改-写」三条指令，中间被中断打断就可能丢状态。
 *     BSRR 是「写 1 生效、写 0 无效果」的单次写操作，硬件层面保证原子性。
 *     在中断和主循环都可能操作同一个引脚时，BSRR 是唯一安全的选择。
 *
 *   注意：BSRR 是只写的，写完硬件自动清零，读它永远是 0。
 * ------------------------------------------------------------------------- */
void gpio_write_fast(GPIO_TypeDef *port, unsigned pin, int on)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 8：模拟一个「按键消抖后的边沿检测」
 *
 *   连续两次读到 1 才算按下（简化的消抖），上一次是 0、这次是 1 才返回 1（上升沿）。
 *   用一个 static 变量记住上一次的状态。
 *
 *   真实按键消抖要配合定时器做几十毫秒的延时，这里只是演示「记住历史状态」的思路。
 * ------------------------------------------------------------------------- */
int button_rising_edge(GPIO_TypeDef *port, unsigned pin)
{
    /* 在这里写你的代码 */
    return 0;
}

/* ===========================================================================
 * 第 3 步：写一个真正的「驱动」——结构体 + 函数指针，这是 C 版的类
 * =========================================================================== */

typedef void (*uart_rx_cb_t)(uint8_t byte, void *user_ctx);

typedef struct {
    const char   *name;
    uart_rx_cb_t  rx_callback;   /* 收到数据时的回调，NULL 表示不关心 */
    void         *user_ctx;      /* 回调需要的用户上下文 */
    uint32_t      rx_count;
    uint32_t      baudrate;
} UartDriver;

/* ---------------------------------------------------------------------------
 * TODO 9：初始化驱动
 * ------------------------------------------------------------------------- */
void uart_init(UartDriver *drv, const char *name, uint32_t baudrate)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 10：注册回调（把函数指针存进驱动结构体）
 * ------------------------------------------------------------------------- */
void uart_on_rx(UartDriver *drv, uart_rx_cb_t cb, void *user_ctx)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 11：模拟「硬件收到一个字节」，需要触发回调
 *   要求：累加 rx_count；如果注册了回调就调用它（把 drv->user_ctx 传进去）。
 *
 *   这个模式你在 STM32 的 HAL 库里会反复看到，比如：
 *     HAL_UART_RegisterCallback(&huart1, HAL_UART_RX_COMPLETE_CB_ID, MyCallback);
 * ------------------------------------------------------------------------- */
void uart_feed_byte(UartDriver *drv, uint8_t byte)
{
    /* 在这里写你的代码 */
}

/* --- 任务 11 用的回调 --- */
static int g_log_count = 0;

static void on_uart_rx(uint8_t byte, void *user_ctx)
{
    const char *tag = (const char *)user_ctx;
    printf("    [%s] 收到字节 0x%02X ('%c')\n",
           tag, byte, (byte >= 32 && byte < 127) ? (char)byte : '.');
    g_log_count++;
}

int main(void)
{
    printf("========== 任务 1-2：配置引脚模式 ==========\n");
    {
        gpio_config_output(GPIOA, LED_PIN);
        {
            uint32_t shift = LED_PIN * MODER_BITS_PER_PIN;
            uint32_t mode  = (GPIOA->MODER >> shift) & 0x3u;
            assert(mode == 1u);                       /* 01 = 通用输出 */
        }
        printf("  PA%u MODER = 0x%08X（第 %u~%u 位 = 01 输出）\n",
               LED_PIN, GPIOA->MODER,
               LED_PIN * MODER_BITS_PER_PIN, LED_PIN * MODER_BITS_PER_PIN + 1);

        gpio_config_input(GPIOA, BUTTON_PIN);
        {
            uint32_t shift = BUTTON_PIN * MODER_BITS_PER_PIN;
            uint32_t mode  = (GPIOA->MODER >> shift) & 0x3u;
            assert(mode == 0u);                       /* 00 = 输入 */
        }
        printf("  PA%u MODER 配置成输入  OK\n", BUTTON_PIN);

        /* 关键：配置引脚 5 不能影响引脚 13 */
        printf("  两个引脚的配置互不干扰（因为用的是字段读写而不是整寄存器赋值）\n");
    }

    printf("\n========== 任务 3-5：输出控制 ==========\n");
    {
        gpio_set(GPIOA, LED_PIN);
        assert((GPIOA->ODR & (1u << LED_PIN)) != 0u);
        printf("  set    -> ODR = 0x%08X（LED 亮）\n", GPIOA->ODR);

        gpio_clear(GPIOA, LED_PIN);
        assert((GPIOA->ODR & (1u << LED_PIN)) == 0u);
        printf("  clear  -> ODR = 0x%08X（LED 灭）\n", GPIOA->ODR);

        gpio_toggle(GPIOA, LED_PIN);
        assert((GPIOA->ODR & (1u << LED_PIN)) != 0u);
        printf("  toggle -> ODR = 0x%08X（LED 亮）\n", GPIOA->ODR);

        /* 别的引脚一个都不能动 */
        assert((GPIOA->ODR & ~(1u << LED_PIN)) == 0u);
        printf("  操作一个引脚不影响其他引脚  OK\n");

        /* 连续翻转 10 次，模拟 LED 闪烁 */
        gpio_clear(GPIOA, LED_PIN);          /* 从「灭」开始 */
        printf("\n  模拟闪烁 10 次：");
        for (int i = 0; i < 10; i++) {
            gpio_toggle(GPIOA, LED_PIN);
            printf("%d", ((GPIOA->ODR >> LED_PIN) & 1u));
        }
        printf("\n");
        assert(((GPIOA->ODR >> LED_PIN) & 1u) == 0u);   /* 偶数次翻转回到原值 */
    }

    printf("\n========== 任务 6：读输入 ==========\n");
    {
        /* 假装外部硬件把 PA13 拉高了 */
        GPIOA->IDR |= (1u << BUTTON_PIN);
        assert(gpio_read(GPIOA, BUTTON_PIN) == 1);
        printf("  IDR 置位后读到 1  OK\n");

        GPIOA->IDR &= ~(1u << BUTTON_PIN);
        assert(gpio_read(GPIOA, BUTTON_PIN) == 0);
        printf("  IDR 清零后读到 0  OK\n");
    }

    printf("\n========== 任务 7：BSRR 原子操作 ==========\n");
    {
        GPIOA->ODR  = 0u;
        GPIOA->BSRR = 0u;

        gpio_write_fast(GPIOA, LED_PIN, 1);
        /* 真实硬件上 BSRR 是「只写」的，写完自动清零，读回永远是 0。
         * 我们这里是普通 SRAM，硬件不会替我们清，所以能读到写入的值。
         * 记住这个差别：STM32 上读 BSRR 得到的永远是 0，别指望用它回读状态。 */
        assert(GPIOA->BSRR == (1u << LED_PIN));
        printf("  写 BSRR = 0x%08X（模拟环境能读回；真实硬件读回永远是 0）\n",
               GPIOA->BSRR);
        GPIOA->BSRR = 0u;                    /* 手动模拟「硬件自动清零」 */

        gpio_set(GPIOA, LED_PIN);
        assert((GPIOA->ODR & (1u << LED_PIN)) != 0u);
        gpio_write_fast(GPIOA, LED_PIN, 0);
        assert(GPIOA->BSRR == (1u << (BSRR_RESET_OFFSET + LED_PIN)));
        GPIOA->BSRR = 0u;

        gpio_clear(GPIOA, LED_PIN);
        assert((GPIOA->ODR & (1u << LED_PIN)) == 0u);
        printf("  BSRR 置位 / 清位两条路径都通了  OK\n");

        printf("\n  对比一下两种写法的机器码差别：\n");
        printf("    ODR 方式：LDR r0,[ODR]; ORR r0,#(1<<5); STR r0,[ODR]   —— 3 条，中间可被打断\n");
        printf("    BSRR 方式：MOV r0,#(1<<5); STR r0,[BSRR]                —— 2 条，单次写，原子\n");
        printf("  但 BSRR 只能置位/清位，改不了 MODER 这种多位字段，\n");
        printf("  所以两种写法各管一段：配置用字段读写，运行期开关用 BSRR。\n");
    }

    printf("\n========== 任务 8：边沿检测 ==========\n");
    {
        int edges = 0;

        /* 初始：IDR 为 0 */
        assert(button_rising_edge(GPIOA, BUTTON_PIN) == 0);   /* 0 -> 0，无边沿 */

        GPIOA->IDR |= (1u << BUTTON_PIN);
        assert(button_rising_edge(GPIOA, BUTTON_PIN) == 1);   /* 0 -> 1，上升沿 */
        if (1) edges++;

        assert(button_rising_edge(GPIOA, BUTTON_PIN) == 0);   /* 1 -> 1，无边沿 */

        GPIOA->IDR &= ~(1u << BUTTON_PIN);
        assert(button_rising_edge(GPIOA, BUTTON_PIN) == 0);   /* 1 -> 0，是下降沿不是上升沿 */

        GPIOA->IDR |= (1u << BUTTON_PIN);
        assert(button_rising_edge(GPIOA, BUTTON_PIN) == 1);
        edges++;

        assert(edges == 2);
        printf("  检测到 %d 次上升沿  OK\n", edges);
        printf("  这就是「按键单击」判定的核心：只对 0->1 的跳变响应\n");
    }

    printf("\n========== 任务 9-11：驱动 + 回调（C 版的类）==========\n");
    {
        UartDriver uart1;

        uart_init(&uart1, "UART1", 115200);
        assert(uart1.baudrate == 115200u);
        assert(uart1.rx_count == 0);
        assert(uart1.rx_callback == NULL);
        printf("  uart_init  OK（name=%s, baud=%u）\n", uart1.name, uart1.baudrate);

        /* 没注册回调时喂数据：不能崩，只累加计数 */
        uart_feed_byte(&uart1, 'X');
        assert(uart1.rx_count == 1);
        printf("  未注册回调时喂数据不崩，计数 = %u  OK\n", uart1.rx_count);

        /* 注册回调 */
        uart_on_rx(&uart1, on_uart_rx, (void *)"UART1");
        uart_feed_byte(&uart1, 'H');
        uart_feed_byte(&uart1, 'i');
        uart_feed_byte(&uart1, 0x0A);

        assert(uart1.rx_count == 4);
        assert(g_log_count == 3);
        printf("  注册回调后喂 3 字节，计数 = %u，回调触发 %d 次  OK\n",
               uart1.rx_count, g_log_count);

        printf("\n  这就是「结构体 + 函数指针」组合出的 C 版类：\n");
        printf("    数据成员：name / baudrate / rx_count      -> struct 的字段\n");
        printf("    方法：    uart_init / uart_on_rx / ...    -> 独立函数，第一个参数是 this\n");
        printf("    虚函数：  rx_callback                     -> 函数指针成员\n");
        printf("  C++ 的 vtable 干的就是这件事，只是编译器替你生成了。\n");
    }

    printf("\n========== 任务 12：volatile 到底防住了什么 ==========\n");
    {
        printf("  下面这段代码，去掉 volatile 会在 -O2 下被优化成死循环：\n\n");
        printf("    /* 假想的中断处理函数会把它改成 1 */\n");
        printf("    static volatile int g_irq_flag = 0;\n\n");
        printf("    void wait_irq(void) {\n");
        printf("        while (g_irq_flag == 0) { }   // 编译器不会把这次读缓存到寄存器\n");
        printf("    }\n\n");
        printf("  如果没有 volatile，编译器会认为「循环体里没人改 g_irq_flag」，\n");
        printf("  于是只读一次，之后永远用寄存器里的旧值 —— 循环永远出不去。\n");
        printf("  而中断是在「编译器视野之外」改这个变量的，它不知道。\n\n");
        printf("  自己验证一下（把上面的代码写成一个 .c 文件）：\n");
        printf("    gcc -O0 -S test.c -o test_o0.s     然后看循环里有没有 ldr\n");
        printf("    gcc -O2 -S test.c -o test_o2.s     对比一下，-O2 里 ldr 没了\n\n");
        printf("  三个必须加 volatile 的场景：\n");
        printf("    1. 外设寄存器（GPIOA->MODER 这类）\n");
        printf("    2. 中断和主循环共享的全局变量\n");
        printf("    3. RTOS 任务之间共享的变量（但这时候通常还要加临界区保护）\n\n");
        printf("  两个必须记住的限制：\n");
        printf("    volatile 不保证原子性 —— reg |= (1U<<5) 仍然是读改写三步\n");
        printf("    volatile 不保证顺序 —— 需要内存屏障时要用 __DMB() 之类\n");
    }

    printf("\n========== 任务 13：从 PC 到 STM32，只差一个地址 ==========\n");
    {
        printf("  今天你写的：        #define GPIOA (&g_gpioa)\n");
        printf("  STM32 上要写的：    #define GPIOA ((GPIO_TypeDef *)0x40020000UL)\n\n");
        printf("  除此之外，一个字都不用改：\n");
        printf("    - struct 描述寄存器布局   （ex07）\n");
        printf("    - 位运算做字段读写         （ex08）\n");
        printf("    - volatile 阻止错误优化    （任务 12）\n");
        printf("    - 函数指针做回调           （ex09）\n");
        printf("  这就是为什么你不需要「先学 STM32 再学 C」。\n");
        printf("  反过来才对：C 学透了，STM32 只是换一套头文件和地址。\n");
    }

    printf("\n>>> ex10 全部通过。下一站：ex11_file_io.c\n");
    return 0;
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

void gpio_config_output(GPIO_TypeDef *port, unsigned pin)
{
    unsigned shift = pin * MODER_BITS_PER_PIN;
    uint32_t mask  = 0x3u << shift;
    port->MODER &= ~mask;                  // 先清
    port->MODER |=  (0x1u << shift);       // 再写 01
    port->OTYPER &= ~(1u << pin);          // 0 = 推挽
}

void gpio_config_input(GPIO_TypeDef *port, unsigned pin)
{
    unsigned shift = pin * MODER_BITS_PER_PIN;
    uint32_t mask  = 0x3u << shift;
    port->MODER &= ~mask;                  // 00 = 输入，清零就够了
}

void gpio_set(GPIO_TypeDef *port, unsigned pin)
{
    port->ODR |= (1u << pin);
}

void gpio_clear(GPIO_TypeDef *port, unsigned pin)
{
    port->ODR &= ~(1u << pin);
}

void gpio_toggle(GPIO_TypeDef *port, unsigned pin)
{
    port->ODR ^= (1u << pin);
}

int gpio_read(GPIO_TypeDef *port, unsigned pin)
{
    return (int)((port->IDR >> pin) & 1u);
}

void gpio_write_fast(GPIO_TypeDef *port, unsigned pin, int on)
{
    if (on) {
        port->BSRR = (1u << (BSRR_SET_OFFSET + pin));
    } else {
        port->BSRR = (1u << (BSRR_RESET_OFFSET + pin));
    }
}

int button_rising_edge(GPIO_TypeDef *port, unsigned pin)
{
    static int last = 0;                   // static: 只初始化一次，函数返回后仍然存在
    int now = (int)((port->IDR >> pin) & 1u);
    int result = (last == 0 && now == 1) ? 1 : 0;
    last = now;
    return result;
}

void uart_init(UartDriver *drv, const char *name, uint32_t baudrate)
{
    if (drv == NULL) return;
    drv->name        = name;
    drv->baudrate    = baudrate;
    drv->rx_callback = NULL;
    drv->user_ctx    = NULL;
    drv->rx_count    = 0;
}

void uart_on_rx(UartDriver *drv, uart_rx_cb_t cb, void *user_ctx)
{
    if (drv == NULL) return;
    drv->rx_callback = cb;
    drv->user_ctx    = user_ctx;
}

void uart_feed_byte(UartDriver *drv, uint8_t byte)
{
    if (drv == NULL) return;
    drv->rx_count++;
    if (drv->rx_callback != NULL) {
        drv->rx_callback(byte, drv->user_ctx);
    }
}

  --- 几个必须理解的细节 ---

  1. 为什么配置引脚必须「先清再写」而不是直接赋值？

       port->MODER |= (0x1u << shift);
       如果该字段原来是 0b10（复用模式），这一句会得到 0b11 —— 错。
       必须先 port->MODER &= ~(0x3u << shift) 清成 00，再写 01。
       这就是 ex08 里反复强调的那条规则在真实硬件上的后果。
       而且这个 bug 是「有时错有时对」：上电时 MODER 全是 0，直接写也能过；
       但如果你在运行时重配置一个已经用过的引脚，就会出错。

  2. static 局部变量和全局变量有什么区别？

       static int last = 0; 放在函数里：
         - 存储期是「整个程序运行期」（在 .data 段或 .bss 段，不在栈上）
         - 作用域是「函数内部」（别的函数看不到它）
       这正是嵌入式里做「状态记忆」最常用的写法。
       但要注意：它是全局共享的，所以这个函数不可重入、
       也不能同时用于两个按键 —— 要支持多按键就得把状态交给调用方管理。

  3. 为什么 BSRR 能保证原子性？

       ODR 的写操作需要先把整个 32 位读出来、改一位、再写回去。
       在「读」和「写」之间如果来中断，中断里也改了 ODR，你的写回就会覆盖它。
       BSRR 的语义是「写 1 的位执行动作，写 0 的位忽略」，
       所以只需要一次写操作，硬件层面不可分割，没有竞态窗口。
       这是硬件给软件提供的原子原语 —— STM32 的 BSRR、AVR 的 PIN 寄存器
       都是这个设计。

  4. 为什么 uart_feed_byte 里要先判 rx_callback != NULL ？

       调用一个 NULL 函数指针是 UB，在 STM32 上直接 HardFault。
       未注册回调是完全正常的使用场景，所以必须判空。
       嵌入式代码里「所有回调调用前都判空」应该成为肌肉记忆。

  5. (void *)"UART1" 这种把字符串字面量当 user_ctx 传的做法安全吗？

       这里安全，因为字符串字面量的生命周期是「整个程序」。
       但如果传的是局部变量的地址，回调异步触发时那个变量早就没了 ——
       这是嵌入式回调最经典的 bug（悬垂 user_ctx）。
       正确做法：user_ctx 指向全局变量、静态变量，或者堆上稳定存在的对象。

   === 到 STM32 上你要补的三件事 ===
     1. 开时钟：__HAL_RCC_GPIOA_CLK_ENABLE();  不使能时钟，寄存器写不进去
     2. 配复用：复用功能引脚要同时设置 MODER=10 和 AFR 寄存器
     3. 看手册：每个寄存器的位定义以 Reference Manual 为准，别凭记忆写
 * ========================================================================== */
