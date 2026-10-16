/* ============================================================================
 * irq.h —— 中断分发器（模拟中断向量表）
 * ----------------------------------------------------------------------------
 * 真实 STM32 的「中断向量表」是启动文件里的一张函数指针数组，
 * 放在 Flash 最开头，每个中断号一个入口。
 * 硬件触发中断时，CPU 就是「按下标查出函数地址、跳过去执行」。
 *
 * 我们这个模块做的就是这件事的可测试版本：注册 / 注销 / 触发 / 分发。
 *
 * ⚠️ 这个文件不需要你修改。
 * ========================================================================== */

#ifndef IRQ_H
#define IRQ_H

#include <stddef.h>

#define IRQ_MAX 8

/* 中断服务函数（Interrupt Service Routine）的签名。
 * 和 ex09 的函数指针一模一样 —— 中断的本质就是函数指针。 */
typedef void (*isr_t)(void);

typedef enum {
    IRQ_OK          =  0,
    IRQ_ERR_INVALID = -1
} irq_status_t;

/* 初始化：清空整张向量表 */
void irq_init(void);

/* 把处理函数挂到某个中断号上。
 * num 越界或 handler 为 NULL -> IRQ_ERR_INVALID
 * 重复挂载同一个中断号 = 覆盖（真实 STM32 里也是这个语义） */
irq_status_t irq_attach(unsigned num, isr_t handler);

/* 摘掉某个中断号上的处理函数 */
irq_status_t irq_detach(unsigned num);

/* 模拟硬件触发一次中断。
 * 真实硬件上这个动作发生时，CPU 会立刻打断主循环跳到 ISR；
 * 我们这里用「直接同步调用」来模拟（够用了，能验证逻辑）。
 * 没挂处理函数的中断被触发时，什么都不发生，但要计数（真实系统里叫「未处理中断」）。 */
void irq_trigger(unsigned num);

/* 统计信息 */
unsigned irq_dispatch_count(void);   /* 成功分发到 ISR 的次数 */
unsigned irq_unhandled_count(void);  /* 触发了但没有 ISR 的次数 */

/* 中断号的助记名字，越界返回 "?"；用于打印 */
const char *irq_name(unsigned num);

#endif /* IRQ_H */
