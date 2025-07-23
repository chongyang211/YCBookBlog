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
- 03.编译流程原理
  - 3.1 编译指令解读
  - 3.2 命令执行过程
  - 3.3 编译文件思考
  - 3.4 运行命令


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













