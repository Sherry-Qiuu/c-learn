/* ============================================================================
 * ex12_ub_traps.c —— 未定义行为（UB）识别训练 · 本包最后一课
 * ----------------------------------------------------------------------------
 * 为什么把 UB 放在最后一课？
 * 因为在 PC 上，UB 通常只是「打印出奇怪的值」或者「偶尔崩一次」，
 * 你可以无视它。但在 STM32 上，UB 是：
 *     - 半夜随机 HardFault，接上仿真器一跑又好了（改了时序）
 *     - 量产之后某批次才出问题（编译器版本不同）
 *     - 一个和 bug 现场毫无关系的变量被改掉
 *
 * 嵌入式工程师的核心竞争力之一，就是「脑子里装着 UB 清单」。
 *
 *   make run/ex12
 *
 * 用法：先看下面的「判断区」，自己写下答案，再翻到文件末尾对答案。
 *
 * ⚠️ 本文件故意保留了两条编译警告，它们是任务本身的一部分，**不要**去修：
 *      main 第 80 行附近  -Wsign-compare       <- 演示 -1 < 1u 这个陷阱
 *      main 第 144 行附近 -Wformat-truncation  <- 演示 snprintf 截断
 *    这正好说明「编译器能帮你抓到一部分问题」。
 *    这也是为什么规矩是「警告一个都不许放过」——但有意识的教学演示除外。
 * ========================================================================== */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <assert.h>

/* ===========================================================================
 *                          判 断 区
 *
 *  下面每题给出一段代码。判断它的行为是：
 *      UB（未定义行为）     还是      确定的（不管结果多反直觉，行为可预期）
 *
 *  请把答案写在这张表里（U = UB，D = determined）：
 *
 *   题号    你的答案      代码
 *   ────    ────────      ────────────────────────────────────────────
 *    1     ________      int i = INT_MAX;  i = i + 1;
 *    2     ________      unsigned u = UINT_MAX;  u = u + 1;
 *    3     ________      int a[5];  a[5] = 1;
 *    4     ________      char *s = "hi";  s[0] = 'H';
 *    5     ________      int *p = NULL;  if (p == NULL) { ... }
 *    6     ________      int *p = NULL;  int x = *p;
 *    7     ________      int x;  x = x + 1;
 *    8     ________      int a[5];  int *p = a + 5;  p - a;
 *    9     ________      1 << 31
 *   10     ________      1u << 31
 *   11     ________      memcpy(p, p + 1, 10);      // p 指向的缓冲区有 16 字节
 *   12     ________      int a[3] = {1,2,3};  int x = a[-1];
 *   13     ________      float f = 0.1f + 0.2f;  if (f == 0.3f) { ... }
 *   14     ________      char buf[4];  strcpy(buf, "hello");
 *   15     ________      unsigned short us = 65535;  us = us + 1;
 *
 *  写完再往下看。自欺欺人没有意义 —— 这些判错一次，将来就要在示波器前
 *  熬夜排查一次。
 * =========================================================================== */

