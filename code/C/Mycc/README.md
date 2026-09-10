# Mycc —— C语言 迷你编译器解释器

《C语言入门精通》综合案例 07 的配套源码：**五段式全链路**实现一门类 C 小语言 mycc——Lexer → Parser → TypeChecker → Codegen(36 条字节码) → 栈式 VM，约 2200 行零依赖 C11。

教程地址：`packages/website/11.CodeX/01.C语言入门精通/02.综合案例/07.迷你编译器解释器.md`

## 编译运行

```bash
make                    # 编译 mycc
./mycc                  # REPL 模式
./mycc examples/fib.mycc    # 文件模式
```

## REPL 会话

```text
mycc> :run print 1 + 2 * 3 - 4;
3
mycc> :run var i = 0; while (i < 5) { print i; i = i + 1; }
0
1
2
3
4
mycc> :dump if (1 == 1) { print 1; } else { print 0; }
== <top> ==
0000     1  CONST                 0  ; 1
0003     |  CONST                 0  ; 1
0006     |  EQ
0007     |  POP_JUMP_IF_FALSE     7
0010     |  CONST                 0  ; 1
0013     |  PRINT
0014     |  JUMP                  4
0017     |  CONST                 1  ; 0
0020     |  PRINT
0021     |  HALT
mycc> :tcheck if (1) { print 1; }
[Type] line 1: if condition needs Bool
mycc> :q
```

## 文件模式与退出码（Unix 惯例）

```text
$ ./mycc examples/hello.mycc     → Hello, mycc! / Hello, world!   exit 0
$ ./mycc examples/fib.mycc       → 0 1 1 2 3 5 8 13 21 34        exit 0
$ ./mycc examples/bad.mycc       → [Parse] bad.mycc:line 2: ...   exit 2
$ ./mycc examples/divzero.mycc   → [Runtime] ... divide by zero   exit 4
```

退出码：0=成功 / 2=词法/语法 / 3=类型 / 4=运行时 / 1=IO。

## 五段式架构

| 文件 | 教程阶段 | 职责 |
|---|---|---|
| `token.h/.c` + `lexer.h/.c` | ② §03 | 状态机词法分析，26 类 tagged union Token |
| `ast.h/.c` | ③ §04 | 12 种节点的 tagged union AST + 树形释放 |
| `parser.h/.c` | ④ §05 | 递归下降 + 优先级爬升（左结合 = while + 高优先级层）|
| `type_check.h/.c` | ⑤ §06 | switch 分派 + 作用域栈 + 函数预注册（支持相互递归）|
| `opcode.h/.c` + `chunk.h/.c` | ⑥ §07 | 36 条指令 + 字节码容器（常量池/行号表/回填三连）|
| `codegen.h/.c` | ⑥ §07 | AST→字节码；短路 &&/\|\|、跳转回填、函数=独立 Chunk |
| `value.h/.c` + `vm.h/.c` | ⑦ §08 | fetch-decode-execute 主循环 + CallFrame 调用栈 |
| `errors.h/.c` | ⑧ §09 | 四级错误码 + setjmp/longjmp 跳板 + 栈回溯 |
| `main.c` | ⑧ §09 | REPL/文件模式 + 顶层收口 + 清理锚点 |

## 与教程原代码的差异（修复的 3 处 bug）

1. **`gen_call` 压栈指令错误**：原版用 `OP_CONST` 压函数名字符串，而 `OP_CALL` 期望 `VAL_FN`——调用任何函数都会报 "can only call functions"。修复：改用 `OP_LOAD_GLOBAL`（VM 侧已有函数表查找逻辑）。
2. **全局 `var` 声明栈不平衡**：`STORE_GLOBAL` 是 peek 语义（配合赋值的 DUP），原版 `gen_var_decl` 未补弹，每次顶层声明泄漏一个栈槽。修复：补 `OP_POP`。
3. **`setjmp` 后资源泄漏**：错误经 longjmp 跳出会跳过正常释放。修复：main 引入静态清理锚点，catch 分支按推进程度统一回收（REPL 多轮执行零累积泄漏）。

另实现教程留作练习的**字符串拼接**（`"Hello, " + name`）：类型检查允许 `STR+STR`，`OP_ADD` 运行时分支拼接——`examples/hello.mycc` 因此可直接运行。

## 已知局限（教程 §12 挑战题）

- 不支持 `for` 循环（挑战一：脱糖成 while）
- 不支持数组（挑战二：VAL_ARRAY + 引用计数）
- 不支持闭包（挑战三：Upvalue 逃逸）
- VM 主循环为 switch 版（挑战四彩蛋：computed goto 可再提 ~30%）
