/* ============================================================================
 * ex05_string_manual.c —— 手写字符串库（不许调 <string.h>）
 * ----------------------------------------------------------------------------
 * 为什么要把标准库重写一遍？
 *   因为 C 的字符串就是「char 数组 + '\0'」，没有长度字段。
 *   只有你自己写过一遍，才知道 strlen 为什么是 O(n)、
 *   strcpy 为什么会溢出、strncpy 为什么老忘补 '\0'。
 *
 * 规则：本文件里你可以用 printf / assert，但禁止使用 strlen / strcpy /
 *       strcat / strcmp / strchr / atoi / itoa / memcpy / memset。
 *       一个都不许调——写了就等于没学。
 *
 *   make run/ex05
 * ========================================================================== */

#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>   /* 只为了 main 里的 strcmp 做校验，别在实现里用 */

/* ---------------------------------------------------------------------------
 * TODO 1：长度（不含结尾的 '\0'）
 * ------------------------------------------------------------------------- */
size_t my_strlen(const char *s)
{
    /* 在这里写你的代码 */
    return 0;
}

/* ---------------------------------------------------------------------------
 * TODO 2：复制（包括 '\0'），返回 dest
 *   调用方保证 dest 足够大——这是 C 的约定，也是它危险的地方。
 * ------------------------------------------------------------------------- */
char *my_strcpy(char *dest, const char *src)
{
    /* 在这里写你的代码 */
    return dest;
}

/* ---------------------------------------------------------------------------
 * TODO 3：限长复制
 *   最多复制 n 个字符；如果 src 比 n 短，剩余部分用 '\0' 填满。
 *   返回 dest。
 *
 *   期望：
 *     char b[10]; my_strncpy(b, "abc", 5);
 *     b[0]='a' b[1]='b' b[2]='c' b[3]='\0' b[4]='\0'
 *   注意：真正的标准 strncpy 不会在后面补 '\0'，所以它超级容易出 bug。
 *         我们这个版本「更安全」，你写完之后再去读一次 man strncpy 体会差别。
 * ------------------------------------------------------------------------- */
char *my_strncpy(char *dest, const char *src, size_t n)
{
    /* 在这里写你的代码 */
    return dest;
}

/* ---------------------------------------------------------------------------
 * TODO 4：追加（把 src 接到 dest 的 '\0' 处），返回 dest
 * ------------------------------------------------------------------------- */
char *my_strcat(char *dest, const char *src)
{
    /* 在这里写你的代码 */
    return dest;
}

/* ---------------------------------------------------------------------------
 * TODO 5：比较
 *   返回 <0 表示 a < b，0 表示相等，>0 表示 a > b
 *   比较的是「第一个不同字符的差值」，不是长度差。
 * ------------------------------------------------------------------------- */
int my_strcmp(const char *a, const char *b)
{
    /* 在这里写你的代码 */
    return 0;
}

/* ---------------------------------------------------------------------------
 * TODO 6：找字符，返回第一次出现的指针；找不到返回 NULL
 *   额外要求：s 为 NULL 时返回 NULL（真正的 strchr 没这个检查）
 * ------------------------------------------------------------------------- */
char *my_strchr(const char *s, char c)
{
    /* 在这里写你的代码 */
    return NULL;
}

/* ---------------------------------------------------------------------------
 * TODO 7：字符串转整数（就像 atoi）
 *   要处理：前导空白、正负号、遇到第一个非数字字符就停
 *   期望：
 *     "123"      -> 123
 *     "  -456"   -> -456
 *     "+789"     -> 789
 *     "12abc"    -> 12
 *     "abc"      -> 0
 *     ""         -> 0
 * ------------------------------------------------------------------------- */
int my_atoi(const char *s)
{
    /* 在这里写你的代码 */
    return 0;
}

