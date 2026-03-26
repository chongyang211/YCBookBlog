#### 目录介绍
- 01.先说个问题
  - 1.1 背景介绍一下
  - 1.2 遇到问题报错
  - 1.3 问题原因
  - 1.4 问题原因分析
- 02.解决方法尝试
  - 2.1 确保文件被编译
  - 2.2 确保文件路径正确
  - 2.3 Makefile管理编译
  - 2.4 检查编译器架构
- 03.编译流程解读
  - 3.1 编译指令解读
  - 3.2 命令执行过程
  - 3.3 编译文件思考
  - 3.4 运行命令
- 04.项目编译原理
  - 4.1 编译详细流程
  - 4.2 预处理（Preprocessing）
  - 4.3 编译（Compilation）
  - 4.4 汇编（Assembly）
  - 4.5 链接（Linking）
  - 4.6 可执行文件
  - 4.7 关键点说明
  - 4.8 为何需要链接
- 05.深入编译器内部
  - 5.1 编译器做了什么
  - 5.2 词法分析阶段
  - 5.3 语法分析阶段
  - 5.4 语义分析阶段
  - 5.5 中间代码生成
  - 5.6 代码优化阶段
  - 5.7 目标代码生成
  - 5.8 用案例串联全流程
- 06.深入链接器内部
  - 6.1 符号表是什么
  - 6.2 符号解析过程
  - 6.3 静态链接和动态链接
  - 6.4 静态库与动态库区别
  - 6.5 链接顺序的影响
  - 6.6 常见链接错误分析
- 07.CMake构建工具
  - 7.1 为什么需要CMake
  - 7.2 CMake是什么
  - 7.3 CMakeLists基本结构
  - 7.4 用CMake管理银行项目
  - 7.5 CMake核心命令详解
  - 7.6 CMake工作原理
  - 7.7 CMake构建流程图解
  - 7.8 多目录项目管理
- 08.CMake进阶实践
  - 8.1 添加第三方库依赖
  - 8.2 条件编译控制
  - 8.3 生成静态库和动态库
  - 8.4 安装和打包
  - 8.5 构建工具对比总结
  - 8.6 完整项目CMake示例



## 01.先说个问题

### 1.1 背景介绍一下

项目结构如下：

```
project/
├── Account.h
├── Account.cpp
├── BankUserManager.cpp
```

当写完了代码之后，就开始进行编译，如下所示：

```bash
CHONGYYANG-MB1:bank yangchong$ g++ BankUserManager.cpp
```

### 1.2 遇到问题报错

然后遇到报错，报错如下：

```text
Undefined symbols for architecture arm64:
  "Account::deposit(double)", referenced from:
      Bank::deposit(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, double) in BankUserManager-397329.o
  "Account::transfer(Account&, double)", referenced from:
      Bank::transfer(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&, double) in BankUserManager-397329.o
  "Account::getName() const", referenced from:
      FileManager::saveToFile(Bank const&, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&) in BankUserManager-397329.o
ld: symbol(s) not found for architecture arm64
clang++: error: linker command failed with exit code 1 (use -v to see invocation)
```

### 1.3 问题原因

你遇到的错误是由于链接器（Linker）找不到 `Account` 类的实现导致的。

具体来说，编译器在编译 `BankUserManager.cpp` 时，发现它依赖于 `Account` 类的成员函数（如 `deposit`、`withdraw`、`transfer` 等），但在链接阶段，这些函数的实现没有被找到，因此报错。

### 1.4 问题原因分析

1. **`Account` 类的实现未编译**，你可能将 `Account` 类的声明和实现分开了（例如，`Account.h` 和 `Account.cpp`），但在编译时没有将 `Account.cpp` 一起编译。

2. **未正确链接目标文件**，如果你已经编译了 `Account.cpp`，但没有将生成的目标文件（如 `Account.o`）与 `BankUserManager.cpp` 的目标文件链接在一起，也会导致这个问题。

## 02.解决方法尝试

### 2.1 确保文件被编译

**确保 `Account` 类的实现文件被编译**，项目结构如下：

```
project/
├── Account.h
├── Account.cpp
├── BankUserManager.cpp
```

在终端中，确保同时编译 `Account.cpp` 和 `BankUserManager.cpp`：

```bash
g++ Account.cpp BankUserManager.cpp -o BankUserManager
```

### 2.2 确保文件路径正确

如果 `Account.h` 和 `Account.cpp` 不在同一目录下，需要在编译时指定正确的路径。例如：

```bash
g++ src/Account.cpp src/BankUserManager.cpp -o BankUserManager
```

### 2.3 Makefile管理编译

如果项目较大，可以使用 `Makefile` 来管理编译过程。例如：

