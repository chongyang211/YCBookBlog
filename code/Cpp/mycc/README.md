# mycc — 迷你编译器解释器

一个用现代 C++17 从零实现的迷你编程语言解释器，完整走完「**源码 → Token → AST → 字节码 → 执行**」五段式编译全链路。它是《C++ 入门到精通》卷二综合案例的第七关，用 17 个类、约 1800 行代码，把 `std::variant`、多态、访问者模式、模板、异常体系等 17 章知识点揉进一个真实可跑的语言实现。

## 特性一览

- **词法分析**：状态机把源码切成 16 类 `Token`，`std::variant` 承载多种载荷
- **语法分析**：递归下降 + 优先级爬升，把扁平 Token 流升维成 17 类 AST 节点
- **语义检查**：访问者模式 + 符号表栈，实现词法作用域与类型检查
- **字节码生成**：36 条栈式指令，跳转回填（backpatching）编译 if/while/&&/||
- **栈式虚拟机**：fetch-decode-execute 主循环 + 调用栈，递归不爆 C++ 栈
- **异常体系**：四级异常类树（词法/语法/类型/运行时），带文件、行号、退出码
- **双模式**：交互式 REPL + 文件模式

## 目录结构

```text
mycc/
├── CMakeLists.txt              # 构建脚本（C++17）
├── README.md
├── include/mycc/               # 头文件（namespace mycc）
│   ├── error.h                 # 四级异常类树
│   ├── token.h                 # Token + TokKind 枚举
│   ├── lexer.h                 # 词法分析器
│   ├── ast.h                   # AstNode 基类 + 17 个派生类
│   ├── parser.h                # 递归下降语法分析器
│   ├── visitor.h               # AstVisitor<R> 模板基类
│   ├── type_checker.h          # 类型检查访问者
│   ├── opcode.h                # 36 条字节码指令
│   ├── chunk.h                 # 字节码容器（常量池 + 回填）
│   ├── codegen.h               # 代码生成访问者
│   ├── value.h                 # 运行期值（variant 五型）
│   └── vm.h                    # 栈式虚拟机
├── src/                        # 实现文件
│   ├── token.cpp  lexer.cpp  parser.cpp  type_checker.cpp
│   ├── opcode.cpp  chunk.cpp  codegen.cpp  vm.cpp
│   └── main.cpp                # REPL + 文件模式入口
└── examples/                   # 示例源码
    ├── hello.mycc              # 函数 + 参数 + 字符串
    ├── fib.mycc                # 递归斐波那契
    ├── bad.mycc                # 语法错误演示
    └── divzero.mycc            # 运行时错误演示
```

## 类职责说明

### 前端（词法 / 语法）

| 类 | 文件 | 作用 |
|----|------|------|
| `Token` / `TokKind` | `token.h` | 词法单元：16 类标记 + `variant<monostate, double, string>` 载荷 |
| `Lexer` | `lexer.h` | 状态机词法分析器，把源码字符串切成 `Token` 流，识别关键字、运算符、字符串、注释 |
| `AstNode` | `ast.h` | AST 抽象基类，携带行号 `line` 与类型标签 `ty`，定义双 `accept` 接口 |
| 17 个派生类 | `ast.h` | 每种语法结构一个类：`NumLit`/`StringLit`/`BoolLit`/`VarRef`/`BinOp`/`UnaryOp`/`Assign`/`Call`（表达式）、`LetDecl`/`PrintStmt`/`ExprStmt`/`BlockStmt`/`IfStmt`/`WhileStmt`/`FuncDecl`/`ReturnStmt`/`Program`（语句） |
| `Parser` | `parser.h` | 递归下降语法分析器，按优先级把 Token 流组装成 AST |

### 语义 / 代码生成（访问者）

| 类 | 文件 | 作用 |
|----|------|------|
| `AstVisitor<R>` | `visitor.h` | 模板访问者基类，17 个纯虚 `visit` 方法，用返回类型 `R` 区分 TypeChecker（`Type`）与 Codegen（`void`） |
| `TypeChecker` | `type_checker.h` | 类型检查访问者，用 `vector<unordered_map>` 实现词法作用域栈，检查变量定义、类型匹配、参数个数 |
| `OpCode` | `opcode.h` | 36 条栈式字节码指令枚举 |
| `Chunk` | `chunk.h` | 字节码容器：指令流 + 常量池（去重）+ 行号表，提供 `emitJump`/`patchJump`/`emitLoop` 回填三剑客 |
| `Codegen` | `codegen.h` | 代码生成访问者，把 AST 翻译成字节码，处理 if/while 跳转回填、&&/|| 短路、函数独立 Chunk |