/* ---------------------------------------------------------------------------
 * TODO 8：整数转字符串（就像 itoa）
 *   base 支持 10 和 16（16 用小写字母）
 *   返回写入的字符数（不含 '\0'）
 *
 *   期望：
 *     my_itoa(0,    buf, 10)  -> buf = "0",      返回 1
 *     my_itoa(1234, buf, 10)  -> buf = "1234",   返回 4
 *     my_itoa(-567, buf, 10)  -> buf = "-567",   返回 4
 *     my_itoa(255,  buf, 16)  -> buf = "ff",     返回 2
 *
 *   坑：-2147483648（INT_MIN）取负会溢出，是 UB。
 *       用 unsigned 的做法绕开，参考文件末尾的说明。
 * ------------------------------------------------------------------------- */
int my_itoa(int value, char *buf, int base)
{
    /* 在这里写你的代码 */
    return 0;
}

/* ---------------------------------------------------------------------------
 * TODO 9：内存块复制（就像 memcpy）
 *   注意参数是 void*，这是 C 里实现「泛型」的唯一手段。
 *   要求：按字节复制 n 个字节。
 * ------------------------------------------------------------------------- */
void *my_memcpy(void *dest, const void *src, size_t n)
{
    /* 在这里写你的代码 */
    return dest;
}

/* ---------------------------------------------------------------------------
 * TODO 10：内存块填充（就像 memset）
 * ------------------------------------------------------------------------- */
void *my_memset(void *dest, int byte, size_t n)
{
    /* 在这里写你的代码 */
    return dest;
}

