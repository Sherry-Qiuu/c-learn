/* ============================================================================
 * ex01_toolchain.c —— 编译链路 · 定长类型 · printf 格式符
 * ----------------------------------------------------------------------------
 * 目标：
 *   1. 亲手走一遍「源码 → 可执行文件」的完整链路（gcc -c / -S / -E）
 *   2. 用 <stdint.h> 的定长类型代替裸 int，这是嵌入式的硬要求
 *   3. 把 printf 格式符练到不会写错——格式符写错在 C 里是未定义行为
 *
 * 编译运行：
 *   make run/ex01
 *   或：gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 exercises/ex01_toolchain.c \
 *          -o build/ex01.exe && ./build/ex01.exe
 *
 * 说明：TODO 没实现之前会报 unused parameter 警告，实现完警告就消失。
 *       这是我故意留的信号，别用 (void)x 把它按下去。
 * ========================================================================== */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

/* ---------------------------------------------------------------------------
 * TODO 1：把 val 格式化成 "0x" + 8 位大写十六进制（不足补 0）写入 buf
 *
 *   期望：
 *     hex32_to_str(0x0000A5C3u, buf, sizeof buf)  ->  buf == "0x0000A5C3"
 *     hex32_to_str(0xFFFFFFFFu, buf, sizeof buf)  ->  buf == "0xFFFFFFFF"
 *     hex32_to_str(0u,          buf, sizeof buf)  ->  buf == "0x00000000"
 *
 *   提示：一个 snprintf 就够。想想格式串里的「宽度」和「补位」怎么写，
 *         以及为什么必须用 snprintf 而不是 sprintf。
 * ------------------------------------------------------------------------- */
void hex32_to_str(uint32_t val, char *buf, size_t bufsz)
{
    /* 在这里写你的代码 */
}

/* ---------------------------------------------------------------------------
 * TODO 2：把 value 度量的「位数」算出来
 *
 *   期望：
 *     bit_width(0u)          -> 0
 *     bit_width(1u)          -> 1
 *     bit_width(0xFu)        -> 4
 *     bit_width(0x100u)      -> 9
 *     bit_width(0xFFFFFFFFu) -> 32
 *
 *   这是嵌入式里非常常见的操作（算掩码、算寄存器位域宽度）。
 *   提示：循环右移，或者用「不断除以 2」的思路。
 * ------------------------------------------------------------------------- */
unsigned bit_width(uint32_t value)
{
    /* 在这里写你的代码 */
    return 0;
}