```makefile
CXX = g++
CXXFLAGS = -std=c++11
TARGET = BankUserManager
SRCS = Account.cpp BankUserManager.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
    $(CXX) $(CXXFLAGS) -c $< -o $@

clean:
    rm -f $(OBJS) $(TARGET)
```

然后在终端中运行：

```bash
make
```

### 2.4 检查编译器架构

如果你在 ARM64 架构（如 Apple Silicon）上编译，确保编译器支持该架构。你可以使用以下命令检查编译器版本和架构：

```bash
g++ -v
```

如果编译器不支持 ARM64，可以尝试安装支持 ARM64 的编译器，或者使用交叉编译工具。

## 03.编译流程原理

```
project/
├── Account.h
├── Account.cpp
├── BankUserManager.cpp
```

- `Account.h` 包含 `Account` 类的声明。
- `Account.cpp` 包含 `Account` 类的实现。
- `BankUserManager.cpp` 包含 `main` 函数，使用 `Account` 类。

### 3.1 编译指令解读

```bash
g++ Account.cpp BankUserManager.cpp -o BankUserManager
```

**1. `g++`**

`g++` 是 GNU C++ 编译器，用于编译和链接 C++ 程序。它是 GCC（GNU Compiler Collection）的一部分。

**2. `Account.cpp` 和 `BankUserManager.cpp`**

这两个是 C++ 源代码文件：

- `Account.cpp`：包含 `Account` 类的实现。
- `BankUserManager.cpp`：包含 `main` 函数或其他逻辑，依赖于 `Account` 类。

编译器会分别编译这两个文件，生成目标文件（`.o` 文件），然后将它们链接在一起。

**3. `-o BankUserManager`**

`-o` 是编译器的选项，用于指定输出文件的名称。`BankUserManager` 是生成的可执行文件的名称。

如果没有 `-o` 选项，编译器会默认生成一个名为 `a.out` 的可执行文件（在 Linux/macOS 上）。

### 3.2 命令执行过程

当运行 `g++ Account.cpp BankUserManager.cpp -o BankUserManager` 时，编译器会执行以下步骤：

1. **编译阶段**
- 编译器将 `Account.cpp` 编译成目标文件（`Account.o`）。
- 编译器将 `BankUserManager.cpp` 编译成目标文件（`BankUserManager.o`）。

2. **链接阶段**
- 编译器将 `Account.o` 和 `BankUserManager.o` 链接在一起，生成最终的可执行文件 `BankUserManager`。

### 3.3 编译文件思考

`BankUserManager.cpp` 依赖于 `Account` 类，而 `Account` 类的实现在 `Account.cpp` 中。

如果只编译 `BankUserManager.cpp`，编译器会报错，因为它找不到 `Account` 类的实现。

通过同时编译 `Account.cpp` 和 `BankUserManager.cpp`，编译器可以找到所有必要的代码，并正确生成可执行文件。

### 3.4 运行命令

在终端中进入项目目录，然后运行：

```bash
g++ Account.cpp BankUserManager.cpp -o BankUserManager
```

如果编译成功，会生成一个名为 `BankUserManager` 的可执行文件。你可以运行它：

```bash
./BankUserManager
```

`g++ Account.cpp BankUserManager.cpp -o BankUserManager` 的作用是：

1. 编译 `Account.cpp` 和 `BankUserManager.cpp`。
2. 将生成的目标文件链接在一起。
3. 生成一个名为 `BankUserManager` 的可执行文件。

通过这条命令，你可以将多个 C++ 源文件编译成一个完整的可执行程序。

## 04.项目编译原理

### 4.1 编译详细流程

```
Account.cpp  --> 预处理 --> 编译 --> 汇编 --> Account.o
BankUserManager.cpp --> 预处理 --> 编译 --> 汇编 --> BankUserManager.o
Account.o + BankUserManager.o --> 链接 --> BankUserManager（可执行文件）
```

### 4.2 预处理（Preprocessing）

**作用**：处理源代码中的预处理指令（如 `#include`、`#define` 等），生成一个“纯净”的 C++ 文件。

**具体步骤**：

- 将 `#include` 指令替换为对应头文件的内容。例如，`#include "Account.h"` 会被替换为 `Account.h` 文件的内容。
- 展开宏定义（`#define`）。
- 处理条件编译指令（如 `#ifdef`、`#ifndef` 等）。

**输出**：生成一个临时文件（通常以 `.i` 或 `.ii` 为扩展名），其中包含所有预处理后的代码。

### 4.3 编译（Compilation）

**作用**：将预处理后的 C++ 代码转换为汇编代码。

**具体步骤**：
- 编译器（`g++`）对预处理后的代码进行语法和语义分析。
- 生成与目标平台（如 ARM64、x86_64）相关的汇编代码。

