# 项目 2 · GPIO + 中断 模拟（STM32 预演）

**目标**：用 PC 上的普通内存，模拟出一套完整的 STM32 风格驱动 + 中断骨架。
做完这个项目，你去看真实的 STM32 工程，会认出 80% 的结构。

---

## 怎么跑

```bash
cd /d/c-learn/projects/gpio_sim

make            # 编译
make run        # 运行（main.c 里有完整断言）
make symbols    # 看符号表，理解 static 的可见性
```

---

## 文件分工

| 文件 | 角色 | 你改吗 |
|---|---|---|
| `gpio.h` | GPIO 驱动接口（寄存器结构体、模式枚举、函数声明） | 不改，读懂 |
| `gpio.c` | GPIO 驱动实现 | **主要写这里** |
| `irq.h` | 中断分发器接口 | 不改 |
| `irq.c` | 中断分发器实现 | **也写这里** |
| `main.c` | 应用层：按键中断翻转 LED | 不改 |
| `Makefile` | 构建 | 不改 |

---

## 模块依赖图

```
        main.c
       /      \
      v        v
   gpio.h    irq.h        <- 应用层只依赖接口
      ^        ^
      |        |
   gpio.c    irq.c        <- 实现层，互不知道对方存在
```

**关键**：`gpio.c` 里没有一行 `#include "irq.h"`，`irq.c` 里也没有一行 `#include "gpio.h"`。
两个模块完全解耦，唯一的协作点在 `main.c`。

这就是嵌入式分层的意义：换一颗芯片，只要重写 `gpio.c`，`irq.c` 和 `main.c` 一个字都不用动。

---

## 程序骨架（和真实 STM32 工程一致）

```
上电复位            gpio_sim_reset()        <- 真实硬件自动做
  ↓
初始化外设          gpio_config(...)        <- 真实工程里还有 RCC 开时钟
  ↓
注册中断            irq_attach(...)         <- HAL_NVIC_SetPriority + EnableIRQ
  ↓
while (1) {        主循环                   <- 真实固件永不退出
    处理中断留下的标志位
}
```

---

## 这个项目要你掌握的六件事

### 1. 中断服务函数必须极短

`main.c` 里的 `on_button_pressed` 只有两行：

```c
static void on_button_pressed(void)
{
    g_press_count++;
    gpio_toggle(GPIOA, LED_PIN);
}
```

没有 `printf`、没有 `malloc`、没有循环。
复杂逻辑全部留给主循环 —— 这就是「上半部 / 下半部」模式，
是后面学 FreeRTOS 时的核心概念。

### 2. `volatile` 的实战意义

```c
static volatile unsigned g_press_count = 0;
```

去掉 `volatile`，然后 `make CFLAGS="... -O2"` 试试，你会看到主循环读到旧值。

### 3. 驱动接口的参数检查

`gpio_config(GPIOA, 99, ...)` 必须返回错误而不是写到越界的内存。
嵌入式代码里，**每一个对外接口都要做参数体检**，因为你不知道调用方会传什么进来。

### 4. 函数指针表 = 中断向量表

`irq.c` 里的 `s_vector[]` 就是启动文件里那张表。
`irq_trigger(6)` 触发 `EXTI6`，和真实硬件的中断号跳转是同一回事。

### 5. `static` 实现 C 版「私有」

`gpio.c` 里的 `s_port_a`、`pin_valid` 都是 `static`。
跑 `make symbols` 对比一下：`static` 的符号是小写 `t`/`b`（本地符号），
非 static 的是大写 `T`/`D`（全局符号，能被链接器跨文件找到）。

### 6. 用 BSRR 而不是 ODR 做原子操作

`gpio_write()` 走 BSRR（单次写），`gpio_set()` 走 ODR（读-改-写）。
在中断和主循环都会操作同一个引脚时，只有 BSRR 是安全的。

---

## 验收标准

`make run` 打印：

```
  全部通过。
  你完成了一个完整的驱动 + 中断 + 主循环骨架。
```

并且编译**零警告**。

---

## 做完之后，对照真实 STM32 工程看这几个文件

如果你以后装了 STM32CubeMX 或看了别人的开源工程，重点看这几处，
你会发现自己全都认识了：

| 你会看到 | 对应本项目里的 |
|---|---|
| `typedef struct { __IO uint32_t MODER; ... } GPIO_TypeDef;` | `gpio.h` 里的结构体 |
| `#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)` | `gpio.c` 里的 `GPIOA` 句柄 |
| `HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)` | `gpio_write(GPIOA, 5, 1)` |
| `HAL_GPIO_TogglePin(...)` | `gpio_toggle(...)` |
| `HAL_NVIC_EnableIRQ(EXTI9_5_IRQn)` | `irq_attach(...)` |
| `void EXTI9_5_IRQHandler(void) { ... }` | `on_button_pressed(void)` |
| `RCC->APB2ENR \|= RCC_APB2ENR_IOPAEN;` | 本项目里没有——这是你要补的下一课 |
| `while (1) { ... }` | `main.c` 第 5 步 |
