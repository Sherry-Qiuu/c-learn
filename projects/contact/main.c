/* ============================================================================
 * main.c —— 通讯录项目的测试驱动
 * ----------------------------------------------------------------------------
 * 这个文件不用你改。它的作用：
 *   1. 走一遍全部接口，用 assert 验证行为
 *   2. 演示「一个模块怎么被另一个模块使用」——它只 include contact.h，
 *      完全不知道 contact.c 内部是怎么实现的
 *
 *   make rebuild && make run
 * ========================================================================== */

#include "contact.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

/* ---------------------------------------------------------------------------
 * 这一步是「分离编译」最直观的证明：
 *   main.c 里调用的 contact_add / contact_find / ... 全都没有定义在这里，
 *   编译 main.c 的时候它们只是「声明」，编译器生成的是一堆
 *   「跳转到符号 contact_add」的指令。
 *   真正把这些符号填上地址的，是最后一步链接。
 *
 * 你可以自己验证：
 *   gcc -c contact.c -o contact.o        # 只编译，能过
 *   gcc -c main.c -o main.o              # 只编译，能过
 *   nm contact.o | grep contact_add      # 看到 T contact_add（已定义）
 *   nm main.o    | grep contact_add      # 看到 U contact_add（未定义，需要外部提供）
 *   gcc contact.o main.o -o app.exe      # 链接，把 U 和 T 对上
 * ------------------------------------------------------------------------- */

static void test_empty_book(void)
{
    printf("\n========== 1. 空通讯录 ==========\n");
    ContactBook book;
    contact_book_init(&book);

    assert(contact_count(&book) == 0);
    assert(contact_at(&book, 0) == NULL);
    printf("  count = %zu，at(0) = NULL  OK\n", contact_count(&book));

    Contact out;
    assert(contact_find(&book, "张三", &out) == CONTACT_ERR_NOT_FOUND);
    assert(contact_remove(&book, "张三") == CONTACT_ERR_NOT_FOUND);
    printf("  空表查找 / 删除都返回 NOT_FOUND  OK\n");

    contact_dump(&book);
}

static void test_add_and_find(void)
{
    printf("\n========== 2. 添加与查找 ==========\n");
    ContactBook book;
    contact_book_init(&book);

    assert(contact_add(&book, "张三", "13800138000", 20) == CONTACT_OK);
    assert(contact_add(&book, "李四", "13900139000", 22) == CONTACT_OK);
    assert(contact_add(&book, "王五", "13700137000", 25) == CONTACT_OK);
    assert(contact_count(&book) == 3);
    printf("  添加 3 条  OK\n");

    /* 重名必须被拒 */
    assert(contact_add(&book, "张三", "11111111111", 99) == CONTACT_ERR_DUPLICATE);
    assert(contact_count(&book) == 3);
    printf("  重名被拒绝，count 仍是 3  OK\n");

    /* 非法参数 */
    assert(contact_add(&book, NULL, "123", 1) == CONTACT_ERR_INVALID);
    assert(contact_add(&book, "张三", NULL, 1) == CONTACT_ERR_INVALID);
    assert(contact_add(&book, "", "123", 1) == CONTACT_ERR_INVALID);
    assert(contact_add(NULL, "a", "b", 1) == CONTACT_ERR_INVALID);
    printf("  空指针 / 空名字被拒绝  OK\n");

    /* 查找 */
    Contact out;
    assert(contact_find(&book, "李四", &out) == CONTACT_OK);
    assert(strcmp(out.name, "李四") == 0);
    assert(strcmp(out.phone, "13900139000") == 0);
    assert(out.age == 22);
    printf("  查到李四：%s / %s / %u  OK\n", out.name, out.phone, out.age);

    assert(contact_find(&book, "赵六", &out) == CONTACT_ERR_NOT_FOUND);
    printf("  查不到的返回 NOT_FOUND  OK\n");

    /* 按下标访问 */
    const Contact *p = contact_at(&book, 1);
    assert(p != NULL && strcmp(p->name, "李四") == 0);
    assert(contact_at(&book, 3) == NULL);
    printf("  at(1) = 李四，at(3) = NULL（越界保护）  OK\n");

    contact_dump(&book);
}

static void test_full(void)
{
    printf("\n========== 3. 容量上限 ==========\n");
    ContactBook book;
    contact_book_init(&book);

    char name[32];
    for (unsigned i = 0; i < CONTACT_BOOK_MAX; i++) {
        snprintf(name, sizeof name, "user%u", i);
        assert(contact_add(&book, name, "10000", i) == CONTACT_OK);
    }
    assert(contact_count(&book) == CONTACT_BOOK_MAX);
    printf("  填满 %d 条  OK\n", CONTACT_BOOK_MAX);

    assert(contact_add(&book, "overflow", "1", 0) == CONTACT_ERR_FULL);
    assert(contact_count(&book) == CONTACT_BOOK_MAX);
    printf("  再添加返回 FULL，count 没变  OK\n");

    /* 删掉一条之后应该能再塞一条进去 */
    assert(contact_remove(&book, "user3") == CONTACT_OK);
    assert(contact_add(&book, "userNew", "2", 0) == CONTACT_OK);
    printf("  删除一条后可以再添加  OK\n");
}

