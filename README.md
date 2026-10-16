# C 语言 2 天冲刺 · 面向 C++ 背景 · 嵌入式（STM32）方向

你的环境**已经全部就绪，本包不需要安装任何东西**：

| 工具 | 位置 / 版本 |
|---|---|
| gcc | `C:\Users\lenovo\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\gcc.exe`（16.1.0） |
| g++ / gdb / cmake / ninja / mingw32-make | 同上目录 |
| VS Code | 1.141.0，已装 `ms-vscode.cpptools` 扩展包 |

---

## 目录结构

```
D:\c-learn\
├── README.md              本文件：2 天日程表
├── Makefile               一键编译 / 运行 / 看汇编
├── exercises\             12 个练习，每个都带自测断言
├── projects\
│   ├── contact\           项目 1：多文件通讯录（练工程化）
│   └── gpio_sim\          项目 2：模拟 GPIO + 中断回调（STM32 味儿）
├── notes\
│   ├── cheatsheet_c_for_cpp.md   C++→C 差异速查 + 指针/内存图
│   └── stm32_prep.md             从 PC 的 C 到 STM32 的 C，差在哪
├── build\                 编译产物（可随时整个删掉）
└── .vscode\               编译 / 调试任务，已配好复用现成 gcc
```

---

## 怎么用

在 **Git Bash** 里：

```bash
cd /d/c-learn

make list                # 列出所有练习
make run/ex01            # 编译并运行 ex01（做完一个跑一个）
make all-ex              # 编译全部练习（只编译，不运行）
make asm/ex02            # 生成汇编，看编译器怎么理解你的指针
make clean               # 清空 build/，不删源码
```

不想用 make，直接一条命令也行：

```bash
gcc -std=c17 -Wall -Wextra -Wpedantic -g -O0 exercises/ex01_toolchain.c -o build/ex01.exe && ./build/ex01.exe
```

VS Code 里：打开 `D:\c-learn` 文件夹，按 `Ctrl+Shift+B` 编译当前文件，按 `F5` 用 gdb 单步调试。

---

## 2 天日程（按 19 小时/天 排）

### Day 1 — 把 C 的内存模型刻进肌肉

| 时段 | 时长 | 内容 | 对应文件 |
|---|---|---|---|
| ① | 2h | 读 `notes/cheatsheet_c_for_cpp.md`，建立"C 少了什么"的清单 | — |
| ② | 2h | 编译链路、`stdint.h` 定长类型、`printf` 格式符 | `ex01` |
| ③ | 3h | 指针基础：取地址、解引用、交换、指针运算与步长 | `ex02` |
| ④ | 3h | 数组退化、`sizeof` 陷阱、二维数组与行指针 | `ex03` |
| ⑤ | 2h | `const` 四种位置、指针数组 vs 数组指针、多级指针 | `ex04` |
| ⑥ | 4h | 手写 `strlen/strcpy/strcat/strcmp/atoi/itoa`——不许调库 | `ex05` |
| ⑦ | 3h | `malloc/realloc/free` + 手写动态数组 | `ex06` |

### Day 2 — 工程化 + 嵌入式前置

| 时段 | 时长 | 内容 | 对应文件 |
|---|---|---|---|
| ① | 3h | `struct/union/位域`、内存对齐、字节序 | `ex07` |
| ② | 3h | **位运算专题**：置位/清位/翻位/查位/掩码（嵌入式的命根子） | `ex08` |
| ③ | 2h | 函数指针、回调表、状态机（中断向量表的思想来源） | `ex09` |
| ④ | 2h | `volatile` + 固定地址访问模拟寄存器 | `ex10` |
| ⑤ | 1h | 文件 I/O：文本与二进制 | `ex11` |
| ⑥ | 1h | 未定义行为识别（先判断，再跑） | `ex12` |
| ⑦ | 3h | 项目 1：多文件通讯录（头文件、守卫、分离编译、Makefile） | `projects/contact` |
| ⑧ | 3h | 项目 2：模拟 GPIO 驱动 + 中断回调分发 | `projects/gpio_sim` |
| ⑨ | 1h | 读 `notes/stm32_prep.md`，把 PC 上学的 C 映射到 STM32 | — |

---

## 规矩（很重要）

1. **每个练习末尾有自测断言，全部通过才算做完。** 断言失败会打印 `Assertion failed` 和行号，照着行号看。
2. **卡住先自己想 20 分钟**，再去翻文件末尾 `参考答案` 注释块。
3. 每写完一个练习，用 `gcc -Wall -Wextra -Wpedantic` 过一遍，**警告一个都别放过**——这些警告在单片机上是真会死人的。
4. 所有代码用 `-std=c17`，这是现代 C 的基线。
5. 没实现 TODO 之前，会有一堆 `unused parameter` 警告。**这是故意的信号**，别用 `(void)x` 把它按下去。

---

## 常见问题

**Q：刚 gcc 编译完立刻运行，报 `Permission denied`？**
Windows Defender 的实时扫描会短暂锁住新生成的 .exe。等一两秒再运行一次就好。
不影响正确性，只是时序问题。

**Q：`make` 找不到？**
`mingw32-make` 在你机器上叫这个名字（随 MinGW 一起装的），它和 Linux 的 `make` 是同一个程序。
嫌长可以自己加个别名：`alias make=mingw32-make`（写进 `~/.bashrc` 永久生效）。

**Q：编译报了 warning 但我看不懂？**
把原文贴给我，我给你解释是哪条规则被触发了。**不要**用 `-w` 把警告关掉。

**Q：中文在控制台里乱码？**
Git Bash 默认 UTF-8，一般没问题。如果用的是 cmd.exe，先执行 `chcp 65001`。

**Q：`make san/exNN` 报错说找不到 -fsanitize=undefined？**
MinGW 对 Sanitizer 支持不完整。这个目标在 MSVC/WSL/Linux 上前提更充分。
在 Windows 上抓 UB，优先靠 `-Wall -Wextra -Wpedantic` + `-fanalyzer`。

**Q：我能不能直接看参考答案？**
可以，但先自己想 20 分钟。这些练习的价值在「卡住的过程」，不在「看完答案」。

---

## 两个项目（Day 2 后半段）

做完 ex01–ex12 之后进项目。每个项目都有自己的 README：

- `projects/contact/README.md` —— 多文件通讯录，学分离编译、头文件守卫、Makefile
- `projects/gpio_sim/README.md` —— 模拟 GPIO 驱动 + 中断，STM32 结构预演

两个项目的**参考答案都已经过编译+运行验证**，你写完之后如果 `make run` 卡住，
可以对照 `contact.c` / `gpio.c` / `irq.c` 文件末尾的答案块逐段比对。

---

## 之后（先不要动手，等你 C 打完基础再说）

STM32 阶段需要的东西，到时候我会先报体积和位置给你确认，**现在一个都不装**：

- STM32CubeIDE 约 3–5 GB，或
- 只装 `arm-none-eabi-gcc` 工具链 + VS Code 插件，约 1–2 GB，更轻
- 硬件：一块 STM32 开发板（F103C8T6 最小系统板最便宜最好上手）
