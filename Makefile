CC      := gcc
CFLAGS  := -std=c17 -Wall -Wextra -Wpedantic -g -O0 -fno-omit-frame-pointer
BUILD   := build

EX      := $(sort $(wildcard exercises/ex*.c))

.PHONY: help list all-ex clean asm san
.DEFAULT_GOAL := help

help:
	@echo ""
	@echo "  make list          列出所有练习"
	@echo "  make run/ex01      编译并运行 exercises/ex01_*.c"
	@echo "  make asm/ex01      生成 build/ex01.s，看汇编"
	@echo "  make san/ex01      带 UBSan 运行（查未定义行为）"
	@echo "  make all-ex        编译全部练习（不运行）"
	@echo "  make clean         清空 build/，源码不动"
	@echo ""

list:
	@echo "可用练习："
	@for f in $(EX); do echo "  $$(basename $$f .c)"; done

$(BUILD):
	@mkdir -p $(BUILD)

all-ex: $(BUILD)
	@for f in $(EX); do \
	  n=$$(basename $$f .c); \
	  printf "%-28s " "$$n"; \
	  if $(CC) $(CFLAGS) $$f -o $(BUILD)/$$n.exe 2>$(BUILD)/$$n.log; then \
	    echo "OK"; \
	  else \
	    echo "FAILED  (看 $(BUILD)/$$n.log)"; \
	    cat $(BUILD)/$$n.log; \
	  fi; \
	done

run/%: exercises/%.c | $(BUILD)
	@$(CC) $(CFLAGS) $< -o $(BUILD)/$*.exe && echo "--- 运行 $* ---" && ./$(BUILD)/$*.exe

san/%: exercises/%.c | $(BUILD)
	@$(CC) $(CFLAGS) -fsanitize=undefined $< -o $(BUILD)/$*_san.exe && echo "--- UBSan 运行 $* ---" && ./$(BUILD)/$*_san.exe

asm/%: exercises/%.c | $(BUILD)
	@$(CC) $(CFLAGS) -S $< -o $(BUILD)/$*.s && echo "已生成 $(BUILD)/$*.s（用 VS Code 打开看）"

clean:
	@rm -rf $(BUILD)
	@mkdir -p $(BUILD)
	@echo "build/ 已清空"