### 执行 / 系统

| 类 | 文件 | 作用 |
|----|------|------|
| `Value` | `value.h` | 运行期值：`variant<Nil, bool, double, string, size_t>`，第五型是函数索引（函数是一等公民） |
| `VM` | `vm.h` | 栈式虚拟机，`fetch-decode-execute` 主循环 + `CallFrame` 调用栈，用堆模拟栈避免递归爆 C++ 栈 |
| `MyccError` 树 | `error.h` | 四级异常：`LexError`/`ParseError`/`TypeError`/`RuntimeError`，带阶段名、文件名、行号、退出码（2/3/4） |

## 快速开始

### 编译

```bash
cd mycc
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

要求编译器支持 C++17（GCC ≥ 7 / Clang ≥ 5 / MSVC ≥ 19.14）。

### 运行

```bash
# 文件模式
./build/mycc examples/fib.mycc        # 输出斐波那契数列 0..34
./build/mycc examples/hello.mycc      # 输出 mycc / world

# REPL 模式（直接运行，无参数）
./build/mycc
```

### REPL 命令

```text
mycc> print 1 + 2 * 3;                 # 直接输入代码即可执行
7
mycc> :run  fn fib(n) { if (n < 2) return n; return fib(n-1)+fib(n-2); }  print fib(10);
55
mycc> :dump  let a = 1 + 2;            # 反汇编看字节码
mycc> :tcheck  print 1 + "abc";        # 仅类型检查
[Type] line 1: arithmetic requires Num operands
mycc> :q                               # 退出
```

## 语言语法

| 特性 | 示例 |
|------|------|
| 字面量 | `42`、`3.14`、`"hello"`、`true`、`false` |
| 算术 | `+ - * / %` |
| 比较 | `== != < <= > >=` |
| 逻辑 | `&& \|\| !`（短路求值） |
| 变量 | `let x = 1;  x = x + 1;` |
| 控制流 | `if / else / while` |
| 函数 | `fn add(a, b) { return a + b; }` |
| 输出 | `print 表达式;` |
| 注释 | `// 单行注释` |

## 架构与数据流

```text
源码 "print 1 + 2 * 3;"
   │  Lexer（词法分析）
   ▼
Token 流 [PRINT, NUM(1), +, NUM(2), *, NUM(3), ;, EOF]
   │  Parser（递归下降）
   ▼
AST  PrintStmt ─ BinOp(+) ─ NumLit(1)
                            └ BinOp(*) ─ NumLit(2)
                                       └ NumLit(3)
   │  TypeChecker（类型检查）
   ▼
标注类型后的 AST（每个节点贴上 ty）
   │  Codegen（代码生成）
   ▼
字节码 [CONST 1, CONST 2, CONST 3, MUL, ADD, PRINT, HALT]
   │  VM（栈式执行）
   ▼
输出 "7"
```

依赖方向**严格单向**：`main → vm/codegen → chunk/type_checker → ast → token/error`，各阶段输出是下一阶段输入，无跨阶段耦合。

## 关键设计取舍

- **`shared_ptr<AstNode>` 而非 `unique_ptr`**：教学场景下 REPL 复用 AST、代码示例更清晰；工程上应改 `unique_ptr`（见挑战题）。
- **模板版访问者**：`AstVisitor<R>` 用返回类型区分 TypeChecker（`Type`）与 Codegen（`void`），突破 GoF 版"单一返回类型"限制。
- **字节码而非树遍历**：性能约 3 倍差距，且跳转回填是系统编程的通用模式（汇编器、链接器、JIT 都用）。
- **栈式 VM 而非寄存器式**：与 AST 递归求值语义天然对齐，编译器友好。
- **`vector` 实现作用域栈 / 调用栈**：既能 push/pop，又能反向遍历查找，比 `std::stack` 更灵活。
