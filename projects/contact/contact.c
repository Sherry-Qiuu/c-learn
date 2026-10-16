/* ============================================================================
 * contact.c —— 通讯录的实现
 * ----------------------------------------------------------------------------
 * 规则：
 *   1. 只 include 自己的头文件 + 标准库，不 include main.c
 *   2. 所有只在本文件用到的辅助函数都加 static
 *   3. 每个函数进来先做「参数体检」，坏参数一律返回 CONTACT_ERR_INVALID
 *   4. 字符串操作用 snprintf，不用 strcpy —— 一旦名字超长就出事了
 *
 *   make rebuild && make run
 * ========================================================================== */

#include "contact.h"

#include <stdio.h>
#include <string.h>

/* ---------------------------------------------------------------------------
 * 私有辅助函数。static = 只在本文件可见。
 *
 * 为什么要有 static？三个理由：
 *   1. 不会和其他文件的同名函数冲突（项目大了这个很重要）
 *   2. 链接器能发现「定义了但没用到」的死代码
 *   3. 读代码的人一眼知道「这个函数不是对外接口」
 * ------------------------------------------------------------------------- */

/* TODO 0（这个已经给你了，照抄即可）：安全的字符串比较，null 安全 */
static int str_eq(const char *a, const char *b)
{
    if (a == NULL || b == NULL) return 0;
    return strcmp(a, b) == 0;
}

/* 把 src 拷进 dst（dst 大小固定为 dstsz），超长直接截断并保证 '\0' 结尾。
 * 返回 0 成功，-1 表示参数非法。
 * TODO 0b：自己实现（提示：snprintf 的返回值 >= dstsz 就说明被截断了） */
static int str_copy_trunc(char *dst, size_t dstsz, const char *src)
{
    /* 在这里写你的代码 */
    return -1;
}

/* ===========================================================================
 * 公开接口
 * =========================================================================== */

void contact_book_init(ContactBook *book)
{
    /* TODO 1：把 book 清空
     *   要求：count 归零；items 里的内容也要清掉（想想为什么 —— 提示：序列化）*/
    /* 在这里写你的代码 */
}

contact_status_t contact_add(ContactBook *book, const char *name,
                             const char *phone, unsigned age)
{
    /* TODO 2：
     *   1. 参数体检：book / name / phone 为空 -> CONTACT_ERR_INVALID
     *      名字为空字符串也算非法
     *   2. 满了 -> CONTACT_ERR_FULL
     *   3. 重名 -> CONTACT_ERR_DUPLICATE（用 str_eq 遍历已有条目）
     *   4. 写入 items[count]，用 str_copy_trunc 保证不溢出
     *   5. count++，返回 CONTACT_OK
     */
    /* 在这里写你的代码 */
    return CONTACT_ERR_INVALID;
}

contact_status_t contact_find(const ContactBook *book, const char *name,
                              Contact *out)
{
    /* TODO 3：
     *   1. 参数体检：book / name / out 为空 -> CONTACT_ERR_INVALID
     *   2. 遍历找到同名条目 -> *out = book->items[i]; return CONTACT_OK
     *   3. 找不到 -> CONTACT_ERR_NOT_FOUND
     *
     *   注意：这里用的是 const ContactBook *，你不能改它。
     *         照着 ex04 学过的 const 规则，别想着强转掉它。
     */
    /* 在这里写你的代码 */
    return CONTACT_ERR_NOT_FOUND;
}

contact_status_t contact_remove(ContactBook *book, const char *name)
{
    /* TODO 4：
     *   1. 参数体检
     *   2. 找到下标 i
     *   3. 把 i+1 到 count-1 的元素整体前移一格（用循环赋值，别用 memcpy 的重叠区域）
     *   4. count--
     *   5. 把最后那个「已经不用的」槽位清空（和 TODO 1 同样的理由）
     *   6. 返回 CONTACT_OK
     */
    /* 在这里写你的代码 */
    return CONTACT_ERR_NOT_FOUND;
}

size_t contact_count(const ContactBook *book)
{
    /* TODO 5 */
    /* 在这里写你的代码 */
    return 0;
}

const Contact *contact_at(const ContactBook *book, size_t index)
{
    /* TODO 6：越界返回 NULL
     *   注意返回类型是 const Contact *，调用方拿到后不能改内容。 */
    /* 在这里写你的代码 */
    return NULL;
}

void contact_dump(const ContactBook *book)
{
    /* TODO 7：打印成下面这样（没有条目时打印 (empty)）
     *   --- 通讯录 共 3 条 ---
     *   [0] 张三            13800138000     20
     *   [1] 李四            13900139000     22
     *   ...
     *   提示：printf 里可以用 %-16s 这种「左对齐、最小宽度 16」的格式，
     *         中文在 Windows 控制台按字节算宽度，对齐不会完美 —— 别纠结这个。
     */
    /* 在这里写你的代码 */
}

size_t contact_serialize(const ContactBook *book, unsigned char *out, size_t outsz)
{
    /* TODO 8（最难的一题）：
     *   把通讯录写成一串紧凑的字节，格式自定但必须满足：
     *     - 不含任何填充字节、不含内存垃圾
     *     - 能被反序列化回来（我们暂时不实现反序列化，但你要能说清格式）
     *     - 缓冲区不足时返回 0，且不写任何东西
     *
     *   建议格式（小端，逐字段）：
     *     每一条 = 32 字节 name（不足补 0） + 16 字节 phone + 4 字节 age(小端)
     *              = 52 字节
     *     总长度 = count * 52
     *
     *   为什么要自己拼字节而不 fwrite 结构体？
     *     因为 sizeof(Contact) 是 52+对齐填充，写出去会把栈垃圾一起送出去。
     *     这在「通过网络发数据」的场景下就是信息泄漏。
     */
    /* 在这里写你的代码 */
    return 0;
}

