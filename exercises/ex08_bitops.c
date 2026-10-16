/* ============================================================================
 * ex08_bitops.c —— 位运算：嵌入式的母语（本包最重要的一课）
 * ----------------------------------------------------------------------------
 * 你在 PC 上写 C++ 时，位运算可能一年用不到一次。
 * 到了 STM32，你 80% 的代码都在和位打交道：
 *   打开某个外设时钟、配置某个引脚为复用模式、读取某个状态标志位……
 *   全都是「读一个 32 位的寄存器，改其中几位，写回去」。
 *
 *   make run/ex08
 * ========================================================================== */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

/* ---------------------------------------------------------------------------
 * 核心操作四件套：置位 / 清位 / 翻位 / 查位
 *   bit 的取值范围 0..31（本练习只做 32 位寄存器的低 32 位）
 * ------------------------------------------------------------------------- */

/* TODO 1：把第 bit 位设成 1，其他位不动 */
uint32_t reg_set_bit(uint32_t reg, unsigned bit)
{
    /* 在这里写你的代码 */
    return reg;
}

/* TODO 2：把第 bit 位清成 0，其他位不动 */
uint32_t reg_clear_bit(uint32_t reg, unsigned bit)
{
    /* 在这里写你的代码 */
    return reg;
}

/* TODO 3：把第 bit 位取反，其他位不动 */
uint32_t reg_toggle_bit(uint32_t reg, unsigned bit)
{
    /* 在这里写你的代码 */
    return reg;
}

/* TODO 4：读取第 bit 位，返回 0 或 1 */
int reg_get_bit(uint32_t reg, unsigned bit)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ---------------------------------------------------------------------------
 * 更常用的是「多位字段」的读写。比如某个寄存器里：
 *   第 4~6 位（3 位宽） 表示时钟分频系数
 *   第 8~11 位（4 位宽）表示数据长度
 * 你需要能把这几位置成任意值，而不影响别的位。
 * ------------------------------------------------------------------------- */

/* TODO 5：把 reg 的第 shift 位开始、宽度 width 的字段写成 value
 *   步骤一定是「先清干净，再写入」—— 顺序错了会留下垃圾位。
 *   要求：value 超出 width 能表示的范围时，只保留低位（自动截断）
 * ------------------------------------------------------------------------- */
uint32_t reg_write_field(uint32_t reg, unsigned shift, unsigned width, uint32_t value)
{
    /* 在这里写你的代码 */
    return reg;
}

/* TODO 6：读出 reg 的第 shift 位开始、宽度 width 的字段（结果靠右对齐）*/
uint32_t reg_read_field(uint32_t reg, unsigned shift, unsigned width)
{
    /* 在这里写你的代码 */
    return 0;
}

/* ---------------------------------------------------------------------------
 * TODO 7：把数字转成二进制字符串（32 位，左边补 0）
 *   期望：to_binary(0x00000005u, buf, 33) -> "00000000000000000000000000000101"
 *   这是你调试寄存器时最常用的工具——printf("%08X") 看不懂的位，
 *   用二进制一眼就看出来了。
 * ------------------------------------------------------------------------- */
void to_binary(uint32_t v, char *buf, size_t bufsz)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 8：统计 1 的个数（popcount）
 *   朴素做法循环 32 次；进阶做法（Brian Kernighan）循环「1 的个数」次。
 *   这里用朴素做法就够了。
 * ------------------------------------------------------------------------- */
unsigned popcount32(uint32_t v)
{
    /* 在这里写你的代码 */
    return 0;
}

/* ---------------------------------------------------------------------------
 * TODO 9：位颠倒（bit reversal，32 位）
 *   期望：reverse_bits32(0x00000001u) == 0x80000000u
 *
 *   为什么需要这个？某些外设（比如 SPI 某些模式、部分传感器）发送数据时
 *   要求「低位先发」（LSB first），你就得先把整个字节序倒过来。
 * ------------------------------------------------------------------------- */
uint32_t reverse_bits32(uint32_t v)
{
    /* 在这里写你的代码 */
    return v;
}

/* ---------------------------------------------------------------------------
 * TODO 10：取最低的那个 1（比如 0b0110 -> 0b0010；0 -> 0）
 *   提示：v & (v - 1) 会清掉最低位的 1，而 v ^ (v & (v-1)) 就把它单独留出来了
 * ------------------------------------------------------------------------- */