int main(void)
{
    printf("========== 任务 1：strlen ==========\n");
    assert(my_strlen("") == 0);
    assert(my_strlen("a") == 1);
    assert(my_strlen("hello") == 5);
    assert(my_strlen("hello\0world") == 5);   /* 遇到 '\0' 就停 */
    printf("  全部通过  OK\n");

    printf("\n========== 任务 2：strcpy ==========\n");
    {
        char b[32];
        my_strcpy(b, "hello");
        assert(strcmp(b, "hello") == 0);
        my_strcpy(b, "");
        assert(b[0] == '\0');
        printf("  全部通过  OK\n");
    }

    printf("\n========== 任务 3：strncpy ==========\n");
    {
        char b[10];
        for (size_t i = 0; i < sizeof b; i++) b[i] = 'X';   /* 先污染 */

        my_strncpy(b, "abc", 5);
        assert(b[0] == 'a' && b[1] == 'b' && b[2] == 'c');
        assert(b[3] == '\0' && b[4] == '\0');               /* 必须补满 */
        assert(b[5] == 'X');                                /* 不能越界写 */

        my_strncpy(b, "abcdefgh", 4);                       /* src 比 n 长 */
        assert(b[0] == 'a' && b[3] == 'd');
        printf("  全部通过  OK\n");
    }

    printf("\n========== 任务 4：strcat ==========\n");
    {
        char b[32] = "foo";
        my_strcat(b, "bar");
        assert(strcmp(b, "foobar") == 0);
        my_strcat(b, "");
        assert(strcmp(b, "foobar") == 0);
        my_strcat(b, "!");
        assert(strcmp(b, "foobar!") == 0);
        printf("  全部通过  OK\n");
    }

    printf("\n========== 任务 5：strcmp ==========\n");
    {
        assert(my_strcmp("abc", "abc") == 0);
        assert(my_strcmp("abc", "abd") < 0);
        assert(my_strcmp("abd", "abc") > 0);
        assert(my_strcmp("abc", "ab") > 0);      /* 长的大 */
        assert(my_strcmp("", "") == 0);
        assert(my_strcmp("", "a") < 0);
        printf("  全部通过  OK\n");
    }

    printf("\n========== 任务 6：strchr ==========\n");
    {
        const char *s = "hello world";
        assert(my_strchr(s, 'o') == s + 4);
        assert(my_strchr(s, 'z') == NULL);
        assert(my_strchr(NULL, 'a') == NULL);
        assert(my_strchr(s, '\0') == s + 11);   /* 找结尾也能找到 */
        printf("  全部通过  OK\n");
    }

    printf("\n========== 任务 7：atoi ==========\n");
    assert(my_atoi("123") == 123);
    assert(my_atoi("  -456") == -456);
    assert(my_atoi("+789") == 789);
    assert(my_atoi("12abc") == 12);
    assert(my_atoi("abc") == 0);
    assert(my_atoi("") == 0);
    assert(my_atoi("0") == 0);
    printf("  全部通过  OK\n");

    printf("\n========== 任务 8：itoa ==========\n");
    {
        char b[32];
        int n;

        n = my_itoa(0, b, 10);      assert(n == 1 && strcmp(b, "0") == 0);
        n = my_itoa(1234, b, 10);   assert(n == 4 && strcmp(b, "1234") == 0);
        n = my_itoa(-567, b, 10);   assert(n == 4 && strcmp(b, "-567") == 0);
        n = my_itoa(255, b, 16);    assert(n == 2 && strcmp(b, "ff") == 0);
        n = my_itoa(-1, b, 16);     assert(n > 0);
        printf("  1..4 通过；-1 的十六进制 = \"%s\"（%d 位）\n", b, n);

        /* 最难的一题：INT_MIN */
        n = my_itoa(-2147483647 - 1, b, 10);
        assert(strcmp(b, "-2147483648") == 0);
        printf("  INT_MIN -> \"%s\"  OK（这一步过了说明你绕开了溢出）\n", b);
    }

    printf("\n========== 任务 9/10：memcpy / memset ==========\n");
    {
        unsigned char src[8] = {1, 2, 3, 4, 5, 6, 7, 8};
        unsigned char dst[8] = {0};
        my_memcpy(dst, src, 8);
        for (int i = 0; i < 8; i++) assert(dst[i] == i + 1);
        printf("  my_memcpy  OK\n");

        my_memset(dst, 0xAA, 8);
        for (int i = 0; i < 8; i++) assert(dst[i] == 0xAA);
        printf("  my_memset  OK\n");

        /* 关键认知：memset 是按「字节」填，不是按「元素」填 */
        int arr[4];
        my_memset(arr, 0, sizeof arr);          /* 清 0 正确 */
        for (int i = 0; i < 4; i++) assert(arr[i] == 0);
        printf("  memset(..., 0, sizeof arr) 清 0 正确\n");

        my_memset(arr, 1, sizeof arr);          /* 想全填 1？错！ */
        printf("  想用 memset 把 int 数组填 1，结果 arr[0] = %d（不是 1）\n", arr[0]);
        printf("  原因：每个字节被填成 0x01，4 个字节合起来是 0x01010101 = 16843009\n");
        printf("  记住：memset 只适合填 0 或 0xFF\n");
    }

    printf("\n========== 任务 11：指针版 vs 数组版，性能一样吗 ==========\n");
    {
        printf("  编译器会把 a[i] 和 *(a + i) 编译成完全相同的指令。\n");
        printf("  想验证：make asm/ex05，然后对比生成的两段汇编。\n");
        printf("  真正影响性能的是「有没有重复计算」，比如：\n");
        printf("    for (i = 0; i < strlen(s); i++)        <-- 每次循环都算一遍长度\n");
        printf("    size_t n = strlen(s); for (i = 0; i < n; i++)  <-- 只算一次\n");
    }

    printf("\n>>> ex05 全部通过。下一站：ex06_malloc_dynarray.c\n");
    return 0;
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

size_t my_strlen(const char *s)
{
    const char *p = s;
    while (*p != '\0') p++;
    return (size_t)(p - s);
}

char *my_strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++) != '\0') { }
    return dest;
}

char *my_strncpy(char *dest, const char *src, size_t n)
{
    size_t i = 0;
    for (; i < n && src[i] != '\0'; i++) dest[i] = src[i];
    for (; i < n; i++) dest[i] = '\0';
    return dest;
}

char *my_strcat(char *dest, const char *src)
{
    char *d = dest;
    while (*d != '\0') d++;
    while ((*d++ = *src++) != '\0') { }
    return dest;
}