/* ============================================================================
 *                          参考答案（想清楚再看）
 * ============================================================================

static int str_copy_trunc(char *dst, size_t dstsz, const char *src)
{
    if (dst == NULL || dstsz == 0 || src == NULL) return -1;
    snprintf(dst, dstsz, "%s", src);
    return 0;
}

void contact_book_init(ContactBook *book)
{
    if (book == NULL) return;
    memset(book, 0, sizeof *book);
}

contact_status_t contact_add(ContactBook *book, const char *name,
                             const char *phone, unsigned age)
{
    if (book == NULL || name == NULL || phone == NULL) return CONTACT_ERR_INVALID;
    if (name[0] == '\0') return CONTACT_ERR_INVALID;
    if (book->count >= CONTACT_BOOK_MAX) return CONTACT_ERR_FULL;

    for (size_t i = 0; i < book->count; i++) {
        if (str_eq(book->items[i].name, name)) return CONTACT_ERR_DUPLICATE;
    }

    Contact *slot = &book->items[book->count];
    memset(slot, 0, sizeof *slot);
    str_copy_trunc(slot->name,  CONTACT_NAME_MAX,  name);
    str_copy_trunc(slot->phone, CONTACT_PHONE_MAX, phone);
    slot->age = age;

    book->count++;
    return CONTACT_OK;
}

contact_status_t contact_find(const ContactBook *book, const char *name,
                              Contact *out)
{
    if (book == NULL || name == NULL || out == NULL) return CONTACT_ERR_INVALID;

    for (size_t i = 0; i < book->count; i++) {
        if (str_eq(book->items[i].name, name)) {
            *out = book->items[i];
            return CONTACT_OK;
        }
    }
    return CONTACT_ERR_NOT_FOUND;
}

contact_status_t contact_remove(ContactBook *book, const char *name)
{
    if (book == NULL || name == NULL) return CONTACT_ERR_INVALID;

    size_t idx = book->count;
    for (size_t i = 0; i < book->count; i++) {
        if (str_eq(book->items[i].name, name)) { idx = i; break; }
    }
    if (idx == book->count) return CONTACT_ERR_NOT_FOUND;

    for (size_t i = idx; i + 1 < book->count; i++) {
        book->items[i] = book->items[i + 1];
    }
    book->count--;
    memset(&book->items[book->count], 0, sizeof book->items[0]);
    return CONTACT_OK;
}

size_t contact_count(const ContactBook *book)
{
    return (book == NULL) ? 0 : book->count;
}

const Contact *contact_at(const ContactBook *book, size_t index)
{
    if (book == NULL || index >= book->count) return NULL;
    return &book->items[index];
}

void contact_dump(const ContactBook *book)
{
    if (book == NULL) return;
    printf("--- 通讯录 共 %zu 条 ---\n", book->count);
    if (book->count == 0) { printf("(empty)\n"); return; }
    for (size_t i = 0; i < book->count; i++) {
        printf("[%zu] %-16s %-16s %u\n",
               i,
               book->items[i].name,
               book->items[i].phone,
               book->items[i].age);
    }
}

size_t contact_serialize(const ContactBook *book, unsigned char *out, size_t outsz)
{
    if (book == NULL || out == NULL) return 0;

    const size_t per = CONTACT_NAME_MAX + CONTACT_PHONE_MAX + 4;
    const size_t need = book->count * per;
    if (outsz < need) return 0;

    size_t k = 0;
    for (size_t i = 0; i < book->count; i++) {
        const Contact *c = &book->items[i];

        for (size_t j = 0; j < CONTACT_NAME_MAX; j++) {
            out[k++] = (unsigned char)c->name[j];
        }
        for (size_t j = 0; j < CONTACT_PHONE_MAX; j++) {
            out[k++] = (unsigned char)c->phone[j];
        }
        out[k++] = (unsigned char)( c->age        & 0xFFu);
        out[k++] = (unsigned char)((c->age >>  8) & 0xFFu);
        out[k++] = (unsigned char)((c->age >> 16) & 0xFFu);
        out[k++] = (unsigned char)((c->age >> 24) & 0xFFu);
    }
    return k;
}

  --- 为什么要 memset(slot, 0, sizeof *slot) ？ ---

  如果不清零，items[count] 里可能残留着上一次删除的记录内容。
  只覆盖 name / phone / age 三个字段是不够的 —— 定长数组里
  '\0' 之后的部分还是旧数据。
  在「序列化」的时候，这些残留就会被原样写出去。
  嵌入式里这可能导致：你以为删掉的密码 / 密钥，其实还在 Flash 里。

  这就是「删除数据时必须擦干净」这条安全规范在代码层面的样子。

  --- 为什么 remove 里不能对重叠区域用 memcpy ？ ---

  for (size_t i = idx; i + 1 < book->count; i++)
      book->items[i] = book->items[i + 1];

  这是从后往前读、往前写，源和目标是重叠的。
  如果改成 memcpy(&items[idx], &items[idx+1], (count-idx-1)*sizeof(Contact))，
  那就是 UB（回忆 ex12 第 11 题）。
  要用也必须是 memmove。

  --- 为什么 contact_at 返回指针，而 contact_find 返回拷贝 ？ ---

  contact_at：调用方明确知道下标合法，只想读，返回指针最省。
             加 const 保证他改不了。
  contact_find：调用方在「找一个东西」，找不到是常态，
             所以用状态码表示失败、用输出参数给出结果，语义更清楚。
  同一个数据结构，两种接口风格并存是完全正常的 ——
  关键是要在 .h 的注释里写清楚，让调用方知道该用哪个。
 * ========================================================================== */