uint32_t lowest_set_bit(uint32_t v)
{
    /* 在这里写你的代码 */
    return 0;
}

int main(void)
{
    printf("========== 任务 1-4：置位 / 清位 / 翻位 / 查位 ==========\n");
    {
        uint32_t r = 0x00000000u;

        r = reg_set_bit(r, 5);
        assert(r == 0x00000020u);
        printf("  set_bit(0x00000000, 5)    -> 0x%08X\n", r);

        r = reg_set_bit(r, 0);
        assert(r == 0x00000021u);
        printf("  set_bit(0x00000020, 0)    -> 0x%08X\n", r);

        r = reg_clear_bit(r, 5);
        assert(r == 0x00000001u);
        printf("  clear_bit(0x00000021, 5)  -> 0x%08X\n", r);

        r = reg_toggle_bit(r, 0);
        assert(r == 0x00000000u);
        printf("  toggle_bit(0x00000001, 0) -> 0x%08X\n", r);

        r = reg_toggle_bit(r, 0);
        assert(r == 0x00000001u);
        printf("  toggle_bit(0x00000000, 0) -> 0x%08X\n", r);

        assert(reg_get_bit(0x00000001u, 0) == 1);
        assert(reg_get_bit(0x00000001u, 1) == 0);
        assert(reg_get_bit(0x80000000u, 31) == 1);
        printf("  get_bit 边界（第 0 位 / 第 31 位）  OK\n");
    }

    printf("\n========== 任务 5/6：多位字段读写 ==========\n");
    {
        uint32_t r = 0xFFFFFFFFu;

        /* 把第 4~6 位（3 位宽）写成 0b101 = 5 */
        r = reg_write_field(r, 4, 3, 5u);
        printf("  全 1 基础上写 4~6 位 = 5  -> 0x%08X\n", r);
        assert(reg_read_field(r, 4, 3) == 5u);
        assert((r & ~(0x7u << 4)) == (0xFFFFFFFFu & ~(0x7u << 4)));   /* 别的位没动 */

        /* 再写一次别的值 */
        r = reg_write_field(r, 4, 3, 0u);
        assert(reg_read_field(r, 4, 3) == 0u);
        printf("  同一位段改写成 0            -> 0x%08X\n", r);

        /* 截断测试：3 位宽最多表示 7，写 0b1111 应该只保留低 3 位 = 7 */
        r = reg_write_field(0u, 4, 3, 0xFu);
        assert(reg_read_field(r, 4, 3) == 7u);
        printf("  写 0xF 进 3 位宽字段 -> 读到 %u（自动截断）\n", reg_read_field(r, 4, 3));

        /* 读写 1 位宽 = 等价于 set/clear */
        r = reg_write_field(0u, 9, 1, 1u);
        assert(reg_get_bit(r, 9) == 1);
        printf("  1 位宽字段等价于 set_bit  OK\n");
    }

    printf("\n========== 任务 7：二进制打印 ==========\n");
    {
        char buf[33];
        to_binary(0x00000005u, buf, sizeof buf);
        printf("  0x00000005 -> %s\n", buf);
        assert(buf[31] == '1' && buf[30] == '0' && buf[29] == '1');
        assert(buf[0] == '0');

        to_binary(0xFFFFFFFFu, buf, sizeof buf);
        assert(buf[0] == '1' && buf[31] == '1');
        printf("  0xFFFFFFFF -> %s\n", buf);

        to_binary(0u, buf, sizeof buf);
        assert(buf[31] == '0');
        printf("  0x00000000 -> %s\n", buf);
    }

    printf("\n========== 任务 8：popcount ==========\n");
    assert(popcount32(0u) == 0);
    assert(popcount32(1u) == 1);
    assert(popcount32(0xFFFFFFFFu) == 32);
    assert(popcount32(0x0000000Fu) == 4);
    assert(popcount32(0x80000001u) == 2);
    printf("  全部通过  OK\n");

    printf("\n========== 任务 9：位颠倒 ==========\n");
    assert(reverse_bits32(0x00000001u) == 0x80000000u);
    assert(reverse_bits32(0x80000000u) == 0x00000001u);
    assert(reverse_bits32(0xFFFFFFFFu) == 0xFFFFFFFFu);
    assert(reverse_bits32(0u) == 0u);
    assert(reverse_bits32(0x0000FFFFu) == 0xFFFF0000u);
    printf("  全部通过  OK\n");

    printf("\n========== 任务 10：取最低位的 1 ==========\n");
    assert(lowest_set_bit(0u) == 0u);
    assert(lowest_set_bit(1u) == 1u);
    assert(lowest_set_bit(0x6u) == 0x2u);
    assert(lowest_set_bit(0xC0u) == 0x40u);
    assert(lowest_set_bit(0x80000000u) == 0x80000000u);
    printf("  全部通过  OK\n");

    printf("\n========== 任务 11：四个必背技巧 ==========\n");
    {
        /* (1) 不用临时变量交换 */
        uint32_t a = 0xDEADBEEFu, b = 0x12345678u;
        a ^= b; b ^= a; a ^= b;
        assert(a == 0x12345678u && b == 0xDEADBEEFu);
        printf("  (1) 异或交换：a=0x%08X b=0x%08X  OK\n", a, b);
        printf("      注意：只有当 a、b 是「不同的变量」时才成立。\n");
        printf("      如果 a 和 b 指向同一块内存（比如 swap(&x,&x)），会被清零。\n\n");

        /* (2) 判断是不是 2 的幂 */
        for (uint32_t v = 1; v <= 1024u; v <<= 1) {
            assert(v != 0u && (v & (v - 1u)) == 0u);
        }
        assert((6u & (6u - 1u)) != 0u);
        printf("  (2) 2 的幂判断：v && !(v & (v-1))  OK\n");
        printf("      原理：2 的幂只有一个 1，减 1 后所有低位变 1，与自己相与必为 0\n\n");

        /* (3) 向上取整到 2 的幂（分配内存时常用）*/
        uint32_t n = 100u;
        uint32_t p = 1u;
        while (p < n) p <<= 1;
        assert(p == 128u);
        printf("  (3) 100 向上取整到 2 的幂 = %u  OK\n\n", p);

        /* (4) 用位运算做「分组标记」——嵌入式里用 32 位表示 32 个事件状态 */
        uint32_t events = 0u;
        events |= (1u << 3);    /* 事件 3 发生 */
        events |= (1u << 17);   /* 事件 17 发生 */
        assert(reg_get_bit(events, 3) == 1);
        assert(reg_get_bit(events, 17) == 1);
        assert(reg_get_bit(events, 4) == 0);
        printf("  (4) 一个 uint32_t 记录 32 个布尔状态，events = 0x%08X  OK\n\n", events);
    }

    printf("========== 任务 12：新手最常犯的四个错 ==========\n");
    {
        printf("  【错 1】用 |= 清位：\n");
        printf("     reg |= (1U << 5);   // 这是置位，不是清位！\n");
        printf("     清位必须写 reg &= ~(1U << 5);\n\n");

        printf("  【错 2】忘了写 U：\n");
        printf("     reg |= (1 << 31);    // 1 是有符号 int，左移 31 位是 UB\n");
        printf("     reg |= (1U << 31);   // 正确\n\n");

        printf("  【错 3】改多位字段没有先清零：\n");
        printf("     reg |= (v << 4);     // 旧的位还留着，结果是「按位或」不是「赋值」\n");
        printf("     正解：reg &= ~(mask << 4); reg |= (v << 4);\n\n");

        printf("  【错 4】以为 |= 是原子的：\n");
        printf("     reg |= (1U << 5)  其实是「读 -> 改 -> 写」三条指令。\n");
        printf("     如果这里中断打断，中断里也改了同一个寄存器，\n");
        printf("     你回去写的时候会把中断的修改覆盖掉。\n");
        printf("     这叫「读改写竞态」，是 RTOS 里引入互斥量/临界区的原因之一。\n");
    }

    printf("\n========== 任务 13：看看 -O2 会做什么 ==========\n");
    printf("  popcount32 是用循环写的，但 GCC 认识这个模式，\n");
    printf("  在 -O2 下很可能把它编译成一条 POPCNT 指令。\n");
    printf("  验证：gcc -O2 -S exercises/ex08_bitops.c -o build/ex08_o2.s\n");
    printf("        然后搜 popcnt\n");
    printf("  这告诉你一件事：先写清楚的代码，优化交给编译器；\n");
    printf("  但寄存器访问（ex10 的 volatile）编译器不敢乱优化，你也不用担心。\n");

    printf("\n>>> ex08 全部通过。下一站：ex09_function_pointer.c\n");
    return 0;
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

uint32_t reg_set_bit(uint32_t reg, unsigned bit)
{
    return reg | (1u << bit);
}

uint32_t reg_clear_bit(uint32_t reg, unsigned bit)
{
    return reg & ~(1u << bit);
}

uint32_t reg_toggle_bit(uint32_t reg, unsigned bit)
{
    return reg ^ (1u << bit);
}

int reg_get_bit(uint32_t reg, unsigned bit)
{
    return (reg >> bit) & 1u;
}

uint32_t reg_write_field(uint32_t reg, unsigned shift, unsigned width, uint32_t value)
{
    uint32_t mask = ((width >= 32u) ? 0xFFFFFFFFu : ((1u << width) - 1u)) << shift;
    reg &= ~mask;                              // 第一步：清干净
    reg |= (value << shift) & mask;            // 第二步：写入（用 mask 截断）
    return reg;
}

uint32_t reg_read_field(uint32_t reg, unsigned shift, unsigned width)
{
    if (width >= 32u) return reg >> shift;
    return (reg >> shift) & ((1u << width) - 1u);
}

void to_binary(uint32_t v, char *buf, size_t bufsz)
{
    if (buf == NULL || bufsz == 0) return;
    size_t n = (bufsz - 1 < 32) ? bufsz - 1 : 32;
    for (size_t i = 0; i < n; i++) {
        unsigned bit = 31u - (unsigned)i;
        buf[i] = ((v >> bit) & 1u) ? '1' : '0';
    }
    buf[n] = '\0';
}

unsigned popcount32(uint32_t v)
{
    unsigned n = 0;
    while (v != 0u) {
        n += (unsigned)(v & 1u);
        v >>= 1;
    }
    return n;
}

uint32_t reverse_bits32(uint32_t v)
{
    uint32_t r = 0;
    for (unsigned i = 0; i < 32u; i++) {
        r = (r << 1) | (v & 1u);
        v >>= 1;
    }
    return r;
}

uint32_t lowest_set_bit(uint32_t v)
{
    return v & (~v + 1u);     // v & -v 的无符号写法
}

  --- 必须记住的公式表 ---

    置位       reg |=  (1u << n);
    清位       reg &= ~(1u << n);
    翻位       reg ^=  (1u << n);
    查位       (reg >> n) & 1u
    取掩码     (1u << width) - 1u
    写字段     reg = (reg & ~(mask << shift)) | ((value << shift) & (mask << shift));
    读字段     (reg >> shift) & mask
    v 的最低 1  v & (~v + 1u)
    清最低 1   v & (v - 1u)
    判 2 的幂  v != 0 && (v & (v - 1u)) == 0

  --- 为什么到处都要写 U 后缀 ---

    C 里 1 的类型是 int（有符号）。对「有符号数」左移到符号位是未定义行为：
        1 << 31        UB（结果理论上可以是负数，编译器可以做任何事）
        1u << 31       合法，得到 2147483648u
    所以嵌入式代码规范里，凡是有位运算的字面量，一律加 U。

  --- 为什么写字段必须先清再写 ---

    假设原本第 4~6 位是 0b011，你要写 0b101。
      只做 reg |= (5u << 4)：0b011 | 0b101 = 0b111   —— 错，变成 7 了
      先清：reg &= ~(0b111 << 4)  然后 reg |= (5u << 4)  —— 对，得到 0b101

    这个错误极其常见，而且是「有时对有时错」，因为只有旧值和新值
    在某位上同时为 1 时才暴露。这就是为什么嵌入式代码必须写单测。

  --- v & (~v + 1u) 为什么等于 v & (-v) ---

    补码里，-v 就是按位取反再 +1。对无符号数，(~v + 1u) 正好是 v 的
    模 2^32 加法逆元。位模式上表现为「保留最低位的 1，其余位反转」，
    所以 v & (~v + 1u) 就只剩最低的那个 1。
    但直接写 -v 在 v 是有符号时可能溢出，所以无符号场景统一写 ~v + 1u。
 * ========================================================================== */
