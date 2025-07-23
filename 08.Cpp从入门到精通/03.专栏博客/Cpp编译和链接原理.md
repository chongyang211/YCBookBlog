#### 目录介绍
- 01.先说个问题
  - 1.1 背景介绍一下
  - 1.2 遇到问题报错
  - 1.3 问题原因分析


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

### 1.3 问题原因分析

你遇到的错误是由于链接器（Linker）找不到 `Account` 类的实现导致的。

具体来说，编译器在编译 `BankUserManager.cpp` 时，发现它依赖于 `Account` 类的成员函数（如 `deposit`、`withdraw`、`transfer` 等），但在链接阶段，这些函数的实现没有被找到，因此报错。
















