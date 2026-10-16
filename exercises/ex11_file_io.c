/* ============================================================================
 * ex11_file_io.c —— 文件 I/O：文本与二进制
 * ----------------------------------------------------------------------------
 * 这一课在嵌入式里看起来用处不大（单片机哪来的文件系统），但其实是刚需：
 *   - 调试时把数据存成 CSV，用 Excel 画波形
 *   - 参数配置持久化到 Flash / SD 卡 / LittleFS
 *   - 固件升级包（.bin）的读写
 *   - 用 PC 写一个上位机，和单片机串口对接，日志落盘
 * 本质都是「把一块字节按某种格式写出去、再读回来」，和这里学的一模一样。
 *
 *   make run/ex11
 * 运行后会在 build/ 目录生成两个文件，课后自己打开看看。
 * ========================================================================== */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

/* 所有输出都放在 build/ 下，删掉 build/ 就等于清空，不污染别的地方 */
#define OUT_TEXT   "build/ex11_text.txt"
#define OUT_CSV    "build/ex11_data.csv"
#define OUT_BINARY "build/ex11_records.bin"

typedef struct {
    uint16_t id;
    uint16_t value;
    uint8_t  flags;
    uint8_t  reserved;
    uint32_t timestamp;
} Record;

_Static_assert(sizeof(Record) == 12, "Record 必须是紧凑的 12 字节，检查有没有填充");

/* ---------------------------------------------------------------------------
 * TODO 1：把若干行文本写进文件
 *   要求：每行末尾写一个 '\n'；每行写完后检查是否出错。
 *   返回：成功写入的行数；fopen 失败返回 -1。
 *
 *   注意 fopen 一定要判 NULL —— 磁盘满、路径不存在、没权限都会失败。
 * ------------------------------------------------------------------------- */
int write_text_lines(const char *path, const char *const *lines, size_t n)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ---------------------------------------------------------------------------
 * TODO 2：把整个文件读进缓冲区
 *   要求：
 *     - 读入的内容末尾必须补 '\0'（这样能当字符串用）
 *     - 最多读 bufsz - 1 个字节，绝不越界
 *     - 返回实际读到的字节数；失败返回 -1
 *   提示：用 fread 一次读，或者 fgetc 循环读。
 * ------------------------------------------------------------------------- */
long read_whole_file(const char *path, char *buf, size_t bufsz)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ---------------------------------------------------------------------------
 * TODO 3：把结构体数组以「二进制」写入文件
 *   直接 fwrite 整个数组，一个字节不多不少。
 *   返回成功写入的记录数；失败返回 -1。
 *
 *   二进制的好处：体积小、读写快、保真（浮点不丢精度）
 *   二进制的问题：字节序、结构体填充、平台字长都依赖具体机器 ——
 *                 所以跨平台传输时通常要定义「协议格式」再逐字段序列化。
 * ------------------------------------------------------------------------- */
int write_records_binary(const char *path, const Record *recs, size_t n)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ---------------------------------------------------------------------------
 * TODO 4：从二进制文件读回结构体数组
 *   最多读 max_n 条。返回实际读到的条数；失败返回 -1。
 *
 *   关键：fread 的返回值是「读到的元素个数」，一定要和你要读的个数比对，
 *         因为文件可能被截断、被别的东西写了、或者根本不是这个格式。
 * ------------------------------------------------------------------------- */
size_t read_records_binary(const char *path, Record *recs, size_t max_n)
{
    /* 在这里写你的代码 */
    return 0;
}

/* ---------------------------------------------------------------------------
 * TODO 5：把记录写成 CSV，头部第一行是字段名
 *   格式（每行一条，字段用逗号分隔）：
 *     id,value,flags,timestamp
 *     1,100,0,1000
 *     2,200,1,2000
 *   返回写入的行数（含表头）；失败返回 -1。
 *
 *   这就是「用 Excel 看单片机数据」的标准做法。
 * ------------------------------------------------------------------------- */
int write_records_csv(const char *path, const Record *recs, size_t n)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ---------------------------------------------------------------------------
 * TODO 6：返回文件大小（字节），失败返回 -1
 *   提示：fseek(f, 0, SEEK_END) 之后 ftell(f)
 * ------------------------------------------------------------------------- */
long file_size(const char *path)
{
    /* 在这里写你的代码 */
    return -1;
}

