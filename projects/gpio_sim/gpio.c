/* ============================================================================
 * gpio.c —— 模拟 GPIO 驱动实现
 * ----------------------------------------------------------------------------
 * 这就是一个「驱动」的样子：把硬件的位操作细节全部封装起来，
 * 对外只暴露「设置模式 / 置位 / 清位 / 读电平」这种人类语言。
 *
 * 真实 STM32 上的 HAL_GPIO_Init / HAL_GPIO_WritePin / HAL_GPIO_TogglePin
 * 干的就是同一件事，只不过它们要处理更多模式、更多复用功能。
 *
 *   make rebuild && make run
 * ========================================================================== */

#include "gpio.h"

#include <stdio.h>

/* ===========================================================================
 * 第 1 部分：模拟的寄存器组（真实硬件上这一段会被芯片取代）
 * ========================================================================== */

#define PINS_PER_PORT 16u
#define MODER_BITS    2u

/* 冒充 GPIOA / GPIOB 的寄存器组。
 * static 表示只在本文件可见 —— 外部只能通过 GPIOA / GPIOB 这两个句柄访问。 */
static GPIO_TypeDef s_port_a;
static GPIO_TypeDef s_port_b;

/* 真正的定义在这里，gpio.h 里只是 extern 声明。
 * 这就是「声明与定义分离」在多文件项目里的样子。 */
GPIO_TypeDef *const GPIOA = &s_port_a;
GPIO_TypeDef *const GPIOB = &s_port_b;

/* ---------------------------------------------------------------------------
 * 私有辅助
 * ------------------------------------------------------------------------- */

/* 参数体检：端口非空、引脚号在 0..15 之间 */
static int pin_valid(const GPIO_TypeDef *port, unsigned pin)
{
    return (port != NULL) && (pin < PINS_PER_PORT);
}

/* ===========================================================================
 * 第 2 部分：驱动接口（这部分要你实现）
 * =========================================================================== */

gpio_status_t gpio_config(GPIO_TypeDef *port, unsigned pin, gpio_mode_t mode)
{
    /* TODO 1：
     *   1. pin_valid 检查，不合法返回 GPIO_ERR_INVALID
     *   2. MODER：引脚 pin 占 shift = pin * MODER_BITS 起的 2 位
     *      —— 必须先清干净 0b11，再写入 (uint32_t)mode
     *   3. OTYPER：输出模式时把对应位设成 0（推挽）。
     *      这里简单处理：所有模式都把 OTYPER 对应位清 0。
     *   4. 返回 GPIO_OK
     *
     *   提醒：mode 是枚举，直接 (uint32_t)mode 就是 0..3，
     *         已经和寄存器编码对齐了（看 gpio.h 的注释）。
     */
    /* 在这里写你的代码 */
    return GPIO_ERR_INVALID;
}

gpio_status_t gpio_set(GPIO_TypeDef *port, unsigned pin)
{
    /* TODO 2：ODR 的 pin 位置 1 */
    /* 在这里写你的代码 */
    return GPIO_ERR_INVALID;
}

gpio_status_t gpio_clear(GPIO_TypeDef *port, unsigned pin)
{
    /* TODO 3：ODR 的 pin 位清 0 */
    /* 在这里写你的代码 */
    return GPIO_ERR_INVALID;
}

gpio_status_t gpio_toggle(GPIO_TypeDef *port, unsigned pin)
{
    /* TODO 4：ODR 的 pin 位取反 */
    /* 在这里写你的代码 */
    return GPIO_ERR_INVALID;
}

gpio_status_t gpio_write(GPIO_TypeDef *port, unsigned pin, int on)
{
    /* TODO 5：用 BSRR —— on 非 0 时写低 16 位的 pin 位，否则写高 16 位的 pin 位。
     *   这就是「单次写、原子」的那条路径。 */
    /* 在这里写你的代码 */
    return GPIO_ERR_INVALID;
}

int gpio_read(GPIO_TypeDef *port, unsigned pin)
{
    /* TODO 6：读 IDR 的第 pin 位，返回 0 或 1；参数不合法返回 -1 */
    /* 在这里写你的代码 */
    return -1;
}

/* ===========================================================================
 * 第 3 部分：测试辅助（这部分是脚手架，直接给你，不用改）
 * ========================================================================== */

void gpio_sim_drive_input(GPIO_TypeDef *port, unsigned pin, int level)
{
    if (!pin_valid(port, pin)) return;

    if (level) {
        port->IDR |= (1u << pin);
    } else {
        port->IDR &= ~(1u << pin);
    }
    /* 注意：真实硬件上 IDR 是「只读」的，你写它没有任何效果，
     * 它的值完全由引脚外面的电平决定。
     * 这里为了能在 PC 上模拟，我们直接改它。
     * 这也是为什么 gpio_sim_* 这些函数不该出现在生产代码里。 */
}