static void test_remove(void)
{
    printf("\n========== 4. 删除与前移 ==========\n");
    ContactBook book;
    contact_book_init(&book);

    contact_add(&book, "A", "1", 1);
    contact_add(&book, "B", "2", 2);
    contact_add(&book, "C", "3", 3);
    contact_add(&book, "D", "4", 4);

    /* 删中间一条：后面的必须前移，不能留空洞 */
    assert(contact_remove(&book, "B") == CONTACT_OK);
    assert(contact_count(&book) == 3);
    assert(strcmp(contact_at(&book, 0)->name, "A") == 0);
    assert(strcmp(contact_at(&book, 1)->name, "C") == 0);
    assert(strcmp(contact_at(&book, 2)->name, "D") == 0);
    printf("  删除中间的 B，C / D 正确前移  OK\n");
    contact_dump(&book);

    /* 删第一条 */
    assert(contact_remove(&book, "A") == CONTACT_OK);
    assert(strcmp(contact_at(&book, 0)->name, "C") == 0);
    printf("  删除首条  OK\n");

    /* 删最后一条 */
    assert(contact_remove(&book, "D") == CONTACT_OK);
    assert(contact_count(&book) == 1);
    printf("  删除尾条  OK\n");

    assert(contact_remove(&book, "C") == CONTACT_OK);
    assert(contact_count(&book) == 0);
    printf("  删空  OK\n");

    /* 删空之后还能正常用 */
    assert(contact_add(&book, "新", "9", 9) == CONTACT_OK);
    assert(contact_count(&book) == 1);
    printf("  删空后重新添加  OK\n");
}

static void test_serialize(void)
{
    printf("\n========== 5. 序列化（不含填充垃圾）==========\n");
    ContactBook book;
    contact_book_init(&book);

    contact_add(&book, "Alice", "1111", 30);
    contact_add(&book, "Bob",   "2222", 40);

    unsigned char buf[512];
    size_t n = contact_serialize(&book, buf, sizeof buf);

    const size_t per = CONTACT_NAME_MAX + CONTACT_PHONE_MAX + 4;
    assert(n == 2 * per);
    printf("  序列化 %zu 字节（每条 %zu 字节）  OK\n", n, per);

    /* 验证第一条的名字部分 */
    assert(memcmp(buf, "Alice", 5) == 0);
    printf("  偏移 0 开始是 \"Alice\"  OK\n");

    /* 验证第一条的 age 是小端 30 */
    size_t age_off = CONTACT_NAME_MAX + CONTACT_PHONE_MAX;
    unsigned age = (unsigned)buf[age_off]
                 | ((unsigned)buf[age_off + 1] << 8)
                 | ((unsigned)buf[age_off + 2] << 16)
                 | ((unsigned)buf[age_off + 3] << 24);
    assert(age == 30);
    printf("  age 按小端解出 = %u  OK\n", age);

    /* 缓冲区不足必须拒绝，且不写坏任何东西 */
    unsigned char small[4] = { 0xAA, 0xAA, 0xAA, 0xAA };
    assert(contact_serialize(&book, small, sizeof small) == 0);
    assert(small[0] == 0xAA);
    printf("  缓冲区不足时返回 0 且不写入  OK\n");

    /* 关键验证：序列化结果里不应该出现「结构体填充」导致的长度膨胀 */
    printf("\n  对照一下：\n");
    printf("    sizeof(Contact)          = %zu 字节\n", sizeof(Contact));
    printf("    序列化后的每条长度        = %zu 字节\n", per);
    printf("    两者恰好相等 —— 因为这个结构体的成员顺序刚好没有产生填充。\n");

    /* 但只要随手加一个 unsigned char 字段，填充就出现了 */
    typedef struct {
        char          name[CONTACT_NAME_MAX];
        char          phone[CONTACT_PHONE_MAX];
        unsigned      age;
        unsigned char flag;
    } ContactV2;

    printf("\n    加一个 unsigned char flag 之后：\n");
    printf("      字段字节总和            = %zu 字节\n", per + 1);
    printf("      sizeof(ContactV2)       = %zu 字节   <- 多出来的 %zu 字节是填充垃圾\n",
           sizeof(ContactV2), sizeof(ContactV2) - (per + 1));
    assert(sizeof(ContactV2) > per + 1);
    printf("      这 %zu 个填充字节里是栈上的随机内容。\n",
           sizeof(ContactV2) - (per + 1));
    printf("      如果直接 fwrite 整个结构体，这些垃圾会一起被写到文件/网络里。\n");
}

static void test_aliasing_bug(void)
{
    printf("\n========== 6. 一个「看着对但其实错」的写法 ==========\n");
    printf("  有人会把 contact_find 写成：\n");
    printf("      return &book->items[i];       // 返回内部指针\n\n");
    printf("  为什么不好？\n");
    printf("    1. 调用方拿到指针后，如果又调了 contact_remove，\n");
    printf("       那个指针可能已经指向被挪动的别的记录了 —— 语义错乱。\n");
    printf("    2. 要返回指针就得放弃 const 语义或者暴露内部布局，\n");
    printf("       以后你想把 items 换成链表都换不了。\n");
    printf("  所以对外接口用「拷贝 + 状态码」，对内才用指针。\n");
    printf("  这条原则在嵌入式里同样适用：模块边界上不要暴露内部指针。\n");
}

int main(void)
{
    printf("================================================\n");
    printf("  通讯录项目 · 测试驱动\n");
    printf("  （任何一个 assert 失败都会立刻终止并报行号）\n");
    printf("================================================\n");

    test_empty_book();
    test_add_and_find();
    test_full();
    test_remove();
    test_serialize();
    test_aliasing_bug();

    printf("\n================================================\n");
    printf("  全部测试通过。你完成了第一个多文件 C 项目。\n");
    printf("================================================\n");
    return 0;
}