int main(void)
{
    printf("========== 任务 1：写文本文件 ==========\n");
    {
        const char *lines[] = {
            "STM32F103C8T6",
            "ARM Cortex-M3 @72MHz",
            "Flash 64KB / SRAM 20KB",
            "今天的目标：点亮 PA5 上的 LED",
        };

        int written = write_text_lines(OUT_TEXT, lines, 4);
        assert(written == 4);
        printf("  写入 %d 行到 %s  OK\n", written, OUT_TEXT);

        /* 路径不存在的情况必须被安全处理 */
        assert(write_text_lines("build/no_such_dir/x.txt", lines, 4) == -1);
        printf("  路径不存在时返回 -1 而不是崩溃  OK\n");
    }

    printf("\n========== 任务 2：读回整个文件 ==========\n");
    {
        char buf[512];
        long n = read_whole_file(OUT_TEXT, buf, sizeof buf);
        assert(n > 0);
        printf("  读到 %ld 字节：\n", n);
        printf("  ---8<---\n%s  ---8<---\n", buf);

        /* 因为我们补了 '\0'，所以可以直接当字符串用 */
        assert(strstr(buf, "Cortex-M3") != NULL);
        printf("  strstr 能直接在读到的内容里搜索  OK\n");

        /* 缓冲区太小的情况 */
        char small[8];
        long n2 = read_whole_file(OUT_TEXT, small, sizeof small);
        assert(n2 == 7);
        assert(small[7] == '\0');
        printf("  缓冲区不足时只读 %ld 字节且正确截断  OK\n", n2);

        assert(read_whole_file("build/does_not_exist.txt", buf, sizeof buf) == -1);
        printf("  文件不存在时返回 -1  OK\n");
    }

    printf("\n========== 任务 3/4：二进制读写 ==========\n");
    {
        Record recs[4] = {
            { 1, 100, 0x01, 0, 1000 },
            { 2, 200, 0x02, 0, 2000 },
            { 3, 300, 0x00, 0, 3000 },
            { 4, 400, 0xFF, 0, 4000 },
        };

        int w = write_records_binary(OUT_BINARY, recs, 4);
        assert(w == 4);
        printf("  写入 4 条记录  OK\n");

        long sz = file_size(OUT_BINARY);
        assert(sz == (long)(4 * sizeof(Record)));
        printf("  文件大小 = %ld 字节 = 4 × %zu  OK\n", sz, sizeof(Record));

        Record back[8];
        memset(back, 0, sizeof back);
        size_t r = read_records_binary(OUT_BINARY, back, 8);
        assert(r == 4);
        for (size_t i = 0; i < 4; i++) {
            assert(back[i].id == recs[i].id);
            assert(back[i].value == recs[i].value);
            assert(back[i].flags == recs[i].flags);
            assert(back[i].timestamp == recs[i].timestamp);
        }
        printf("  读回 4 条记录，逐字段比对通过  OK\n");

        /* 只读前 2 条 */
        Record partial[2];
        size_t r2 = read_records_binary(OUT_BINARY, partial, 2);
        assert(r2 == 2);
        printf("  限制最多读 2 条时只读到 2 条  OK\n");
    }

    printf("\n========== 任务 5：导出 CSV ==========\n");
    {
        Record recs[3] = {
            { 1, 100, 0x01, 0, 1000 },
            { 2, 200, 0x02, 0, 2000 },
            { 3, 300, 0x00, 0, 3000 },
        };
        int lines = write_records_csv(OUT_CSV, recs, 3);
        assert(lines == 4);   /* 3 条 + 1 行表头 */
        printf("  写入 %d 行到 %s  OK\n", lines, OUT_CSV);

        char buf[512];
        read_whole_file(OUT_CSV, buf, sizeof buf);
        printf("  ---8<---\n%s  ---8<---\n", buf);
    }

    printf("\n========== 任务 6：文件大小 ==========\n");
    {
        long a = file_size(OUT_BINARY);
        long b = file_size(OUT_TEXT);
        assert(a > 0 && b > 0);
        printf("  %s = %ld 字节\n", OUT_BINARY, a);
        printf("  %s = %ld 字节\n", OUT_TEXT, b);
        assert(file_size("build/nope.bin") == -1);
        printf("  文件不存在时返回 -1  OK\n");
    }

    printf("\n========== 任务 7：文件 I/O 的四个必须动作 ==========\n");
    {
        printf("  1. fopen 必须判 NULL\n");
        printf("     FILE *f = fopen(path, \"rb\");\n");
        printf("     if (f == NULL) { /* 处理错误 */ }\n");
        printf("     嵌入式里 fopen 失败太常见了（SD 卡没插、FAT 表损坏）。\n\n");

        printf("  2. fclose 必须调，而且每条错误分支上都不能漏\n");
        printf("     嵌入式里漏 fclose 会一直占着 FatFs 的文件句柄，\n");
        printf("     句柄耗尽之后再 fopen 就全部失败了。\n\n");

        printf("  3. fread / fwrite 的返回值必须用\n");
        printf("     fwrite 可能只写了一半（磁盘满），fread 可能读到的比要的少（文件被截断）。\n");
        printf("     永远不要假设「调用成功 = 全部完成」。\n\n");

        printf("  4. 缓存必须 fflush 或 fclose 才会真正落盘\n");
        printf("     PC 上进程正常退出会帮你清，但单片机掉电不会。\n");
        printf("     存配置到 Flash 时，写完一定要让数据真正落地再断电。\n");
    }

    printf("\n========== 任务 8：文本 vs 二进制，怎么选 ==========\n");
    {
        printf("  ┌────────────┬──────────────────┬──────────────────────┐\n");
        printf("  │            │ 文本 (CSV/JSON)   │ 二进制 (.bin)         │\n");
        printf("  ├────────────┼──────────────────┼──────────────────────┤\n");
        printf("  │ 人可读      │ 是                │ 否                    │\n");
        printf("  │ 体积        │ 大（3~10 倍）     │ 小，紧凑              │\n");
        printf("  │ 精度        │ 浮点要格式化，会丢 │ 完整保留              │\n");
        printf("  │ 跨平台      │ 好                │ 差（字节序/对齐）     │\n");
        printf("  │ 调试友好    │ 极好              │ 要写解析工具          │\n");
        printf("  └────────────┴──────────────────┴──────────────────────┘\n\n");
        printf("  经验法则：\n");
        printf("    - 调试、配置、日志、给 Excel 看  -> 文本\n");
        printf("    - 固件、大量采样数据、Flash 存储 -> 二进制\n");
        printf("    - 跨设备通信：永远不要直接 fwrite 结构体，\n");
        printf("      要按「逐字段 + 明确字节序」的方式序列化。\n");
    }

    printf("\n>>> ex11 全部通过。下一站：ex12_ub_traps.c（最后一课）\n");
    return 0;
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

int write_text_lines(const char *path, const char *const *lines, size_t n)
{
    if (path == NULL || lines == NULL) return -1;

    FILE *f = fopen(path, "w");
    if (f == NULL) return -1;

    int written = 0;
    for (size_t i = 0; i < n; i++) {
        if (fputs(lines[i], f) == EOF) break;
        if (fputc('\n', f) == EOF)     break;
        written++;
    }

    if (fclose(f) != 0) return -1;   // fclose 的返回值也不能忽略
    return written;
}

long read_whole_file(const char *path, char *buf, size_t bufsz)
{
    if (path == NULL || buf == NULL || bufsz == 0) return -1;

    FILE *f = fopen(path, "rb");
    if (f == NULL) return -1;

    size_t got = fread(buf, 1, bufsz - 1, f);
    buf[got] = '\0';                 // 补 '\0'，让它能当字符串用
    fclose(f);
    return (long)got;
}

int write_records_binary(const char *path, const Record *recs, size_t n)
{
    if (path == NULL || recs == NULL) return -1;

    FILE *f = fopen(path, "wb");
    if (f == NULL) return -1;

    size_t w = fwrite(recs, sizeof(Record), n, f);
    fclose(f);

    if (w != n) return -1;           // 写了一半也算失败
    return (int)w;
}

size_t read_records_binary(const char *path, Record *recs, size_t max_n)
{
    if (path == NULL || recs == NULL || max_n == 0) return 0;

    FILE *f = fopen(path, "rb");
    if (f == NULL) return 0;

    size_t r = fread(recs, sizeof(Record), max_n, f);
    fclose(f);
    return r;
}

int write_records_csv(const char *path, const Record *recs, size_t n)
{
    if (path == NULL || recs == NULL) return -1;

    FILE *f = fopen(path, "w");
    if (f == NULL) return -1;

    int lines = 0;
    if (fprintf(f, "id,value,flags,timestamp\n") < 0) { fclose(f); return -1; }
    lines++;

    for (size_t i = 0; i < n; i++) {
        int k = fprintf(f, "%u,%u,%u,%u\n",
                        (unsigned)recs[i].id,
                        (unsigned)recs[i].value,
                        (unsigned)recs[i].flags,
                        (unsigned)recs[i].timestamp);
        if (k < 0) { fclose(f); return -1; }
        lines++;
    }

    if (fclose(f) != 0) return -1;
    return lines;
}

long file_size(const char *path)
{
    if (path == NULL) return -1;

    FILE *f = fopen(path, "rb");
    if (f == NULL) return -1;

    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return -1; }
    long sz = ftell(f);
    fclose(f);
    return sz;
}

  --- 三个必须说清楚的点 ---

  1. _Static_assert(sizeof(Record) == 12, ...) 是干什么的？

       它在「编译期」检查结构体大小。我在 ex07 讲过：编译器会为了对齐插入填充字节。
       如果 Record 实际是 16 字节，这个断言会在编译时直接报错，
       而不是等到你写完固件烧进板子、发现协议对不上才排查。

       这是 C11 的特性（C++ 里是 static_assert）。
       嵌入式项目里凡是「要和外部世界交换字节」的结构体，都该加这个断言。
       Record 里我特意把 reserved 补上，就是为了让它正好 12 字节、没有填充。

  2. 为什么 read 时要传 bufsz 并且只能用 bufsz - 1 个字节？

       因为要留一个位置给 '\0'。
       如果 fread(buf, 1, bufsz, f) 把缓冲区填满，那 buf[got] = '\0' 就写到界外了。
       这个 off-by-one 是 C 里最经典的错误，从 strcpy 到 fread 都是同一个坑。
       记住公式：要当字符串用，可用容量永远是「缓冲区大小 - 1」。

  3. fclose 的返回值为什么也要检查？

       因为数据是先写到用户态缓冲区、fclose 时才真正落盘的。
       如果磁盘满或 SD 卡被拔了，错误会在 fclose 时才暴露。
       嵌入式里「日志写完了但文件是空的」几乎都是这个原因。
       同理，需要强一致的时候要在 fclose 之前调 fflush(f)。
 * ========================================================================== */