void gpio_sim_reset(void)
{
    /* 逐个字段清零，而不是 memset —— 因为字段是 volatile，
     * 用 memset 需要强转掉 volatile 限定符，那是在骗编译器。
     * 嵌入式规范里一般要求避免这种写法。 */
    s_port_a.MODER  = 0u;
    s_port_a.OTYPER = 0u;
    s_port_a.ODR    = 0u;
    s_port_a.IDR    = 0u;
    s_port_a.BSRR   = 0u;

    s_port_b.MODER  = 0u;
    s_port_b.OTYPER = 0u;
    s_port_b.ODR    = 0u;
    s_port_b.IDR    = 0u;
    s_port_b.BSRR   = 0u;
}

void gpio_sim_apply_bsrr(GPIO_TypeDef *port)
{
    if (port == NULL) return;

    uint32_t v = port->BSRR;

    port->ODR |= (v & 0xFFFFu);                    /* 低 16 位写 1 -> 置位 */
    port->ODR &= ~((v >> 16) & 0xFFFFu);           /* 高 16 位写 1 -> 清位 */
    port->BSRR = 0u;                               /* 硬件自动清零 */
}

void gpio_sim_dump(GPIO_TypeDef *port, const char *label)
{
    if (port == NULL) return;
    printf("  %-6s MODER=%08X OTYPER=%08X ODR=%08X IDR=%08X\n",
           (label != NULL) ? label : "?",
           (unsigned)port->MODER,
           (unsigned)port->OTYPER,
           (unsigned)port->ODR,
           (unsigned)port->IDR);
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

gpio_status_t gpio_config(GPIO_TypeDef *port, unsigned pin, gpio_mode_t mode)
{
    if (!pin_valid(port, pin)) return GPIO_ERR_INVALID;

    unsigned shift = pin * MODER_BITS;
    uint32_t mask  = ((1u << MODER_BITS) - 1u) << shift;   // 0b11 << shift

    port->MODER &= ~mask;                       // 先清
    port->MODER |= ((uint32_t)mode << shift);   // 再写

    port->OTYPER &= ~(1u << pin);               // 0 = 推挽

    return GPIO_OK;
}

gpio_status_t gpio_set(GPIO_TypeDef *port, unsigned pin)
{
    if (!pin_valid(port, pin)) return GPIO_ERR_INVALID;
    port->ODR |= (1u << pin);
    return GPIO_OK;
}

gpio_status_t gpio_clear(GPIO_TypeDef *port, unsigned pin)
{
    if (!pin_valid(port, pin)) return GPIO_ERR_INVALID;
    port->ODR &= ~(1u << pin);
    return GPIO_OK;
}

gpio_status_t gpio_toggle(GPIO_TypeDef *port, unsigned pin)
{
    if (!pin_valid(port, pin)) return GPIO_ERR_INVALID;
    port->ODR ^= (1u << pin);
    return GPIO_OK;
}

gpio_status_t gpio_write(GPIO_TypeDef *port, unsigned pin, int on)
{
    if (!pin_valid(port, pin)) return GPIO_ERR_INVALID;

    if (on) {
        port->BSRR = (1u << pin);            // 低 16 位：置位
    } else {
        port->BSRR = (1u << (pin + 16u));    // 高 16 位：清位
    }
    return GPIO_OK;
}

int gpio_read(GPIO_TypeDef *port, unsigned pin)
{
    if (!pin_valid(port, pin)) return -1;
    return (int)((port->IDR >> pin) & 1u);
}

  --- 三个必须说清楚的点 ---

  1. 为什么 gpio_config 必须「先清后写」？

       如果写成 port->MODER |= ((uint32_t)mode << shift)：
         原本是 0b10（复用模式），要改成 0b01（输出模式）
         0b10 | 0b01 = 0b11 -> 变成「模拟模式」，完全错。
       上电时 MODER 全是 0，所以「只写不清」在第一次配置时看起来是对的 ——
       这个 bug 只在你重新配置一个已经用过的引脚时才出现。
       这就是嵌入式 bug 最难查的那一类。

  2. 为什么 GPIOA 定义成 GPIO_TypeDef *const 而不是 GPIO_TypeDef *？

       *const 后面：指针本身不能改（不能 GPIOA = &s_port_b），
        但它指向的寄存器可以被改（本来就是给硬件写的）。
       如果没有 const，别人可以一句 GPIOA = GPIOB 就把整个端口映射偷换了，
       而且这种 bug 极难定位（代码看着完全正常）。
       凡是「不该被重新赋值的全局指针」，一律加 const。

  3. 为什么 pin_valid 用 unsigned pin 而检查 pin < 16？

       如果参数写成 int pin，别人传 -1 进来，
       1u << pin 里的 pin 会被隐式转成 unsigned 变成 4294967295，
       这是 UB（移位位数超过类型宽度）。
       用 unsigned 就没这个问题 —— 负数在调用点就变成了一个巨大的正数，
       被 pin < 16 拦下。
       这就是 ex12 讲的「整数提升陷阱」在真实代码里的应用。

   === 到真实 STM32 上，这两件事必须做 ===
     1. 开时钟：RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        没使能时钟就写 GPIO 寄存器，写了没有任何反应 ——
        这是新手最常见的「代码看着对但灯不亮」的原因。
     2. 配复用：如果引脚要接 UART/SPI/PWM，
        除了 MODER 设成 0b10，还要配 AFR 寄存器选择具体功能。
 * ========================================================================== */
