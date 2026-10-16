/* ============================================================================
 * gpio.h —— 模拟 GPIO 驱动的接口
 * ----------------------------------------------------------------------------
 * 这个头文件的写法，和 ST（意法半导体）官方的 stm32f1xx_hal_gpio.h
 * 结构几乎一模一样。你把它读懂，以后看真实 STM32 代码时会有「我认得这个」
 * 的感觉。
 *
 * ⚠️ 这个文件不需要你修改。
 * ========================================================================== */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/* ---------------------------------------------------------------------------
 * 寄存器组布局。真实 STM32 里这是芯片手册定义的，
 * 而且**必须**每个字段都是 volatile —— 硬件会改它们，编译器不知道。
 * ------------------------------------------------------------------------- */
typedef struct {
    volatile uint32_t MODER;   /* 每引脚 2 位：模式 */
    volatile uint32_t OTYPER;  /* 每引脚 1 位：推挽 / 开漏 */
    volatile uint32_t ODR;     /* 每引脚 1 位：输出数据 */
    volatile uint32_t IDR;     /* 每引脚 1 位：输入数据（只读，硬件写） */
    volatile uint32_t BSRR;    /* 低 16 位写 1 = 置位；高 16 位写 1 = 清位（只写） */
} GPIO_TypeDef;

/* 编译期检查：确认这个结构体的布局符合我们预期的 5 × 4 = 20 字节，
 * 没有任何对齐填充。手册上寄存器是紧挨着的，一旦编译器插了填充就全错位了。
 * embedded 项目里这种断言非常重要。 */
_Static_assert(sizeof(GPIO_TypeDef) == 20, "GPIO_TypeDef 布局不符合预期（有填充？）");

/* ---------------------------------------------------------------------------
 * 端口句柄。
 *
 * 这里用 extern 声明「这两个指针在别的文件里定义」——
 * gpio.c 里会给出真正的定义，指向模拟的寄存器组。
 *
 * 真实 STM32 里对应的写法是：
 *   #define GPIOA ((GPIO_TypeDef *) GPIOA_BASE)
 *   #define GPIOB ((GPIO_TypeDef *) GPIOB_BASE)
 * 用宏是因为那是编译期常量地址，不占 RAM。
 * 我们这里用 extern 指针是为了能在 PC 上指向一块真实内存。
 * ------------------------------------------------------------------------- */
extern GPIO_TypeDef *const GPIOA;
extern GPIO_TypeDef *const GPIOB;

/* ---------------------------------------------------------------------------
 * 引脚模式。值直接对应 MODER 里的 2 位编码 —— 让枚举值等于寄存器编码，
 * 可以少一次映射转换。这是嵌入式里很常见的做法。
 * ------------------------------------------------------------------------- */
typedef enum {
    GPIO_MODE_INPUT  = 0u,
    GPIO_MODE_OUTPUT = 1u,
    GPIO_MODE_AF     = 2u,
    GPIO_MODE_ANALOG = 3u
} gpio_mode_t;

typedef enum {
    GPIO_OK          =  0,
    GPIO_ERR_INVALID = -1
} gpio_status_t;

/* ---------------------------------------------------------------------------
 * 板级连接（真实项目里这些通常在 board.h 里）
 * ------------------------------------------------------------------------- */
#define LED_PIN       5u
#define BUTTON_PIN   13u
#define BUTTON_IRQ    6u      /* 按键接在 EXTI6 上，对应 EXTI9_5_IRQn 类中断 */

/* ---------------------------------------------------------------------------
 * 驱动接口
 * ------------------------------------------------------------------------- */

/* 配置引脚模式。pin 超过 15 返回 GPIO_ERR_INVALID */
gpio_status_t gpio_config(GPIO_TypeDef *port, unsigned pin, gpio_mode_t mode);

/* 输出高低电平（走 ODR，读-改-写） */
gpio_status_t gpio_set(GPIO_TypeDef *port, unsigned pin);
gpio_status_t gpio_clear(GPIO_TypeDef *port, unsigned pin);
gpio_status_t gpio_toggle(GPIO_TypeDef *port, unsigned pin);

/* 原子地置位/清位（走 BSRR，单次写） */
gpio_status_t gpio_write(GPIO_TypeDef *port, unsigned pin, int on);

/* 读输入电平，返回 0 / 1；参数非法返回 -1 */
int gpio_read(GPIO_TypeDef *port, unsigned pin);

/* ---------------------------------------------------------------------------
 * 仅用于 PC 上测试的辅助函数（真实硬件上当然没有这个）
 * ------------------------------------------------------------------------- */
/* 模拟「外部把引脚拉高/拉低」—— 直接改 IDR，就像真按键按下去那样 */
void gpio_sim_drive_input(GPIO_TypeDef *port, unsigned pin, int level);

/* 模拟「上电复位」：把所有寄存器清零 */
void gpio_sim_reset(void);

/* ---------------------------------------------------------------------------
 * 模拟「硬件根据 BSRR 更新 ODR，然后自动把 BSRR 清零」这个动作。
 *
 * 为什么需要这个函数？
 *   BSRR 是**只写**寄存器。软件往里写 1，是硬件负责把 ODR 对应位改掉，
 *   改完硬件自己把 BSRR 清成 0。整个过程软件不参与，也读不到。
 *   在 PC 上没有这颗硬件，所以要有人扮演它 —— 就是这个函数。
 *
 * 这也解释了一个新手常问的问题：
 *   「我写了 BSRR，为什么读 ODR 没变？」
 *   因为中间那一步（硬件）在你的模拟环境里被跳过了。
 *   真实芯片上，写完 BSRR 的下一条指令读 ODR 就能看到变化。
 * ------------------------------------------------------------------------- */
void gpio_sim_apply_bsrr(GPIO_TypeDef *port);

/* 打印某个端口的寄存器状态（调试用） */
void gpio_sim_dump(GPIO_TypeDef *port, const char *label);

#endif /* GPIO_H */