int main(void)
{
    printf("========== 任务 0：先看懂这些类型到底多大 ==========\n");
    printf("char      = %zu 字节\n", sizeof(char));
    printf("short     = %zu 字节\n", sizeof(short));
    printf("int       = %zu 字节   <-- 注意：C 标准只保证 >= 2 字节\n", sizeof(int));
    printf("long      = %zu 字节\n", sizeof(long));
    printf("long long = %zu 字节\n", sizeof(long long));
    printf("pointer   = %zu 字节\n", sizeof(void *));
    printf("size_t    = %zu 字节\n", sizeof(size_t));
    printf("\n定长类型（嵌入式一律用这套）：\n");
    printf("uint8_t   = %zu 字节\n", sizeof(uint8_t));
    printf("uint16_t  = %zu 字节\n", sizeof(uint16_t));
    printf("uint32_t  = %zu 字节   <-- STM32 的寄存器几乎全是它\n", sizeof(uint32_t));
    printf("uint64_t  = %zu 字节\n", sizeof(uint64_t));

    printf("\n========== 任务 1：十六进制格式化 ==========\n");
    {
        char buf[16];

        hex32_to_str(0x0000A5C3u, buf, sizeof buf);
        assert(strcmp(buf, "0x0000A5C3") == 0);
        printf("  0x0000A5C3 -> \"%s\"  OK\n", buf);

        hex32_to_str(0xFFFFFFFFu, buf, sizeof buf);
        assert(strcmp(buf, "0xFFFFFFFF") == 0);
        printf("  0xFFFFFFFF -> \"%s\"  OK\n", buf);

        hex32_to_str(0u, buf, sizeof buf);
        assert(strcmp(buf, "0x00000000") == 0);
        printf("  0x00000000 -> \"%s\"  OK\n", buf);
    }

    printf("\n========== 任务 2：算位宽 ==========\n");
    {
        assert(bit_width(0u) == 0);
        assert(bit_width(1u) == 1);
        assert(bit_width(2u) == 2);
        assert(bit_width(0xFu) == 4);
        assert(bit_width(0x100u) == 9);
        assert(bit_width(0xFFFFFFFFu) == 32);
        printf("  bit_width 全部通过\n");
    }

    printf("\n========== 任务 3：先猜结果，再取消注释跑 ==========\n");
    {
        /* --- 无符号溢出是「回绕」，行为完全确定 --- */
        uint8_t u8 = 250;
        u8 = (uint8_t)(u8 + 10);
        printf("  (uint8_t)(250 + 10) = %u  <-- 回绕到 4\n", (unsigned)u8);
        assert(u8 == 4);

        /* --- 有符号溢出是 UB，编译器可以干任何事 ---
         * int i = 2147483647;
         * i = i + 1;            <-- 别跑，这是未定义行为
         */

        /* --- size_t 是无符号，这个经典陷阱的循环永不结束 ---
         * for (size_t i = 5; i >= 0; i--) { ... }    i 到 0 后再减 1 变成巨大正数
         */

        /* --- 同理：这个判断永远为真 ---
         * if (strlen(s) - 1 >= 0) { ... }            因为左边是无符号
         */
        printf("  以上两条陷阱已在注释里，自己改一版跑跑看\n");
    }

    printf("\n========== 任务 4：看编译器到底做了什么 ==========\n");
    printf("  在终端里依次敲（把 ex01 换成你想看的练习名）：\n");
    printf("    gcc -std=c17 -E exercises/ex01_toolchain.c -o build/ex01.i   # 只看预处理\n");
    printf("    gcc -std=c17 -O0 -S exercises/ex01_toolchain.c -o build/ex01.s  # 生成汇编\n");
    printf("    gcc -std=c17 -O0 -c exercises/ex01_toolchain.c -o build/ex01.o  # 生成目标文件\n");
    printf("    objdump -d build/ex01.o | head -40                            # 反汇编\n");
    printf("  打开 build/ex01.s，找到 hex32_to_str 那一段，数数它占了多少条指令。\n");
    printf("  这就是「C 代码 → 机器指令」的最后一公里。\n");

    printf("\n>>> ex01 全部通过。下一站：ex02_pointer_basics.c\n");
    return 0;
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

void hex32_to_str(uint32_t val, char *buf, size_t bufsz)
{
    snprintf(buf, bufsz, "0x%08X", (unsigned)val);
}

unsigned bit_width(uint32_t value)
{
    unsigned n = 0;
    while (value != 0u) {
        n++;
        value >>= 1;
    }
    return n;
}

  为什么用 snprintf 而不是 sprintf：
    sprintf 不知道 buf 有多大，写超了就直接踩坏相邻内存。
    snprintf 第二参数是 buf 的容量，最多写 bufsz-1 个字符再补 '\0'，
    这是 C 里唯一安全的格式化输出方式。

  为什么 "%08X" 能补零：
    %X      无符号十六进制、大写
    0       用 0 填充（默认是空格填充）
    8       最小宽度 8
    合起来：不足 8 位就在左边补 0

  为什么 (unsigned)val 这个强转看起来多余？
    uint32_t 在 MinGW 64 位下就是 unsigned int，所以是可以省。
    但写上它是个好习惯——换个平台 uint32_t 可能是 unsigned long，
    那时 %X 和实参类型不匹配就是 UB，强转一下就永远安全。

  为什么 bit_width 用无符号右移：
    对负数右移是实现定义行为（算术移位还是逻辑移位看编译器），
    而无符号右移一定是逻辑移位、空位补 0，行为完全确定。
    C 里凡是位运算，一律用无符号类型——这条规则能帮你躲掉一半的 UB。
 * ========================================================================== */
