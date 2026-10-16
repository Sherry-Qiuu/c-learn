# =============================================================================
# GPIO + 中断 模拟项目 · Makefile
# -----------------------------------------------------------------------------
# 三个源文件、两个模块（gpio 和 irq 互不知道对方存在，只通过 main 协作）：
#
#   gpio.c  --include--> gpio.h
#   irq.c   --include--> irq.h
#   main.c  --include--> gpio.h + irq.h   <- 唯一知道两边接口的人
#
# 这就是嵌入式项目的分层：驱动层 / 服务层 / 应用层，互相只通过头文件耦合。
#
# 常用命令：
#   make            编译链接
#   make run        编译并运行
#   make clean      清理
#   make symbols    看符号表
#   make warnings   强制显示所有警告（默认也是全开的）
# =============================================================================

CC      := gcc
CFLAGS  := -std=c17 -Wall -Wextra -Wpedantic -g -O0 -fno-omit-frame-pointer
TARGET  := app.exe

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
	@echo "  make symbols        查看符号表"
	@echo "  make clean          删除 .o .exe .d"
	@echo ""
	@echo "当前源文件：$(SRCS)"
	@echo ""

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "  [LINK] $(notdir $^) -> $@"
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "  完成：$@"

%.o: %.c
	@echo "  [CC]   $< -> $@"
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

run: $(TARGET)
	@echo ""
	@./$(TARGET)

symbols: $(OBJS)
	@for o in $(OBJS); do \
	  echo "=== $$o ==="; \
	  nm $$o | grep -E " [TU] " | sed 's/^/  /'; \
	  echo ""; \
	done
	@echo "T = 已定义（text）；U = 未定义，需要别的 .o 提供"
	@echo "注意 gpio.o / irq.o 里那些 static 的函数和变量，在符号表里是小写 t / b"

rebuild: clean all

clean:
	@rm -f $(OBJS) $(DEPS) $(TARGET)
	@echo "  已清理中间产物"