int main(void)
{
    printf("========== 上面的 15 道题，答案在文件末尾 ==========\n");
    printf("先自己判断完，再来跑这个程序。\n\n");

    printf("========== 演示 1：有符号溢出 UB vs 无符号回绕 确定 ==========\n");
    {
        unsigned u = UINT_MAX;
        u = u + 1u;
        printf("  unsigned UINT_MAX + 1 = %u   <- 确定的回绕，标准保证\n", u);
        assert(u == 0u);

        printf("  int INT_MAX + 1      = ?     <- UB！下面这行不能跑\n");
        /* int i = INT_MAX; i = i + 1;   把这行取消注释，在 -O2 下结果不可预测 */
        printf("  实际经验：-O0 下你会看到 -2147483648，\n");
        printf("            -O2 下编译器可能直接假设「这个溢出不会发生」，\n");
        printf("            把整个 if (i > 0) 优化掉。这就是它危险的地方。\n");
    }

    printf("\n========== 演示 2：整数提升陷阱（不是 UB，但反直觉）==========\n");
    {
        int i = -1;
        unsigned u = 1u;

        if (i < u) {
            printf("  -1 < 1u 是 true（反直觉但行为确定）\n");
        } else {
            printf("  -1 < 1u 是 false（反直觉但行为确定）\n");
        }
        printf("  原因：比较时 int 被转成 unsigned，-1 变成 4294967295\n");

        /* 这个 bug 在嵌入式里很常见：len 是 int，函数返回 size_t */
        int len = -1;
        size_t n = (size_t)len;
        printf("  (size_t)(-1) = %zu（在 64 位上是 18446744073709551615）\n", n);
        printf("  所以：凡是和 size_t 比较的变量，一律用 size_t 或显式强转。\n");

        printf("\n  另一个经典：\n");
        printf("    for (size_t i = 5; i >= 0; i--)   // 死循环\n");
        printf("    if (strlen(s) - 1 >= 0)           // 永远为真\n");
        printf("  这两条都会让编译器开心地警告你：comparison of unsigned expression >= 0 is always true\n");
        printf("  所以 -Wall -Wextra 一个警告都不能放过。\n");
    }

    printf("\n========== 演示 3：浮点不能用 == 比较 ==========\n");
    {
        float f = 0.1f + 0.2f;
        printf("  0.1f + 0.2f == 0.3f ?  %s\n", (f == 0.3f) ? "true" : "false");
        printf("  实际值 = %.10f\n", (double)f);
        printf("  0.3f   = %.10f\n", (double)0.3f);

        /* 正确做法 1：比较差值是否小于容差 */
        float eps = 1e-6f;
        float diff = f - 0.3f;
        if (diff < 0) diff = -diff;
        printf("  |f - 0.3f| = %.10f < 1e-6 ? %s  <- 正确的比较方式\n",
               (double)diff, (diff < eps) ? "true" : "false");
        assert(diff < eps);

        /* 正确做法 2：干脆不要用浮点。嵌入式里角度、电压都用定点整数表示 */
        printf("\n  嵌入式建议：能用整数就别用浮点。\n");
        printf("    - 很多 Cortex-M0/M3 没有 FPU，浮点是软件模拟，慢几十倍\n");
        printf("    - 定点数：把 1.23 存成 1230（放大 1000 倍），全程整数运算\n");
        printf("    - 需要浮点时用 double 而不是 float 会更稳，但更慢\n");
    }

    printf("\n========== 演示 4：字符串的两种写法，一种能改一种不能 ==========\n");
    {
        char  modifiable[] = "hello";     /* 在栈上，6 个字节，可以改 */
        const char *readonly = "hello";   /* 指向只读区的字面量，改就是 UB */

        modifiable[0] = 'H';
        printf("  char s[] = \"hello\" 改首字母 -> %s   OK（有自己的一份拷贝）\n", modifiable);
        printf("  const char *s = \"hello\" 指向的是只读区的字面量\n");
        printf("    s[0] = 'H' 在 PC 上通常直接段错误；\n");
        printf("    在 STM32 上，字面量放在 Flash 里，写 Flash 需要解锁+擦除，\n");
        printf("    直接写会触发总线错误 HardFault。\n");
        (void)readonly;
    }

    printf("\n========== 演示 5：安全的字符串操作（正确姿势）==========\n");
    {
        char dst[8];

        /* 危险：不知道 dst 多大 */
        /* strcpy(dst, "this is a very long string");   // 溢出，UB */

        /* 安全：snprintf 知道容量，并且返回「本来想写多长」 */
        int want = snprintf(dst, sizeof dst, "%s", "this is a very long string");
        printf("  snprintf 实际写入 = \"%s\"（长度 %zu）\n", dst, strlen(dst));
        printf("  snprintf 返回值 = %d（如果 >= 8，说明被截断了）\n", want);
        assert(want >= (int)sizeof dst);
        printf("  -> 返回值 >= 缓冲区大小就等于「发生了截断」，必须处理这个情况\n");

        /* 另一个安全姿势：先算需要多大 */
        const char *src = "abcdefghij";
        size_t need = strlen(src) + 1;
        printf("  或者：先 strlen 算出需要 %zu 字节，再去 malloc / 检查缓冲区\n", need);
    }

    printf("\n========== 演示 6：越界读的「隐形」特性 ==========\n");
    {
        /* 越界读往往不崩，只是给你垃圾数据。
         * 这是最危险的 UB —— 因为它不报错。 */
        int a[4] = {10, 20, 30, 40};
        printf("  a[0..3] = %d %d %d %d\n", a[0], a[1], a[2], a[3]);
        printf("  a[4]（越界）会读到栈上紧挨着的字节，值不确定，但通常不崩。\n");
        printf("  嵌入式里的后果：你读到的是「另一个变量的值」，\n");
        printf("  于是程序逻辑莫名其妙地错，但永远不会崩给你看。\n");

        /* 正确的边界检查写法 */
        for (int i = -1; i <= 4; i++) {
            if (i >= 0 && i < 4) {
                printf("    合法索引 %d -> %d\n", i, a[i]);
            } else {
                printf("    拒绝越界索引 %d\n", i);
            }
        }
    }

    printf("\n========== 演示 7：怎么让编译器帮你抓 UB ==========\n");
    {
        printf("  第 1 层（最有效）：把警告全开并当成错误\n");
        printf("     gcc -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow \\\n");
        printf("         -Wcast-align -Wstrict-aliasing -O2 -c file.c\n\n");

        printf("  第 2 层：运行时检查未定义行为\n");
        printf("     gcc -fsanitize=undefined -g file.c -o t.exe\n");
        printf("     跑一遍，任何符号溢出、空指针、越界都会被指出行号\n");
        printf("     （本包的 make san/exNN 就是这个）\n\n");

        printf("  第 3 层：内存错误专门检查\n");
        printf("     gcc -fsanitize=address,undefined -g file.c\n");
        printf("     抓 use-after-free、越界写、内存泄漏\n");
        printf("     注意：ASan 在 MinGW 上支持不完整，PC 上建议用 WSL 或 Linux\n\n");

        printf("  第 4 层：静态分析\n");
        printf("     gcc -fanalyzer -c file.c        （GCC 内置，慢但能查逻辑错误）\n");
        printf("     cppcheck --enable=all file.c\n");
        printf("     clang-tidy                       （有 Clang 时最好用）\n\n");

        printf("  STM32 上的额外手段：\n");
        printf("     - 运行期检查栈溢出（在栈末尾填魔数，定期检查）\n");
        printf("     - HardFault_Handler 里把寄存器 dump 到串口/Flash\n");
        printf("     - 用 MPU 把某些内存区设成只读，越界写立刻触发异常\n");
    }

    printf("\n========== 演示 8：你可能会遇到的「灵异事件」对照表 ==========\n");
    {
        printf("  ┌──────────────────────────────┬──────────────────────────┐\n");
        printf("  │ 症状                          │ 大概率原因                │\n");
        printf("  ├──────────────────────────────┼──────────────────────────┤\n");
        printf("  │ 加了 printf 就好了            │ UB（时间/布局变了）       │\n");
        printf("  │ -O0 正常，-O2 崩              │ UB 或有 volatile 漏加     │\n");
        printf("  │ 某个不相关的变量被改          │ 数组越界写 / 栈溢出       │\n");
        printf("  │ 中断里改的变量主循环读不到    │ 忘加 volatile             │\n");
        printf("  │ 浮点结果每次不一样            │ 浮点比较用 == / 精度问题  │\n");
        printf("  │ 跑几小时后死机                │ 内存碎片 / 计数溢出        │\n");
        printf("  │ 刚开机就 HardFault           │ 时钟没使能就写寄存器       │\n");
        printf("  └──────────────────────────────┴──────────────────────────┘\n");
    }

    printf("\n>>> ex12 全部通过。\n");
    printf(">>> 现在去做两个项目：projects/contact 和 projects/gpio_sim\n");
    return 0;
}

