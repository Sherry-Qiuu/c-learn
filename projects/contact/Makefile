# =============================================================================
# 通讯录项目 · Makefile
# -----------------------------------------------------------------------------
# 这是你第一次用「分离编译」：
#   每个 .c 单独编译成 .o，最后链接成可执行文件。
#
# 为什么要这么麻烦？因为改一个 .c 只需要重编那一个文件。
# 项目大了以后（比如 STM32 工程有几百个文件），全量重编要几分钟，
# 增量编译只要几秒 —— 这就是 Make 存在的意义。
#
# 常用命令：
#   make           等价于 make all
#   make run       编译并运行
#   make clean     清掉中间产物
#   make rebuild   clean + all（改了 Makefile 之后用）
#   make symbols   看看目标文件里的符号（T = 已定义，U = 需要外部提供）
# =============================================================================

CC      := gcc
CFLAGS  := -std=c17 -Wall -Wextra -Wpedantic -g -O0 -fno-omit-frame-pointer
TARGET  := app.exe

# 自动收集当前目录所有 .c，加新文件不用改 Makefile
SRCS    := $(wildcard *.c)
OBJS    := $(SRCS:.c=.o)
DEPS    := $(OBJS:.o=.d)

.PHONY: all clean run rebuild symbols help
.DEFAULT_GOAL := help

help:
	@echo ""
	@echo "  make / make all     编译链接 -> $(TARGET)"
	@echo "  make run            编译并运行"
	@echo "  make rebuild        clean + all"
	@echo "  make symbols        查看符号表（理解链接）"
	@echo "  make clean          删除 .o .exe .d"
	@echo ""
	@echo "当前源文件：$(SRCS)"
	@echo ""

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "  [LINK] $^ -> $@"
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "  完成：$@"

# -MMD -MP 会自动生成 .d 依赖文件，头文件改了也能触发重编
%.o: %.c
	@echo "  [CC]   $< -> $@"
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

run: $(TARGET)
	@echo ""
	@./$(TARGET)

symbols: $(OBJS)
	@echo "=== contact.o 里已定义的函数（T）==="
	@nm contact.o | grep -E " T " || true
	@echo ""
	@echo "=== main.o 里需要外界提供的符号（U）==="
	@nm main.o | grep -E " U " | grep -v "^.* U _" || true
	@echo ""
	@echo "T = 本文件定义；U = 本文件用到但没定义，靠链接器去别的 .o 里找"

rebuild: clean all

clean:
	@rm -f $(OBJS) $(DEPS) $(TARGET)
	@echo "  已清理中间产物"