**输出**：生成汇编文件（通常以 `.s` 为扩展名）。

### 4.4 汇编（Assembly）

**作用**：将汇编代码转换为机器码（目标文件）。

**具体步骤**：
- 汇编器将汇编代码转换为机器指令。
- 生成目标文件（`.o` 文件），其中包含机器码和符号表（用于链接阶段）。

**输出**：生成目标文件（`Account.o` 和 `BankUserManager.o`）。


### 4.5 链接（Linking）

**作用**：将多个目标文件和库文件合并，生成最终的可执行文件。

**具体步骤**：
- 链接器（`ld`）将 `Account.o` 和 `BankUserManager.o` 合并。
- 解析符号引用（如 `BankUserManager.o` 中对 `Account` 类方法的调用）。
- 将标准库（如 `libstdc++`）和其他依赖库链接到可执行文件中。

**输出**：生成可执行文件（`BankUserManager`）。


### 4.6 可执行文件

**作用**：包含机器码和运行时信息，可以直接在操作系统上运行。

**具体内容**：

- 机器码：CPU 可以直接执行的指令。
- 符号表：用于调试和动态链接。
- 段信息：如代码段（`.text`）、数据段（`.data`）、未初始化数据段（`.bss`）等。


### 4.7 关键点说明

1. **预处理**：处理 `#include` 和宏定义，生成纯净的 C++ 代码。
2. **编译**：将 C++ 代码转换为汇编代码。
3. **汇编**：将汇编代码转换为机器码（目标文件）。
4. **链接**：将多个目标文件和库文件合并，生成可执行文件。

### 4.8 为何需要链接

- `BankUserManager.cpp` 依赖于 `Account` 类的实现，而 `Account` 类的实现在 `Account.cpp` 中。
- 在编译阶段，`BankUserManager.cpp` 只知道 `Account` 类的声明（通过 `Account.h`），但不知道其实现。
- 在链接阶段，链接器将 `Account.o` 和 `BankUserManager.o` 合并，解析符号引用，生成完整的可执行文件。


## 05.深入编译器内部

前面讲了编译的大致流程（预处理→编译→汇编→链接），但"编译"这一步其实内部非常复杂。这一节用银行项目的代码深入拆解编译器内部到底做了什么。

### 5.1 编译器做了什么

编译器的核心任务是：**把人写的 C++ 代码翻译成机器能执行的指令**。

这个翻译过程不是一步到位的，而是分成多个阶段，像流水线一样依次处理：

```
源代码(.cpp) → 词法分析 → 语法分析 → 语义分析 → 中间代码生成 → 代码优化 → 目标代码生成(.s)
```

每一个阶段都有明确的输入和输出，下面逐个来看。

### 5.2 词法分析阶段

**做什么**：把源代码的字符流拆分成一个个有意义的"词"（Token）。

就像你读一篇英文文章，第一步是识别出每个单词，而不是一个字母一个字母地看。

以 `Account.cpp` 中的一行代码为例：

```cpp
void Account::deposit(double amount) {
```

词法分析器会把它拆成以下 Token：

| Token | 类型 |
|-------|------|
| `void` | 关键字 |
| `Account` | 标识符 |
| `::` | 作用域运算符 |
| `deposit` | 标识符 |
| `(` | 左括号 |
| `double` | 关键字 |
| `amount` | 标识符 |
| `)` | 右括号 |
| `{` | 左花括号 |

**关键点**：词法分析只关心"这是什么词"，不关心这些词组合起来是什么意思。如果你写了 `123abc`，词法分析阶段就能发现这不是一个合法的 Token，会报词法错误。

### 5.3 语法分析阶段

**做什么**：把 Token 序列组织成一棵**抽象语法树**（AST，Abstract Syntax Tree），检查代码的语法结构是否正确。

就像读懂一个句子不仅要认识单词，还要理解主谓宾的结构。

对于 `void Account::deposit(double amount) { balance += amount; }`，语法分析器会构建类似这样的树：

```
FunctionDefinition
├── 返回类型: void
├── 类名: Account
├── 函数名: deposit
├── 参数列表
│   └── 参数: double amount
└── 函数体
    └── 复合赋值表达式 (+=)
        ├── 左: balance
        └── 右: amount
```

**关键点**：语法分析检查的是代码结构是否合法。比如你写了 `void deposit(double,) {}`，多了一个逗号，语法分析阶段就会报错 `expected parameter declarator`。但语法分析**不检查**变量是否已声明、类型是否匹配等问题。

### 5.4 语义分析阶段

**做什么**：在语法树的基础上检查代码的**含义是否正确**。

语法正确不等于语义正确。比如 `"hello" + 3.14` 语法上没问题，但语义上不合法。

以银行项目为例，语义分析器会检查：