/* ============================================================================
 *                        答案与解析（判断完再看）
 * ============================================================================

   1    U    int i = INT_MAX; i = i + 1;
             有符号整数溢出是 UB。C 标准只说「行为未定义」，不说「回绕成 -2147483648」。
             -O0 下你大概率会看到回绕，-O2 下编译器可能把「i > 0」之类的判断直接省略。
             修法：改用 unsigned，或者先判断 i < INT_MAX 再加。

   2    D    unsigned u = UINT_MAX; u = u + 1;
             无符号数运算的定义就是「模 2^n」，回绕到 0。这是标准明确规定的。
             嵌入式里用无符号数做计数、累加时，回绕是可预测的，可以依赖。

   3    U    int a[5]; a[5] = 1;
             越界写。数组只有 a[0] 到 a[4]。这一写会踩到栈上相邻的变量或返回地址。
             它「看起来能跑」是最可怕的地方 —— 真正被破坏的东西可能几小时后才发作。

   4    U    char *s = "hi"; s[0] = 'H';
             修改字符串字面量是 UB。字面量通常在只读段（PC）或 Flash（STM32）。
             PC 上多半段错误，STM32 上会 HardFault（写 Flash 要先解锁和擦除）。

   5    D    int *p = NULL; if (p == NULL) { ... }
             比较空指针完全合法，这正是防错写法。只有「解引用」空指针才是 UB。

   6    U    int *p = NULL; int x = *p;
             解引用空指针。PC 上段错误，STM32 上访问 0x00000000 会 HardFault
             （0x00000000 在 STM32 上是 Flash 别名区，读也许能读，写一定异常）。
             这是嵌入式第一大 bug，占新手 HardFault 的一半以上。

   7    U    int x; x = x + 1;
             读取未初始化的自动变量。值是不确定的（不一定是 0！栈上是上次的垃圾）。
             注意：全局变量和 static 变量会被编译器清 0，栈变量不会。
             嵌入式里「局部变量没初始化」是很常见的 HardFault 来源。

   8    D    int a[5]; int *p = a + 5; p - a;
             a + 5 是「越界一格」的指针（past-the-end），形成它是合法的，
             参与比较和相减也是合法的，结果就是 5。
             但 *（a + 5) —— 解引用它 —— 就是 UB 了。

   9    U    1 << 31
             1 是有符号 int。左移到符号位（第 31 位）是 UB。
             在 -O0 上你会得到 -2147483648，但它不保证。

  10    D    1u << 31
             1u 是无符号的，无符号左移按模 2^32 定义，结果是 2147483648u。
             这就是为什么嵌入式代码里位运算一律写 1U << n。

  11    U    memcpy(p, p + 1, 10);
             源和目标重叠。memcpy 明确不处理重叠，必须用 memmove。
             这在「数组内整体平移」的场景里极易踩到。

  12    U    int a[3] = {1,2,3}; int x = a[-1];
             负索引越界。等价于 *(a - 1)，读的是数组前一个位置。

  13    D    float f = 0.1f + 0.2f; if (f == 0.3f)
             行为是确定的（IEEE 754 规定得很死），但结果是 false。
             0.1、0.2、0.3 在二进制浮点里都不能精确表示。
             这不是 UB，但结果反直觉，所以实用上要避免用 == 比较浮点。

  14    U    char buf[4]; strcpy(buf, "hello");
             缓冲区溢出写。6 个字节写进 4 个字节的空间，踩掉相邻内存。
             这是 C 安全漏洞史第一名（缓冲区溢出攻击的原型）。
             永远不要用 strcpy / strcat / sprintf 处理未知长度的输入。

  15    D    unsigned short us = 65535; us = us + 1;
             行为确定，但结果要小心：us + 1 里的 us 会先被「整数提升」成 int，
             计算得到 65536（int，没溢出），再赋回 unsigned short 时
             实现定义为 0（C 标准对这种「转成窄类型且值超出范围」的情况
             是 implementation-defined，GCC 上的行为是取模）。

  --- 计分 ---
    15 题全对：你的 UB 直觉已经足够写嵌入式代码了。
    错 1~3 题：回去把错的几条亲手验证一遍（用 make san/exNN）。
    错 4 题以上：重读 notes/cheatsheet_c_for_cpp.md 的第 7 节，再回来做一遍。

  --- 记住这条元规则 ---
   C 里「能编译」「能跑」「结果看起来对」这三件事，都不等于「代码是对的」。
   单片机没有 MMU、没有操作系统帮你兜底。你唯一的防线是：
     1. 脑子里有 UB 清单
     2. -Wall -Wextra -Wpedantic 零警告
     3. 关键路径上手写断言 / 边界检查
     4. 交差之前用 UBSan 跑一遍
 * ========================================================================== */