int my_strcmp(const char *a, const char *b)
{
    while (*a != '\0' && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

char *my_strchr(const char *s, char c)
{
    if (s == NULL) return NULL;
    for (; *s != '\0'; s++) {
        if (*s == c) return (char *)s;
    }
    return (c == '\0') ? (char *)s : NULL;
}

int my_atoi(const char *s)
{
    if (s == NULL) return 0;
    while (*s == ' ' || *s == '\t' || *s == '\n') s++;

    int sign = 1;
    if (*s == '+' || *s == '-') {
        if (*s == '-') sign = -1;
        s++;
    }

    int v = 0;
    while (*s >= '0' && *s <= '9') {
        v = v * 10 + (*s - '0');
        s++;
    }
    return sign * v;
}

int my_itoa(int value, char *buf, int base)
{
    static const char digits[] = "0123456789abcdef";

    char tmp[36];
    int  i = 0;
    int  neg = 0;

    unsigned int u;
    if (value < 0 && base == 10) {
        neg = 1;
        u = (unsigned int)(-(value + 1)) + 1u;   // 绕开 INT_MIN 取负的 UB
    } else {
        u = (unsigned int)value;
    }

    if (u == 0u) tmp[i++] = '0';
    while (u != 0u) {
        tmp[i++] = digits[u % (unsigned int)base];
        u /= (unsigned int)base;
    }
    if (neg) tmp[i++] = '-';

    int len = i;
    for (int k = 0; k < len; k++) {
        buf[k] = tmp[len - 1 - k];
    }
    buf[len] = '\0';
    return len;
}

void *my_memcpy(void *dest, const void *src, size_t n)
{
    unsigned char       *d = dest;
    const unsigned char *s = src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];   // 按字节，所以任何类型都能拷
    return dest;
}

void *my_memset(void *dest, int byte, size_t n)
{
    unsigned char *d = dest;
    for (size_t i = 0; i < n; i++) d[i] = (unsigned char)byte;
    return dest;
}

  --- 几个必须说清楚的坑 ---

  1. while ((*d++ = *src++) != '\0') { }  这行怎么读？
       先算 *d++ = *src++：把 src 当前字符拷到 d 当前位置，然后两个指针都往后走。
       赋值表达式的值是「被赋的那个字符」。
       所以整个循环的意思是：一边拷一边前进，直到拷到 '\0' 才停（'\0' 也拷进去了）。
       这行经典 C 代码在嵌入式启动文件里到处都是，必须能读懂。

  2. my_strcmp 为什么返回 (unsigned char)*a - (unsigned char)*b ？
       直接写 *a - *b 的话，如果 char 是有符号的（x86 默认），
       遇到 0x80 以上的字节会算出负数，排序结果就错了。
       强制转成 unsigned char 才是标准 strcmp 的行为。

  3. my_itoa 里 INT_MIN 为什么不能直接取负？
       INT_MIN 是 -2147483648，它的绝对值 2147483648 超过了 int 的上限，
       写 -value 就是有符号溢出 = UB。
       标准做法：先 value + 1 变成 -2147483647（取负安全），
       再用 unsigned 加回 1，得到 2147483648 这个无符号值。

  4. memset(arr, 1, sizeof arr) 为什么不能把 int 数组填成 1 ？
       memset 的单位是字节。每个字节写 0x01，一个 int 占 4 字节，
       拼起来就是 0x01010101 = 16843009。
       想给 int 数组赋初值只能用循环，或者用 C 的初始化列表。

  5. 为什么 my_memcpy 能拷贝任何类型？
       参数是 void*，先转成 unsigned char* 再按字节走。
       unsigned char 是标准保证「没有陷阱表示、可以逐字节访问任何对象」的类型。
       用 char* 在某些平台上会碰到符号问题，所以一律用 unsigned char*。

   === 嵌入式关联 ===
   你在 ex08 写的位运算、在 ex10 写的寄存器访问，底层其实就是这些 memcpy 风格的
   字节操作。STM32 的 HAL_UART_Transmit 里那一行行 for 循环，和这里一模一样。
 * ========================================================================== */