1. **类型检查**：`deposit(double amount)` 的参数是 `double`，如果你调用 `account.deposit("abc")`，语义分析会报类型不匹配。
2. **作用域检查**：`balance` 是 `Account` 类的成员变量，在 `deposit` 方法里可以访问；但如果你在 `main` 函数里直接写 `balance += 100`，语义分析会报"未声明的标识符"。
3. **访问权限检查**：如果 `balance` 是 `private` 成员，外部直接访问会报错。
4. **函数签名匹配**：调用 `transfer(Account& to, double amount)` 时，参数数量和类型必须匹配。

**关键点**：语义分析是编译器帮你"检查逻辑"的阶段。很多编译报错（比如 `use of undeclared identifier`、`no matching function for call to`）都出自这个阶段。

### 5.5 中间代码生成

**做什么**：把语法树转换成一种**与平台无关**的中间表示（IR，Intermediate Representation）。

为什么不直接生成机器码？因为中间代码有两个好处：

1. **方便优化**：在 IR 上做优化比在汇编上容易得多。
2. **跨平台**：同一份 IR 可以生成 ARM64（Apple Silicon）或 x86_64（Intel）的机器码。

以 `balance += amount;` 为例，生成的 IR（以 LLVM IR 为例）大致如下：

```llvm
%1 = load double, double* %balance      ; 读取 balance 的值
%2 = load double, double* %amount       ; 读取 amount 的值
%3 = fadd double %1, %2                 ; 浮点加法
store double %3, double* %balance       ; 结果存回 balance
```

**关键点**：你用 `clang++ -S -emit-llvm Account.cpp` 可以查看实际生成的 LLVM IR，这是理解编译器行为的好方法。

### 5.6 代码优化阶段

**做什么**：对中间代码进行优化，让生成的程序更快、更小。

编译器优化是一个非常深的领域，这里举几个常见例子：

**1. 常量折叠**：编译期直接算出结果

```cpp
// 优化前
double fee = 100 * 0.05;
// 优化后（编译器直接算出 5.0）
double fee = 5.0;
```

**2. 内联展开**：把小函数直接"复制粘贴"到调用处，省去函数调用开销

```cpp
// 如果 getBalance() 只是 return balance;
// 编译器可能把 account.getBalance() 直接替换为访问 account.balance
```

**3. 死代码消除**：删除永远不会执行的代码

```cpp
if (false) {
    cout << "这行永远不会执行";  // 编译器会删掉这段
}
```

**关键点**：`g++` 通过 `-O` 参数控制优化等级：
- `-O0`：不优化（默认，适合调试）
- `-O1`：基础优化
- `-O2`：推荐的优化等级（速度和大小的平衡）
- `-O3`：激进优化（可能增大代码体积）
- `-Os`：优化代码大小

### 5.7 目标代码生成

**做什么**：把优化后的中间代码转换成**目标平台的汇编代码**。

对于 Apple Silicon Mac（ARM64），`balance += amount;` 最终会变成类似这样的 ARM64 汇编：

```asm
ldr     d0, [x0, #8]     ; 从对象偏移8字节处加载 balance 到寄存器 d0
ldr     d1, [sp, #16]    ; 从栈上加载 amount 到寄存器 d1
fadd    d0, d0, d1        ; d0 = d0 + d1（浮点加法）
str     d0, [x0, #8]     ; 将结果存回 balance
```

你可以用 `g++ -S Account.cpp` 生成汇编文件 `Account.s`，查看完整的汇编代码。

### 5.8 用案例串联全流程

以银行项目中 `account.deposit(500.0)` 这一行调用为例，串联编译器内部全流程：

```
源代码: account.deposit(500.0);

→ 词法分析: [account] [.] [deposit] [(] [500.0] [)] [;]
           标识符  点运算符 标识符  左括号 浮点常量 右括号 分号

→ 语法分析: 构建AST
           MemberFunctionCall
           ├── 对象: account
           ├── 方法: deposit
           └── 参数: 500.0 (double)

→ 语义分析:
           ✓ account 是 Account 类型（已声明）
           ✓ Account 类有 deposit(double) 方法
           ✓ 500.0 是 double，参数类型匹配
           ✓ deposit 是 public 方法，可以调用

→ 中间代码: call void @Account::deposit(%class.Account* %account, double 500.0)

→ 代码优化: （此处没有什么可优化的，保持原样）

→ 目标代码:
           mov     x0, x19          ; 把 account 对象地址放入 x0
           fmov    d0, #500.0       ; 把 500.0 放入浮点寄存器
           bl      _ZN7Account7depositEd  ; 调用 Account::deposit
```

到这一步，汇编器会把汇编代码变成 `.o` 目标文件中的二进制机器码，然后交给链接器完成最终的拼接。


