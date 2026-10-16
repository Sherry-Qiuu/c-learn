/* ============================================================================
 * contact.h —— 通讯录的「接口」
 * ----------------------------------------------------------------------------
 * 重要观念：头文件是「合同」，不是「实现」。
 *   头文件里放：类型定义、宏、函数的声明、外部变量的 extern 声明
 *   头文件里绝不放：函数的定义、变量的定义（会导致多文件重复定义）
 *
 * 嵌入式项目里，这个「接口先行」的习惯尤其重要 ——
 * 因为硬件接口定下来之后很难改，先把 .h 写清楚能省掉大量返工。
 *
 * ⚠️ 这个文件不需要你修改。先读懂它，再去实现 contact.c。
 * ========================================================================== */

#ifndef CONTACT_H            /* 头文件守卫：防止同一个 .c 里 include 两次 */
#define CONTACT_H

#include <stddef.h>

/* ---------------------------------------------------------------------------
 * 容量配置。嵌入式里一切都有上限，所以这些必须是编译期常量，
 * 而不是「用到多少算多少」。
 * ------------------------------------------------------------------------- */
#define CONTACT_NAME_MAX   32
#define CONTACT_PHONE_MAX  16
#define CONTACT_BOOK_MAX    8

/* ---------------------------------------------------------------------------
 * 一条联系人记录。
 * 注意这里用的全是「定长数组」而不是 char* / std::string：
 *   - 定长 = 无动态分配 = 无碎片 = 行为可预测
 *   - 整个结构体可以被整齐地写进 Flash / EEPROM / 通过网络发送
 * 代价是浪费一点空间、且名字有长度上限。嵌入式里这个交换是划算的。
 * ------------------------------------------------------------------------- */
typedef struct {
    char     name[CONTACT_NAME_MAX];
    char     phone[CONTACT_PHONE_MAX];
    unsigned age;
} Contact;

/* ---------------------------------------------------------------------------
 * 整个通讯录。这就是 C 版的「容器」：
 *   一个定长数组 + 一个已用计数。
 * 绝不要用「数组末尾放哨兵值」的做法 —— 那要求某个取值永远不被使用。
 * ------------------------------------------------------------------------- */
typedef struct {
    Contact items[CONTACT_BOOK_MAX];
    size_t  count;
} ContactBook;

/* ---------------------------------------------------------------------------
 * 统一的状态码。
 *
 * 为什么不用返回指针 + NULL 表示失败？
 *   因为「失败的原因」丢失了 —— 是满了？还是重名？还是参数非法？
 *   嵌入式里错误原因必须能上报（要写进日志、要点亮故障灯），
 *   所以用枚举状态码是标准做法。
 *
 * 为什么不用负数当成功、正数当错误？
 *   因为那样容易写反。让 0 = 成功是 C 社区的共同约定，硬记。
 * ------------------------------------------------------------------------- */
typedef enum {
    CONTACT_OK             =  0,   /* 成功 */
    CONTACT_ERR_FULL       = -1,   /* 通讯录已满 */
    CONTACT_ERR_NOT_FOUND  = -2,   /* 查无此人 */
    CONTACT_ERR_INVALID    = -3,   /* 参数非法（空指针 / 空名字 / 名字太长） */
    CONTACT_ERR_DUPLICATE  = -4    /* 同名联系人已存在 */
} contact_status_t;

/* ---------------------------------------------------------------------------
 * 接口声明。看这些签名，你能读出设计者的意图：
 *   只读的用 const，要改的传指针，能失败的一律返回 contact_status_t。
 * ------------------------------------------------------------------------- */

/* 清空通讯录 */
void contact_book_init(ContactBook *book);

/* 添加联系人。名字重复返回 CONTACT_ERR_DUPLICATE；满了返回 CONTACT_ERR_FULL */
contact_status_t contact_add(ContactBook *book, const char *name,
                             const char *phone, unsigned age);

/* 按名字查找。找到就把记录拷进 *out 并返回 CONTACT_OK */
contact_status_t contact_find(const ContactBook *book, const char *name,
                              Contact *out);

/* 按名字删除。删掉之后要把后面的元素前移，保持数组紧凑 */
contact_status_t contact_remove(ContactBook *book, const char *name);

/* 当前条目数 */
size_t contact_count(const ContactBook *book);

/* 按下标访问。越界返回 NULL —— 注意这里返回的是「指针」，
 * 因为调用方可能只想读，不想拷一整份结构体。 */
const Contact *contact_at(const ContactBook *book, size_t index);

/* 把全部联系人打印出来（调试用） */
void contact_dump(const ContactBook *book);

/* 把通讯录序列化成紧凑的字节流（不含填充垃圾），返回写入的字节数；
 * 缓冲区不足返回 0。
 *
 * 为什么不能直接 fwrite(&book, sizeof book) ？
 *   因为结构体里有填充字节（回想 ex07），填充里是内存垃圾，
 *   直接写出去等于把栈上的随机内容泄漏到文件/网络里。
 *   正确做法：逐字段、逐条目地写。 */
size_t contact_serialize(const ContactBook *book, unsigned char *out, size_t outsz);

#endif /* CONTACT_H */
