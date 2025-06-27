# 专栏笔记总结大全


## C++入门到精通上

**01.C++简单介绍**

- 1.1.1 C++语言介绍：是一种静态类型的、编译式的、通用的、大小写敏感的、不规则的编程语言，支持过程化编程、面向对象编程和泛型编程。 
- 1.2 C++四大特性：包括面向对象开发的四大特性：封装，继承，多台，抽象。 
- 1.3 C++标准库：1.核心语言；2.C++ 标准库；3.标准模板库（STL）。 
- 1.4 C++编译器认识：通过 g++ 来编译cpp文件，编译后会生成一个a.out文件。 
- 1.5 helloWorld：main函数是C++程序的入口点，程序从这里开始执行。 
- 1.6 多种注释使用：在代码中加一些说明和解释，方便自己或其他程序员程序员阅读代码。 
- 1.7 命名空间：引入命名空间概念，主要作为附加信息来区分不同库中相同名称的函数，类，变量等。 
- 1.8 头文件名：使用.h扩展名：传统上，C++头文件使用.h作为文件扩展名 
- 1.9 cout输出：std::cout是标准输出流对象，用于将文本输出到控制台。使用<<运算符将数据插入到std::cout中，并将其输出到屏幕上。

**02.修饰符和标识符**

- 2.1 修饰符：是用于修改基本数据类型的关键字。它们可以改变数据类型的行为、范围或存储方式。
- 2.2 标志符：C++规定给标识符（变量、常量）命名时，有一套自己的规则。
- 2.3 常见关键字：预先保留的单词（标识符），在定义变量或者常量时候，不要用关键字。

**03.数据和数据类型**

- 3.1 变量和常量：变量是给一段指定的内存空间起名，方便操作这段内存。
- 3.2 数据类型介绍：有基本数据类型比如int，复合数据类型比如array，指针类型，其他数据类型比如字符串和map
- 3.3 整型数据：比较常见有int，short，long，long long等不同的类型。
- 3.4 浮点型数据：浮点型变量分为两种，单精度float【占4个字节】和双精度double【占8个字节】
- 3.5 字符型数据：字符型变量用于显示单个字符
- 3.6 布尔类型数据：布尔数据类型代表真或假的值
- 3.7 sizeof：利用sizeof关键字可以统计数据类型所占内存大小
- 3.8 数据输入：语法：** `cin >> 变量 `，用于从键盘获取数据
- 3.9 类型的转换：自动类型转化；还有强制类型转化，通过使用特定的转换操作符来显式执行的类型转换。
- 3.10 字符串使用：
- 3.11 转义字符：用于表示一些，不能显示出来的ASCII字符。

**04.运算符和表达式**

- 4.1 运算符介绍：用于执行代码的运算
- 4.2 算术运算符：有加减乘除，取模，还有递增和递减
- 4.3 赋值运算符：用于将表达式的值赋给变量
- 4.4 比较运算符：用于表达式的比较，并返回一个真值或假值
- 4.5 逻辑运算符：逻辑运算符用于执行逻辑操作并生成布尔结果
- 4.6 位运算符：
- 4.7 杂项运算符：
- 4.8 运算符优先级：

**06.函数**

- 6.1 函数概述和定义：函数的定义一般主要有5个步骤，返回值类型，函数名，参数，函数体语句，return返回值
- 6.2 函数的调用，如何理解形参和实参：形式参数是函数或方法定义中声明的参数，而实际参数是在函数或方法调用时传递给形式参数的具体值。
- 6.3 值传递如何理解：是将实际参数的值复制给函数或方法的形式参数。在值传递中，函数或方法使用的是形式参数的副本，而不是直接操作实际参数本身。
- 6.5 函数声明：是指在代码中提前声明函数的名称、参数列表和返回类型，以便在后续的代码中使用该函数。
- 6.6 函数分文件编写：它将函数的声明和定义分别放置在不同的文件中。这种方式可以提高代码的可维护性、可读性和可重用性。
- 6.7 如何理解函数默认参数：函数默认参数是一种允许在函数声明中为某些参数指定默认值的机制。如果调用函数时没有为这些参数提供实参，那么函数会自动使用默认值。
- 6.8 函数重载：是指在同一个作用域内，可以定义多个同名函数，但它们的参数列表不同。函数重载允许使用相同的函数名来实现不同的功能


**07.指针**

- 7.1 指针如何理解：它存储了一个内存地址。指针可以指向其他变量或对象的内存地址，允许直接访问或操作这些变量或对象。
- 7.3 指针占用内存空间：在 32 位系统中，指针通常占用 4 个字节（32 位）的内存空间。
- 7.4.1 如何理解空指针：空指针通常用来表示指针没有被初始化或者指向了无效的内存地址。在 C++ 中，空指针的值通常是 0 或者使用 nullptr 关键字表示。
- 7.4.2 如何理解野指针：野指针是指指向未知内存地址或已释放的内存地址的指针，使用野指针可能会导致程序崩溃、内存泄漏或产生不可预测的行为。
- 7.5 const修饰指针如何理解：1.常量指针；2.指向常量的指针；3.指向常量的常量指针。
- 7.6 如何理解指针和数组：1.数组名是指针；2.指针可以用来访问数组中的元素；3.当传递数组给函数时，实际上传递的是数组的地址。

**08.引用**

- 8.1 如何理解引用的定义：引用是一个变量的别名，它允许你通过不同的名称访问同一个变量。引用是 C++ 中非常强大的特性，常用于函数参数传递、返回值优化等场景。
- 8.2 如何理解引用概念：引用是一个变量的别名，它必须在声明时初始化，并且一旦绑定到一个变量，就不能再绑定到其他变量。
- 8.3 引用与函数：1.引用常用于函数参数传递，可以避免拷贝开销，同时允许函数修改实参。2.引用可以作为函数的返回值，但必须确保返回的引用指向有效的内存。
- 8.4 如何理解引用的特性：1.引用在声明时必须初始化，否则会编译错误。2.不可重新绑定，引用一旦绑定到一个变量，就不能再绑定到其他变量。
- 8.5 如何理解引用的本质：引用的本质在c++内部实现是一个指针常量
- 8.6 如何理解常量引用：常量引用（const 引用）用于防止通过引用修改原变量，同时避免拷贝开销。

**09.结构体**

- 9.1 结构体的概念是什么：自定义的数据类型，用于组合不同类型的数据成员。结构体允许你将多个相关的数据项组合在一起，以便更方便地管理和操作这些数据。
- 9.2 结构体定义和作用：struct 结构体名 变量名 这种方式来定义结构体。主要作用是：1.组织数据，2.传递复杂数据
- 9.3 结构体数组如何理解：结构体数组允许你在一个数组中存储多个结构体实例，每个实例可以包含多个数据成员。
- 9.4 结构体指针如何理解：结构体指针是指向结构体实例的指针，允许你通过指针来访问和操作结构体的数据成员。
- 9.5 如何理解结构体嵌套：结构体嵌套是指在一个结构体中包含另一个结构体作为其成员。将多个相关的数据项组合在一起，以便更方便地管理和操作这些数据。
- 9.6 如何理解结构体做参数：
- 9.7 如何理解const修饰结构体：使用 const 关键字修饰结构体意味着该结构体实例是不可修改的，即其成员变量在声明后不能被修改

**10.类和对象**

- 10.1.1 如何理解c++类的含义：类是一种用户自定义的数据类型，用于封装数据和方法（函数）。类可以看作是一种模板，用于创建具有相似属性和行为的对象。
- 10.1.2 struct和class区别：struct和class唯一的区别就在于，默认的访问权限不同
- 10.2.1 类的定义如何理解：用户自定义的类型，在设计类的时候，属性和行为写在一起，表现事物。
- 10.2.3 如何理解对象指针：对象指针是指向类的实例（对象）的指针，它存储了对象在内存中的地址。
- 10.3.1 如何理解成员变量：成员变量是类或结构体中声明的变量，用于存储对象的状态信息。定义了对象的属性。
- 10.3.3 如何理解访问权限：访问权限是指控制类的成员（成员变量和成员函数）对外部代码的可见性和访问权限。public、private 和 protected，用于限制类的成员对外部代码的访问方式。
- 10.4.1 构造和析构函数：构造函数主要作用在于创建对象时为对象的成员属性赋值；析构函数主要作用在于对象**销毁前**系统自动调用。
- 10.4.2 构造函数分类：按参数分为有参构造和无参构造，按类型分为普通构造和拷贝构造。
- 10.4.3 拷贝构造函数：特殊的构造函数，用于创建一个新对象，并将其初始化为另一个同类型对象的副本。
- 10.4.4 构造调用规则：
- 10.4.5 初始化列表：
- 10.4.8 深拷贝与浅拷贝：浅拷贝是简单的赋值拷贝操作。深拷贝是在堆区重新申请空间，进行拷贝操作！



**15.内存分配堆和栈**

- 15.1 如何理解c++内存分区模型：将内存大方向划分为4个区域，代码区，全局区，栈区，堆区
- 15.1.1 如何理解代码区：存储程序的二进制代码（即编译后的机器指令）。
- 15.1.2 如何理解全局区：1.全局变量存储在全局区，2.静态局部变量存储在全局区。特点是在程序启动时分配，程序结束时释放。
- 15.1.3 如何理解栈区：存储局部变量、函数参数、函数返回地址等。一般在函数方法中，比如局部变量存储在栈区，方法调用完后就释放。
- 15.1.4 如何理解堆区：存储动态分配的内存（如 new 和 malloc 分配的内存），这些动态内存需要手动释放
- 15.2.1 什么是动态内存：指程序在运行时（而不是编译时）从堆区（Heap）分配的内存。通过 new 和 delete 操作符来分配和释放内存。
- 15.2.5 如何理解内存泄漏：如果动态分配的内存没有被释放，会导致内存泄漏。解决方法：确保每次 new 都有对应的 delete。
- 15.2.6 如何理解野指针：未初始化的指针称为野指针，访问野指针会导致未定义行为。
- 15.2.7 如何理解悬空指针问题：释放内存后，指针仍然指向已释放的内存地址，称为悬空指针。解决方法：释放内存后将指针置为 nullptr。


**16.IO流和File**

- 16.1.1 输入输出流：字节流是从设备流向内存，这叫做输入操作。如果字节流是从内存流向设备，这叫做输出操作。
- 16.1.2 I/O库头文件：ios 是所有流类的基类，它派生出 istream （输入） 和 ostream （输出）。
- 16.1.3 标准输出流cout：cout是标准输出流对象，用于向控制台输出数据，一般可以用它打印数据。
- 16.1.4 标准输入流cin：cin是标准输入流对象，用于从控制台或终端读取用户输入的数据。
- 16.1.5 标准错误流cerr：cerr是标准错误流对象，用于向控制台或终端输出错误信息。
- 16.2.1 文件是什么：
- 16.2.2 文件类型分类：文本文件，二进制文件
- 16.2.3 操作文件类：ifstream 专用于从文件中读取数据；ofstream 专用于向文件中写入数据；fstream 既可用于从文件中读取数据，又可用于向文件中写入数据。
- 16.2.4 文本文件写：
- 16.2.6 二进制文件写
- 16.3.1 文件错误与状态：
- 16.3.2 文件的追加：
- 16.3.3 文件结尾的判断：
- 16.3.4 在指定位置读/写文件：


## 参考博客

黑马程序员：https://space.bilibili.com/37974444

黑马程序员匠心之作|C++教程从0到1入门编程,学习编程不再难 ：https://www.bilibili.com/video/av41559729/

最新QT从入门到实战完整版|传智教育：https://www.bilibili.com/video/BV1g4411H78N

职工系统：https://github.com/zz2summer/StudentManageSystem

五子棋：https://github.com/zz2summer/GoBangByCpp

- **书籍**：
    - 《C++ Primer》
    - 《Effective C++》
    - 《The C++ Programming Language》（Bjarne Stroustrup 著）
- **在线教程**：
    - [C++ 官方文档](https://isocpp.org/)
    - [Learn C++](https://www.learncpp.com/)
- **开发工具**：
    - 编译器：GCC、Clang、MSVC。
    - IDE：Visual Studio、Code::Blocks、CLion。


--------------------------------------------------------------------------------------------------



--------------------------------------------------------------------------------------------------


### **4. 函数对象**
#### **4.1 自定义函数对象**
```cpp
#include <iostream>
#include <vector>
#include <algorithm>

struct Square {
    int operator()(int x) const {
        return x * x;
    }
};

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    Square square;
    std::transform(vec.begin(), vec.end(), vec.begin(), square);
    for (int i : vec) {
        std::cout << i << " "; // 输出: 1 4 9 16 25
    }
    return 0;
}
```

#### **4.2 STL 提供的函数对象**
STL 提供了许多内置函数对象，如 `std::plus`、`std::minus`、`std::greater` 等。
```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

int main() {
    std::vector<int> vec = {5, 3, 1, 4, 2};
    std::sort(vec.begin(), vec.end(), std::greater<int>());
    for (int i : vec) {
        std::cout << i << " "; // 输出: 5 4 3 2 1
    }
    return 0;
}
```

---

### **5. 迭代器**
#### **5.1 使用迭代器遍历容器**
```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " "; // 输出: 1 2 3 4 5
    }
    return 0;
}
```

#### **5.2 反向迭代器**
```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        std::cout << *it << " "; // 输出: 5 4 3 2 1
    }
    return 0;
}
```

---

### **6. 总结**
- **容器**：用于存储数据，如 `vector`、`list`、`map` 等。
- **迭代器**：用于遍历容器中的元素。
- **算法**：用于操作容器中的数据，如 `sort`、`find`、`accumulate` 等。
- **函数对象**：可以像函数一样调用的对象，如 `std::greater`。

STL 是 C++ 中强大的工具库，能够显著提高开发效率，减少代码重复。熟练掌握 STL 是成为高效 C++ 程序员的关键。


--------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------

C++ 标准模板库（STL）中的 **迭代器（Iterators）** 是一种抽象的概念，用于遍历容器中的元素。迭代器类似于指针，提供了访问和操作容器中元素的方法。STL 中的算法和容器都依赖于迭代器来实现通用性。

---

### **1. 迭代器的分类**
STL 迭代器分为以下几类，按功能从弱到强排列：

#### **1.1 输入迭代器（Input Iterator）**
- 支持读取元素。
- 只能单向遍历（`++` 操作）。
- 例如：`std::istream_iterator`。

#### **1.2 输出迭代器（Output Iterator）**
- 支持写入元素。
- 只能单向遍历（`++` 操作）。
- 例如：`std::ostream_iterator`。

#### **1.3 前向迭代器（Forward Iterator）**
- 支持读取和写入元素。
- 只能单向遍历（`++` 操作）。
- 例如：`std::forward_list` 的迭代器。

#### **1.4 双向迭代器（Bidirectional Iterator）**
- 支持读取和写入元素。
- 支持双向遍历（`++` 和 `--` 操作）。
- 例如：`std::list` 的迭代器。

#### **1.5 随机访问迭代器（Random Access Iterator）**
- 支持读取和写入元素。
- 支持随机访问（`+`、`-`、`[]` 操作）。
- 例如：`std::vector`、`std::deque` 的迭代器。

---

### **2. 常用迭代器操作**
迭代器的基本操作包括：
- `*it`：访问迭代器指向的元素。
- `it->member`：访问迭代器指向的对象的成员。
- `++it`、`it++`：移动到下一个元素。
- `--it`、`it--`：移动到上一个元素（仅限双向和随机访问迭代器）。
- `it1 == it2`、`it1 != it2`：比较两个迭代器是否指向同一位置。
- `it + n`、`it - n`：随机访问（仅限随机访问迭代器）。

---

### **3. 迭代器的使用示例**
#### **3.1 遍历 `vector`**
```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " "; // 输出: 1 2 3 4 5
    }
    return 0;
}
```

#### **3.2 反向遍历 `vector`**
```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        std::cout << *it << " "; // 输出: 5 4 3 2 1
    }
    return 0;
}
```

#### **3.3 使用 `std::find` 查找元素**
```cpp
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto it = std::find(vec.begin(), vec.end(), 3);
    if (it != vec.end()) {
        std::cout << "Found: " << *it << std::endl; // 输出: Found: 3
    }
    return 0;
}
```

---

### **4. 特殊迭代器**
#### **4.1 `std::istream_iterator`**
用于从输入流中读取数据。
```cpp
#include <iostream>
#include <iterator>
#include <vector>

int main() {
    std::vector<int> vec;
    std::istream_iterator<int> input_it(std::cin), eof;
    std::copy(input_it, eof, std::back_inserter(vec));
    for (int i : vec) {
        std::cout << i << " ";
    }
    return 0;
}
```

#### **4.2 `std::ostream_iterator`**
用于向输出流中写入数据。
```cpp
#include <iostream>
#include <iterator>
#include <vector>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::ostream_iterator<int> output_it(std::cout, " ");
    std::copy(vec.begin(), vec.end(), output_it); // 输出: 1 2 3 4 5
    return 0;
}
```

#### **4.3 `std::back_inserter`**
用于在容器尾部插入元素。
```cpp
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>

int main() {
    std::vector<int> vec = {1, 2, 3};
    std::fill_n(std::back_inserter(vec), 3, 10);
    for (int i : vec) {
        std::cout << i << " "; // 输出: 1 2 3 10 10 10
    }
    return 0;
}
```

---

### **5. 迭代器适配器**
迭代器适配器是对现有迭代器的封装，提供了额外的功能。

#### **5.1 `std::reverse_iterator`**
反向遍历容器。
```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        std::cout << *it << " "; // 输出: 5 4 3 2 1
    }
    return 0;
}
```

#### **5.2 `std::move_iterator`**
将元素移动到另一个容器。
```cpp
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>

int main() {
    std::vector<int> src = {1, 2, 3, 4, 5};
    std::vector<int> dst;
    std::copy(std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()), std::back_inserter(dst));
    for (int i : dst) {
        std::cout << i << " "; // 输出: 1 2 3 4 5
    }
    return 0;
}
```

---

### **6. 总结**
- 迭代器是 STL 的核心概念，用于遍历和操作容器中的元素。
- 迭代器分为输入、输出、前向、双向和随机访问迭代器。
- 特殊迭代器（如 `std::istream_iterator`、`std::ostream_iterator`）提供了与流交互的功能。
- 迭代器适配器（如 `std::reverse_iterator`、`std::move_iterator`）扩展了迭代器的功能。

熟练掌握迭代器的使用是高效使用 STL 的关键。

--------------------------------------------------------------------------------------------------


C++ 中的 **异常处理** 是一种机制，用于处理程序运行时可能发生的错误或异常情况。通过异常处理，可以将正常的程序逻辑与错误处理逻辑分离，提高代码的可读性和可维护性。

C++ 异常处理的核心关键字包括：
- `try`：定义可能抛出异常的代码块。
- `catch`：捕获并处理异常。
- `throw`：抛出异常。

---

### **1. 基本语法**
```cpp
try {
    // 可能抛出异常的代码
    if (error_condition) {
        throw exception_object; // 抛出异常
    }
} catch (exception_type &e) {
    // 处理异常
    std::cerr << "Exception caught: " << e.what() << std::endl;
}
```

---

### **2. 异常处理示例**
#### **2.1 抛出和捕获基本类型异常**
```cpp
#include <iostream>

int main() {
    try {
        int age = -1;
        if (age < 0) {
            throw "Age cannot be negative!"; // 抛出字符串异常
        }
    } catch (const char *e) {
        std::cerr << "Error: " << e << std::endl; // 输出: Error: Age cannot be negative!
    }
    return 0;
}
```

#### **2.2 抛出和捕获标准异常**
C++ 标准库提供了许多异常类，如 `std::exception`、`std::runtime_error` 等。
```cpp
#include <iostream>
#include <stdexcept>

int main() {
    try {
        int age = -1;
        if (age < 0) {
            throw std::runtime_error("Age cannot be negative!"); // 抛出标准异常
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl; // 输出: Exception caught: Age cannot be negative!
    }
    return 0;
}
```

#### **2.3 捕获多种异常**
可以使用多个 `catch` 块捕获不同类型的异常。
```cpp
#include <iostream>
#include <stdexcept>

int main() {
    try {
        int age = -1;
        if (age < 0) {
            throw std::runtime_error("Age cannot be negative!");
        }
    } catch (const std::runtime_error &e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Generic exception: " << e.what() << std::endl;
    }
    return 0;
}
```

---

### **3. 自定义异常类**
可以通过继承 `std::exception` 或直接定义类来创建自定义异常。
```cpp
#include <iostream>
#include <stdexcept>

class MyException : public std::exception {
public:
    const char *what() const noexcept override {
        return "My custom exception!";
    }
};

int main() {
    try {
        throw MyException(); // 抛出自定义异常
    } catch (const MyException &e) {
        std::cerr << "Caught: " << e.what() << std::endl; // 输出: Caught: My custom exception!
    }
    return 0;
}
```

---

### **4. 异常传播**
如果异常在函数内部没有被捕获，它会传播到调用该函数的代码中。
```cpp
#include <iostream>
#include <stdexcept>

void riskyFunction() {
    throw std::runtime_error("Something went wrong!");
}

int main() {
    try {
        riskyFunction();
    } catch (const std::exception &e) {
        std::cerr << "Caught in main: " << e.what() << std::endl; // 输出: Caught in main: Something went wrong!
    }
    return 0;
}
```

---

### **5. 栈展开（Stack Unwinding）**
当异常被抛出时，C++ 会沿着调用栈向上查找匹配的 `catch` 块，并在此过程中销毁局部对象（调用析构函数）。
```cpp
#include <iostream>
#include <stdexcept>

class Resource {
public:
    Resource() { std::cout << "Resource acquired\n"; }
    ~Resource() { std::cout << "Resource released\n"; }
};

void riskyFunction() {
    Resource res;
    throw std::runtime_error("Error in riskyFunction!");
}

int main() {
    try {
        riskyFunction();
    } catch (const std::exception &e) {
        std::cerr << "Caught: " << e.what() << std::endl;
    }
    return 0;
}
```

输出：
```
Resource acquired
Resource released
Caught: Error in riskyFunction!
```

---

### **6. `noexcept` 关键字**
`noexcept` 用于指示函数不会抛出异常。如果标记为 `noexcept` 的函数抛出了异常，程序会直接终止。
```cpp
#include <iostream>

void safeFunction() noexcept {
    std::cout << "This function is safe!\n";
}

int main() {
    safeFunction();
    return 0;
}
```

---

### **7. 异常处理的最佳实践**
1. **避免滥用异常**：异常处理适用于处理罕见的、不可恢复的错误，不应用于控制程序流程。
2. **捕获特定异常**：尽量捕获特定类型的异常，而不是通用的 `std::exception`。
3. **资源管理**：使用 RAII（资源获取即初始化）模式管理资源，确保异常发生时资源能够正确释放。
4. **避免抛出析构函数中的异常**：析构函数中的异常可能导致程序终止。

---

### **8. 总结**
- 异常处理通过 `try`、`catch` 和 `throw` 实现。
- 可以抛出和捕获基本类型、标准异常或自定义异常。
- 异常会沿着调用栈传播，直到被捕获。
- 使用 `noexcept` 标记不会抛出异常的函数。
- 遵循最佳实践，确保代码的健壮性和可维护性。

通过合理使用异常处理，可以有效地管理程序中的错误和异常情况。

--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------



### **4. 多线程内存模型**
C++11 引入了多线程内存模型，定义了多线程环境下的内存访问行为。

#### **4.1 内存顺序（Memory Order）**
C++ 提供了以下内存顺序选项：
- `memory_order_relaxed`：无同步或顺序约束。
- `memory_order_acquire`：确保当前操作之前的所有读操作不会被重排序。
- `memory_order_release`：确保当前操作之后的所有写操作不会被重排序。
- `memory_order_seq_cst`：最强的顺序约束，所有操作按顺序执行。

示例：
```cpp
#include <atomic>
#include <thread>
#include <iostream>

std::atomic<int> x(0);
std::atomic<int> y(0);

void thread1() {
    x.store(1, std::memory_order_relaxed);
    y.store(1, std::memory_order_release);
}

void thread2() {
    while (y.load(std::memory_order_acquire) != 1) {}
    std::cout << x.load(std::memory_order_relaxed) << std::endl; // 输出: 1
}

int main() {
    std::thread t1(thread1);
    std::thread t2(thread2);
    t1.join();
    t2.join();
    return 0;
}
```

#### **4.2 原子操作**
原子操作是不可分割的操作，确保在多线程环境下的正确性。
```cpp
#include <atomic>
#include <thread>
#include <iostream>

std::atomic<int> counter(0);

void increment() {
    for (int i = 0; i < 1000; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    std::cout << "Counter: " << counter << std::endl; // 输出: Counter: 2000
    return 0;
}
```

---

#### **5.2 内存池**
内存池是一种优化技术，用于减少频繁动态内存分配的开销。

---

### **6. 总结**
- C++ 的内存模型包括栈、堆、全局/静态存储区、常量存储区和代码区。
- 对象生命周期由其存储位置决定。
- 内存对齐可以提高访问效率。
- 多线程内存模型定义了内存访问行为，原子操作确保线程安全。
- 智能指针和内存池是常用的内存管理工具。

理解 C++ 的内存模型有助于编写高效、安全的程序，尤其是在多线程环境下。

--------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------


#### **1.3 `std::unique_lock`**
- 比 `std::lock_guard` 更灵活，支持手动加锁和解锁。
- 适用于需要延迟加锁或条件变量的场景。

示例：
```cpp
void increment() {
    for (int i = 0; i < 1000; ++i) {
        std::unique_lock<std::mutex> lock(mtx); // 自动加锁和解锁
        ++sharedData;
        lock.unlock(); // 手动解锁
    }
}
```

---

### **2. 读写锁（Read-Write Lock）**
读写锁允许多个线程同时读取共享资源，但写操作需要独占访问。

#### **2.1 `std::shared_mutex`**
- C++17 引入的读写锁。
- 使用 `lock_shared()` 和 `unlock_shared()` 进行读锁定。
- 使用 `lock()` 和 `unlock()` 进行写锁定。

示例：
```cpp
#include <iostream>
#include <thread>
#include <shared_mutex>

std::shared_mutex rwMutex;
int sharedData = 0;

void readData() {
    std::shared_lock<std::shared_mutex> lock(rwMutex); // 读锁定
    std::cout << "Read Data: " << sharedData << std::endl;
}

void writeData() {
    std::unique_lock<std::shared_mutex> lock(rwMutex); // 写锁定
    ++sharedData;
    std::cout << "Write Data: " << sharedData << std::endl;
}

int main() {
    std::thread t1(readData);
    std::thread t2(writeData);
    t1.join();
    t2.join();
    return 0;
}
```

---

### **3. 条件变量（Condition Variable）**
条件变量用于线程间的同步，允许线程等待某个条件成立。

#### **3.1 `std::condition_variable`**
- 与 `std::mutex` 配合使用。
- 使用 `wait()` 等待条件，`notify_one()` 或 `notify_all()` 通知等待的线程。

示例：
```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void waitForReady() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return ready; }); // 等待条件成立
    std::cout << "Ready!" << std::endl;
}

void setReady() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }
    cv.notify_one(); // 通知等待的线程
}

int main() {
    std::thread t1(waitForReady);
    std::thread t2(setReady);
    t1.join();
    t2.join();
    return 0;
}
```

---

### **4. 死锁与避免**
死锁是指多个线程互相等待对方释放锁，导致程序无法继续执行。

#### **4.1 死锁示例**
```cpp
std::mutex mtx1, mtx2;

void thread1() {
    mtx1.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mtx2.lock(); // 等待 mtx2
    mtx2.unlock();
    mtx1.unlock();
}

void thread2() {
    mtx2.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mtx1.lock(); // 等待 mtx1
    mtx1.unlock();
    mtx2.unlock();
}

int main() {
    std::thread t1(thread1);
    std::thread t2(thread2);
    t1.join();
    t2.join();
    return 0;
}
```

#### **4.2 避免死锁**
- 按固定顺序加锁。
- 使用 `std::lock()` 同时锁定多个互斥锁。

示例：
```cpp
void thread1() {
    std::lock(mtx1, mtx2); // 同时锁定
    std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
    // 操作共享资源
}

void thread2() {
    std::lock(mtx1, mtx2); // 同时锁定
    std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
    // 操作共享资源
}
```

---

### **5. 总结**
- **互斥锁**：`std::mutex`、`std::lock_guard`、`std::unique_lock`。
- **读写锁**：`std::shared_mutex`。
- **条件变量**：`std::condition_variable`。
- **死锁**：通过固定顺序加锁或 `std::lock()` 避免。

合理使用锁可以确保多线程程序的正确性和性能。


--------------------------------------------------------------------------------------------------

在 C++ 中，**`std::mutex`** 是标准库提供的一种互斥锁（Mutex），用于保护共享资源，避免多个线程同时访问导致的数据竞争问题。`std::mutex` 是 C++11 引入的，是多线程编程中最基本的同步工具之一。

---

### **1. `std::mutex` 的基本用法**
`std::mutex` 提供了以下主要方法：
- `lock()`：加锁，如果锁已被其他线程持有，则当前线程会阻塞。
- `unlock()`：解锁，释放锁。
- `try_lock()`：尝试加锁，如果锁已被其他线程持有，则立即返回 `false`，否则返回 `true`。

#### **1.1 手动加锁和解锁**
```cpp
#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;
int sharedData = 0;

void increment() {
    for (int i = 0; i < 1000; ++i) {
        mtx.lock(); // 加锁
        ++sharedData;
        mtx.unlock(); // 解锁
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    std::cout << "Shared Data: " << sharedData << std::endl; // 输出: Shared Data: 2000
    return 0;
}
```

#### **1.2 使用 `try_lock()`**
```cpp
void increment() {
    for (int i = 0; i < 1000; ++i) {
        while (!mtx.try_lock()) { // 尝试加锁
            std::this_thread::yield(); // 让出 CPU
        }
        ++sharedData;
        mtx.unlock(); // 解锁
    }
}
```

---

### **2. 自动管理锁**
为了避免忘记解锁或异常情况下未解锁，C++ 提供了 `std::lock_guard` 和 `std::unique_lock` 来自动管理锁的生命周期。

#### **2.1 `std::lock_guard`**
- 在构造时加锁，在析构时解锁。
- 适用于简单的加锁场景。

示例：
```cpp
void increment() {
    for (int i = 0; i < 1000; ++i) {
        std::lock_guard<std::mutex> lock(mtx); // 自动加锁和解锁
        ++sharedData;
    }
}
```

#### **2.2 `std::unique_lock`**
- 比 `std::lock_guard` 更灵活，支持手动加锁和解锁。
- 适用于需要延迟加锁或条件变量的场景。

示例：
```cpp
void increment() {
    for (int i = 0; i < 1000; ++i) {
        std::unique_lock<std::mutex> lock(mtx); // 自动加锁
        ++sharedData;
        lock.unlock(); // 手动解锁
    }
}
```

---

### **3. 死锁问题**
死锁是指多个线程互相等待对方释放锁，导致程序无法继续执行。使用 `std::mutex` 时需要注意避免死锁。

#### **3.1 死锁示例**
```cpp
std::mutex mtx1, mtx2;

void thread1() {
    mtx1.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mtx2.lock(); // 等待 mtx2
    mtx2.unlock();
    mtx1.unlock();
}

void thread2() {
    mtx2.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mtx1.lock(); // 等待 mtx1
    mtx1.unlock();
    mtx2.unlock();
}

int main() {
    std::thread t1(thread1);
    std::thread t2(thread2);
    t1.join();
    t2.join();
    return 0;
}
```

#### **3.2 避免死锁**
- 按固定顺序加锁。
- 使用 `std::lock()` 同时锁定多个互斥锁。

示例：
```cpp
void thread1() {
    std::lock(mtx1, mtx2); // 同时锁定
    std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
    // 操作共享资源
}

void thread2() {
    std::lock(mtx1, mtx2); // 同时锁定
    std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
    // 操作共享资源
}
```

---

### **4. 递归互斥锁 `std::recursive_mutex`**
`std::recursive_mutex` 是一种特殊的互斥锁，允许同一个线程多次加锁。

#### **4.1 使用场景**
- 当某个函数可能递归调用自身，并且需要加锁时。

示例：
```cpp
#include <iostream>
#include <thread>
#include <mutex>

std::recursive_mutex rmtx;

void recursiveFunction(int n) {
    std::lock_guard<std::recursive_mutex> lock(rmtx); // 递归加锁
    if (n > 0) {
        std::cout << "n = " << n << std::endl;
        recursiveFunction(n - 1);
    }
}

int main() {
    std::thread t(recursiveFunction, 3);
    t.join();
    return 0;
}
```

---

### **5. 超时互斥锁 `std::timed_mutex`**
`std::timed_mutex` 是一种支持超时加锁的互斥锁，提供了 `try_lock_for()` 和 `try_lock_until()` 方法。

#### **5.1 使用 `try_lock_for()`**
```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::timed_mutex tmtx;

void tryLockFunction() {
    if (tmtx.try_lock_for(std::chrono::milliseconds(100))) { // 尝试加锁，最多等待 100ms
        std::cout << "Lock acquired!" << std::endl;
        tmtx.unlock();
    } else {
        std::cout << "Failed to acquire lock!" << std::endl;
    }
}

int main() {
    std::thread t1(tryLockFunction);
    std::thread t2(tryLockFunction);
    t1.join();
    t2.join();
    return 0;
}
```

---

### **6. 总结**
- `std::mutex` 是 C++ 中最基本的互斥锁，用于保护共享资源。
- 使用 `std::lock_guard` 或 `std::unique_lock` 可以自动管理锁的生命周期。
- 避免死锁的方法包括按固定顺序加锁和使用 `std::lock()`。
- `std::recursive_mutex` 允许同一个线程多次加锁。
- `std::timed_mutex` 支持超时加锁。

合理使用 `std::mutex` 可以确保多线程程序的正确性和性能。

--------------------------------------------------------------------------------------------------



### **3. 线程同步**
多线程访问共享资源时，需要使用同步机制（如互斥锁、条件变量）避免数据竞争。

#### **3.1 使用 `std::mutex` 保护共享资源**
```cpp
#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;
int sharedData = 0;

void increment() {
    for (int i = 0; i < 1000; ++i) {
        std::lock_guard<std::mutex> lock(mtx); // 自动加锁和解锁
        ++sharedData;
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    std::cout << "Shared Data: " << sharedData << std::endl; // 输出: Shared Data: 2000
    return 0;
}
```

#### **3.2 使用 `std::condition_variable` 实现线程间通信**
```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void waitForReady() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return ready; }); // 等待条件成立
    std::cout << "Ready!" << std::endl;
}

void setReady() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }
    cv.notify_one(); // 通知等待的线程
}

int main() {
    std::thread t1(waitForReady);
    std::thread t2(setReady);
    t1.join();
    t2.join();
    return 0;
}
```

---

### **4. 线程局部存储**
使用 `thread_local` 关键字声明线程局部变量，每个线程拥有独立的变量副本。

#### **4.1 使用 `thread_local`**
```cpp
#include <iostream>
#include <thread>

thread_local int threadLocalData = 0;

void threadFunction(int id) {
    threadLocalData = id;
    std::cout << "Thread " << id << " has data: " << threadLocalData << std::endl;
}

int main() {
    std::thread t1(threadFunction, 1);
    std::thread t2(threadFunction, 2);
    t1.join();
    t2.join();
    return 0;
}
```

---

### **5. 线程池**
C++ 标准库没有直接提供线程池，但可以使用第三方库（如 [ThreadPool](https://github.com/progschj/ThreadPool)）或手动实现。

#### **5.1 简单线程池实现**
```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return !tasks.empty() || stop; });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template <class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;
};

int main() {
    ThreadPool pool(4);

    for (int i = 0; i < 8; ++i) {
        pool.enqueue([i] {
            std::cout << "Task " << i << " is running on thread " << std::this_thread::get_id() << std::endl;
        });
    }

    return 0;
}
```

---

### **6. 总结**
- 使用 `std::thread` 创建和管理线程。
- 使用 `join()` 或 `detach()` 控制线程的生命周期。
- 使用 `std::mutex` 和 `std::condition_variable` 实现线程同步。
- 使用 `thread_local` 声明线程局部变量。
- 线程池可以提高多线程任务的效率。

通过合理使用线程，可以编写出高效、并发的 C++ 程序。


--------------------------------------------------------------------------------------------------

在 C++ 中，**函数** 是程序的基本构建块，用于封装可重用的代码逻辑。函数可以接受输入参数，执行特定任务，并返回结果。以下是关于 C++ 函数的详细说明和示例：

---

### **1. 函数的基本语法**
```cpp
返回类型 函数名(参数列表) {
    // 函数体
    return 返回值; // 如果返回类型不是 void
}
```

- **返回类型**：函数返回值的类型（如 `int`、`double`、`void` 等）。
- **函数名**：函数的名称，用于调用函数。
- **参数列表**：函数接受的输入参数，用逗号分隔。
- **函数体**：函数的具体实现代码。
- **返回值**：函数执行后返回的结果（如果返回类型不是 `void`）。

---

### **2. 示例：简单的函数**
```cpp
#include <iostream>
using namespace std;

// 定义一个函数，计算两个整数的和
int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(3, 5); // 调用函数
    cout << "Sum: " << result << endl;
    return 0;
}
```

**输出**：
```
Sum: 8
```

---

### **3. 函数的组成部分**
#### **3.1 返回类型**
- 如果函数不需要返回值，返回类型为 `void`。
- 如果函数需要返回值，返回类型可以是任意数据类型（如 `int`、`double`、`string` 等）。

#### **3.2 参数列表**
- 参数是函数的输入，可以有零个或多个。
- 每个参数包括类型和名称，用逗号分隔。

#### **3.3 函数体**
- 函数体是函数的具体实现代码。
- 如果返回类型不是 `void`，必须使用 `return` 语句返回一个值。

---

### **4. 函数的调用**
通过函数名和参数列表调用函数。例如：
```cpp
int result = add(3, 5); // 调用 add 函数
```

---

### **5. 函数的重载**
C++ 支持函数重载，即多个函数可以具有相同的名称，但参数列表不同。

```cpp
#include <iostream>
using namespace std;

// 重载 add 函数
int add(int a, int b) {
    return a + b;
}

double add(double a, double b) {
    return a + b;
}

int main() {
    cout << "Sum (int): " << add(3, 5) << endl;
    cout << "Sum (double): " << add(3.5, 5.2) << endl;
    return 0;
}
```

**输出**：
```
Sum (int): 8
Sum (double): 8.7
```

---

### **6. 默认参数**
可以为函数的参数指定默认值。调用函数时，如果未提供该参数，则使用默认值。

```cpp
#include <iostream>
using namespace std;

// 默认参数
void printMessage(string message = "Hello, World!") {
    cout << message << endl;
}

int main() {
    printMessage(); // 使用默认参数
    printMessage("Custom Message"); // 使用自定义参数
    return 0;
}
```

**输出**：
```
Hello, World!
Custom Message
```

---

### **7. 内联函数**
使用 `inline` 关键字可以将函数声明为内联函数，编译器会尝试将函数调用替换为函数体，以减少函数调用的开销。

```cpp
#include <iostream>
using namespace std;

// 内联函数
inline int square(int x) {
    return x * x;
}

int main() {
    cout << "Square of 5: " << square(5) << endl;
    return 0;
}
```

**输出**：
```
Square of 5: 25
```

---

### **8. 递归函数**
函数可以调用自身，称为递归函数。

```cpp
#include <iostream>
using namespace std;

// 递归函数：计算阶乘
int factorial(int n) {
    if (n == 0 || n == 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int main() {
    cout << "Factorial of 5: " << factorial(5) << endl;
    return 0;
}
```

**输出**：
```
Factorial of 5: 120
```

---

### **9. 函数指针**
函数指针是指向函数的指针变量，可以用于动态调用函数。

```cpp
#include <iostream>
using namespace std;

// 函数
int add(int a, int b) {
    return a + b;
}

int main() {
    // 声明函数指针
    int (*funcPtr)(int, int) = add;

    // 使用函数指针调用函数
    int result = funcPtr(3, 5);
    cout << "Sum: " << result << endl;

    return 0;
}
```

**输出**：
```
Sum: 8
```

---

### **10. Lambda 表达式**
C++11 引入了 Lambda 表达式，用于定义匿名函数。

```cpp
#include <iostream>
using namespace std;

int main() {
    // Lambda 表达式
    auto add = [](int a, int b) -> int {
        return a + b;
    };

    cout << "Sum: " << add(3, 5) << endl;
    return 0;
}
```

**输出**：
```
Sum: 8
```

---

### **11. 总结**
- 函数是 C++ 中封装代码逻辑的基本单元。
- 函数可以接受参数并返回值。
- 支持函数重载、默认参数、内联函数、递归函数、函数指针和 Lambda 表达式。
- 合理使用函数可以提高代码的可读性、可维护性和复用性。

通过掌握这些知识，你可以在 C++ 中编写高效、灵活的代码！

--------------------------------------------------------------------------------------------------

在 C++ 中，**指针** 是一个非常重要的概念，它用于直接操作内存地址。指针可以指向变量、数组、函数等，是 C++ 中实现动态内存管理、高效数据操作和复杂数据结构的基础。以下是关于 C++ 指针的详细说明和示例：

---

### **1. 指针的基本概念**
指针是一个变量，其值是另一个变量的内存地址。通过指针，可以直接访问和操作内存中的数据。

#### **1.1 指针的声明**
```cpp
数据类型 *指针变量名;
```
- `数据类型`：指针指向的变量的类型（如 `int`、`double` 等）。
- `*`：表示这是一个指针变量。
- `指针变量名`：指针的名称。

#### **1.2 示例**
```cpp
int *ptr; // 声明一个指向 int 类型的指针
```

---

### **2. 指针的基本操作**
#### **2.1 取地址运算符 `&`**
`&` 用于获取变量的内存地址。

```cpp
int num = 10;
int *ptr = &num; // ptr 指向 num 的地址
```

#### **2.2 解引用运算符 `*`**
`*` 用于访问指针指向的内存地址中的值。

```cpp
int num = 10;
int *ptr = &num;
cout << *ptr; // 输出 10
```

#### **2.3 示例**
```cpp
#include <iostream>
using namespace std;

int main() {
    int num = 42;
    int *ptr = &num; // ptr 指向 num 的地址

    cout << "Value of num: " << num << endl;
    cout << "Address of num: " << &num << endl;
    cout << "Value of ptr: " << ptr << endl;
    cout << "Value pointed by ptr: " << *ptr << endl;

    return 0;
}
```

**输出**：
```
Value of num: 42
Address of num: 0x7ffee4b5c9ac
Value of ptr: 0x7ffee4b5c9ac
Value pointed by ptr: 42
```

---

### **3. 指针的初始化**
指针在使用前必须初始化，否则会指向一个未知的内存地址，可能导致程序崩溃。

```cpp
int *ptr = nullptr; // 初始化为空指针
```

---

### **4. 指针与数组**
数组名本身就是一个指针，指向数组的第一个元素。

#### **4.1 示例**
```cpp
#include <iostream>
using namespace std;

int main() {
    int arr[3] = {10, 20, 30};
    int *ptr = arr; // ptr 指向数组的第一个元素

    for (int i = 0; i < 3; i++) {
        cout << "Element " << i << ": " << *(ptr + i) << endl;
    }

    return 0;
}
```

**输出**：
```
Element 0: 10
Element 1: 20
Element 2: 30
```

---

### **5. 指针与函数**
指针可以作为函数的参数或返回值，用于传递或返回内存地址。

#### **5.1 指针作为函数参数**
```cpp
#include <iostream>
using namespace std;

void increment(int *ptr) {
    (*ptr)++; // 修改指针指向的值
}

int main() {
    int num = 10;
    increment(&num); // 传递 num 的地址
    cout << "Incremented value: " << num << endl;
    return 0;
}
```

**输出**：
```
Incremented value: 11
```

#### **5.2 指针作为函数返回值**
```cpp
#include <iostream>
using namespace std;

int* getMax(int *a, int *b) {
    return (*a > *b) ? a : b;
}

int main() {
    int x = 10, y = 20;
    int *maxPtr = getMax(&x, &y);
    cout << "Max value: " << *maxPtr << endl;
    return 0;
}
```

**输出**：
```
Max value: 20
```

---

### **6. 动态内存管理**
C++ 使用 `new` 和 `delete` 运算符动态分配和释放内存。

#### **6.1 动态分配内存**
```cpp
int *ptr = new int; // 动态分配一个 int 类型的内存
*ptr = 42; // 赋值
```

#### **6.2 动态分配数组**
```cpp
int *arr = new int[5]; // 动态分配一个包含 5 个 int 的数组
for (int i = 0; i < 5; i++) {
    arr[i] = i + 1;
}
```

#### **6.3 释放内存**
```cpp
delete ptr; // 释放单个变量
delete[] arr; // 释放数组
```

---

### **7. 指针与常量**
指针可以与 `const` 关键字结合，表示指针指向的值或指针本身不可修改。

#### **7.1 指向常量的指针**
```cpp
const int *ptr; // ptr 指向的值不可修改
```

#### **7.2 常量指针**
```cpp
int *const ptr = &num; // ptr 本身不可修改
```

#### **7.3 指向常量的常量指针**
```cpp
const int *const ptr = &num; // ptr 和 ptr 指向的值都不可修改
```

---

### **8. 指针的高级用法**
#### **8.1 指针的指针**
指针可以指向另一个指针。

```cpp
int num = 10;
int *ptr = &num;
int **ptr2 = &ptr; // ptr2 指向 ptr
```

#### **8.2 函数指针**
指针可以指向函数，用于动态调用函数。

```cpp
#include <iostream>
using namespace std;

int add(int a, int b) {
    return a + b;
}

int main() {
    int (*funcPtr)(int, int) = add; // 函数指针
    cout << "Sum: " << funcPtr(3, 5) << endl;
    return 0;
}
```

---

### **9. 总结**
- 指针是 C++ 中直接操作内存地址的工具。
- 指针可以指向变量、数组、函数等。
- 指针与动态内存管理、函数参数传递、复杂数据结构等密切相关。
- 使用指针时需要小心，避免空指针、野指针和内存泄漏等问题。

通过掌握指针的使用，你可以编写更高效、灵活的 C++ 程序！

--------------------------------------------------------------------------------------------------

在 C++ 中，**引用** 是一种别名机制，它为已存在的变量提供了一个新的名称。引用与指针类似，但更安全且易于使用。引用在函数参数传递、返回值优化等场景中非常有用。以下是关于 C++ 引用的详细说明和示例：

---

### **1. 引用的基本概念**
引用是一个变量的别名，它必须在声明时初始化，并且一旦绑定到一个变量后，就不能再绑定到其他变量。

#### **1.1 引用的声明**
```cpp
数据类型 &引用名 = 变量名;
```
- `数据类型`：引用绑定的变量的类型。
- `&`：表示这是一个引用。
- `引用名`：引用的名称。
- `变量名`：引用绑定的变量。

#### **1.2 示例**
```cpp
int num = 10;
int &ref = num; // ref 是 num 的引用
```

---

### **2. 引用的特性**
#### **2.1 引用必须初始化**
引用在声明时必须绑定到一个变量，否则会编译错误。

```cpp
int num = 10;
int &ref = num; // 正确
int &ref2;      // 错误：引用必须初始化
```

#### **2.2 引用不能重新绑定**
引用一旦绑定到一个变量后，就不能再绑定到其他变量。

```cpp
int num1 = 10, num2 = 20;
int &ref = num1;
ref = num2; // 这是赋值操作，不是重新绑定
```

#### **2.3 引用是变量的别名**
通过引用可以访问和修改原变量的值。

```cpp
int num = 10;
int &ref = num;
ref = 20; // 修改 ref 的值
cout << num; // 输出 20
```

---

### **3. 引用与指针的区别**
| 特性               | 引用                          | 指针                          |
|--------------------|-------------------------------|-------------------------------|
| 初始化             | 必须初始化                    | 可以不初始化                  |
| 重新绑定           | 不能重新绑定                  | 可以重新指向其他变量          |
| 空值               | 不能为空                      | 可以为空（`nullptr`）         |
| 操作符             | 使用 `.` 访问成员              | 使用 `->` 访问成员            |
| 语法               | 更简洁                        | 更灵活                        |

---

### **4. 引用作为函数参数**
引用常用于函数参数传递，可以避免拷贝大对象，同时允许函数修改实参的值。

#### **4.1 示例**
```cpp
#include <iostream>
using namespace std;

void increment(int &ref) {
    ref++; // 修改引用的值
}

int main() {
    int num = 10;
    increment(num); // 传递 num 的引用
    cout << "Incremented value: " << num << endl;
    return 0;
}
```

**输出**：
```
Incremented value: 11
```

---

### **5. 引用作为函数返回值**
函数可以返回引用，但必须确保返回的引用指向的变量在函数调用结束后仍然有效。

#### **5.1 示例**
```cpp
#include <iostream>
using namespace std;

int &getMax(int &a, int &b) {
    return (a > b) ? a : b;
}

int main() {
    int x = 10, y = 20;
    int &maxRef = getMax(x, y); // maxRef 是 x 或 y 的引用
    maxRef = 30; // 修改最大值
    cout << "x: " << x << ", y: " << y << endl;
    return 0;
}
```

**输出**：
```
x: 10, y: 30
```

---

### **6. 常量引用**
常量引用用于防止函数修改实参的值，同时避免拷贝大对象。

#### **6.1 示例**
```cpp
#include <iostream>
using namespace std;

void printValue(const int &ref) {
    cout << "Value: " << ref << endl;
    // ref = 10; // 错误：不能修改常量引用
}

int main() {
    int num = 42;
    printValue(num); // 传递 num 的常量引用
    return 0;
}
```

**输出**：
```
Value: 42
```

---

### **7. 引用与数组**
引用可以绑定到数组，但语法较为特殊。

#### **7.1 示例**
```cpp
#include <iostream>
using namespace std;

int main() {
    int arr[3] = {10, 20, 30};
    int (&ref)[3] = arr; // ref 是数组 arr 的引用

    for (int i = 0; i < 3; i++) {
        cout << ref[i] << " ";
    }

    return 0;
}
```

**输出**：
```
10 20 30
```

---

### **8. 引用与范围 for 循环**
引用可以用于范围 for 循环，直接修改容器中的元素。

#### **8.1 示例**
```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> vec = {1, 2, 3, 4, 5};

    for (int &ref : vec) {
        ref *= 2; // 修改容器中的元素
    }

    for (int val : vec) {
        cout << val << " ";
    }

    return 0;
}
```

**输出**：
```
2 4 6 8 10
```

---

### **9. 总结**
- 引用是变量的别名，必须在声明时初始化。
- 引用不能重新绑定，也不能为空。
- 引用常用于函数参数传递和返回值优化。
- 常量引用可以防止函数修改实参的值。
- 引用比指针更安全、更简洁，但在某些场景下指针更灵活。

通过掌握引用的使用，你可以编写更高效、更易读的 C++ 代码！

--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------


在 C++ 中，**继承** 是面向对象编程（OOP）的核心特性之一，它允许一个类（派生类）基于另一个类（基类）创建，从而复用基类的成员并扩展其功能。以下是关于 C++ 继承的详细说明和示例：

---

### **1. 继承的基本概念**
- **基类（父类）**：被继承的类。
- **派生类（子类）**：继承基类的类。
- 派生类可以访问基类的成员（根据访问权限），并可以添加新的成员或重写基类的成员函数。

---

### **2. 继承的语法**
```cpp
class 派生类名 : 访问修饰符 基类名 {
    // 派生类的成员
};
```
- **访问修饰符**：可以是 `public`、`protected` 或 `private`，决定基类成员在派生类中的访问权限。

---

### **3. 继承的类型**
#### **3.1 公有继承（`public`）**
- 基类的 `public` 成员在派生类中仍然是 `public`。
- 基类的 `protected` 成员在派生类中仍然是 `protected`。
- 基类的 `private` 成员在派生类中不可访问。

#### **3.2 保护继承（`protected`）**
- 基类的 `public` 和 `protected` 成员在派生类中都变为 `protected`。
- 基类的 `private` 成员在派生类中不可访问。

#### **3.3 私有继承（`private`）**
- 基类的 `public` 和 `protected` 成员在派生类中都变为 `private`。
- 基类的 `private` 成员在派生类中不可访问。

---

### **4. 继承的示例**
#### **4.1 公有继承**
```cpp
#include <iostream>
using namespace std;

// 基类
class Animal {
public:
    void eat() {
        cout << "Animal is eating." << endl;
    }
};

// 派生类
class Dog : public Animal {
public:
    void bark() {
        cout << "Dog is barking." << endl;
    }
};

int main() {
    Dog dog;
    dog.eat();  // 调用基类的成员函数
    dog.bark(); // 调用派生类的成员函数
    return 0;
}
```

**输出**：
```
Animal is eating.
Dog is barking.
```

#### **4.2 保护继承**
```cpp
#include <iostream>
using namespace std;

// 基类
class Animal {
protected:
    void eat() {
        cout << "Animal is eating." << endl;
    }
};

// 派生类
class Dog : protected Animal {
public:
    void bark() {
        eat(); // 可以访问基类的 protected 成员
        cout << "Dog is barking." << endl;
    }
};

int main() {
    Dog dog;
    dog.bark(); // 调用派生类的成员函数
    // dog.eat(); // 错误：eat() 在派生类中是 protected，外部不可访问
    return 0;
}
```

**输出**：
```
Animal is eating.
Dog is barking.
```

#### **4.3 私有继承**
```cpp
#include <iostream>
using namespace std;

// 基类
class Animal {
public:
    void eat() {
        cout << "Animal is eating." << endl;
    }
};

// 派生类
class Dog : private Animal {
public:
    void bark() {
        eat(); // 可以访问基类的 public 成员
        cout << "Dog is barking." << endl;
    }
};

int main() {
    Dog dog;
    dog.bark(); // 调用派生类的成员函数
    // dog.eat(); // 错误：eat() 在派生类中是 private，外部不可访问
    return 0;
}
```

**输出**：
```
Animal is eating.
Dog is barking.
```

---

### **5. 多重继承**
C++ 支持多重继承，即一个派生类可以从多个基类继承。

#### **5.1 示例**
```cpp
#include <iostream>
using namespace std;

// 基类 1
class Animal {
public:
    void eat() {
        cout << "Animal is eating." << endl;
    }
};

// 基类 2
class Mammal {
public:
    void breathe() {
        cout << "Mammal is breathing." << endl;
    }
};

// 派生类
class Dog : public Animal, public Mammal {
public:
    void bark() {
        cout << "Dog is barking." << endl;
    }
};

int main() {
    Dog dog;
    dog.eat();    // 调用基类 Animal 的成员函数
    dog.breathe(); // 调用基类 Mammal 的成员函数
    dog.bark();   // 调用派生类的成员函数
    return 0;
}
```

**输出**：
```
Animal is eating.
Mammal is breathing.
Dog is barking.
```

---

### **6. 函数重写与多态**
派生类可以重写基类的成员函数，并通过基类指针或引用实现多态。

#### **6.1 示例**
```cpp
#include <iostream>
using namespace std;

// 基类
class Animal {
public:
    virtual void speak() { // 虚函数
        cout << "Animal speaks." << endl;
    }
};

// 派生类
class Dog : public Animal {
public:
    void speak() override { // 重写基类的虚函数
        cout << "Dog barks." << endl;
    }
};

int main() {
    Animal *animal = new Dog(); // 基类指针指向派生类对象
    animal->speak(); // 调用派生类的重写函数
    delete animal;
    return 0;
}
```

**输出**：
```
Dog barks.
```

---

### **7. 总结**
- 继承是 C++ 中实现代码复用的重要机制。
- 继承类型包括公有继承、保护继承和私有继承。
- 派生类可以访问基类的成员（根据访问权限），并可以重写基类的成员函数。
- 多重继承允许一个派生类从多个基类继承。
- 虚函数和多态性使得基类指针或引用可以调用派生类的重写函数。

通过掌握继承的使用，你可以设计出更灵活、更高效的面向对象程序！

--------------------------------------------------------------------------------------------------


---

### **2. 虚函数**
虚函数是实现动态多态的关键。通过在基类中声明虚函数，派生类可以重写该函数，从而实现多态。

#### **2.1 虚函数的声明**
```cpp
virtual 返回类型 函数名(参数列表);
```

#### **2.2 示例**
```cpp
#include <iostream>
using namespace std;

// 基类
class Animal {
public:
    virtual void speak() { // 虚函数
        cout << "Animal speaks." << endl;
    }
};

// 派生类
class Dog : public Animal {
public:
    void speak() override { // 重写基类的虚函数
        cout << "Dog barks." << endl;
    }
};

// 派生类
class Cat : public Animal {
public:
    void speak() override { // 重写基类的虚函数
        cout << "Cat meows." << endl;
    }
};

int main() {
    Animal *animal1 = new Dog(); // 基类指针指向派生类对象
    Animal *animal2 = new Cat(); // 基类指针指向派生类对象

    animal1->speak(); // 调用 Dog 的 speak()
    animal2->speak(); // 调用 Cat 的 speak()

    delete animal1;
    delete animal2;
    return 0;
}
```

**输出**：
```
Dog barks.
Cat meows.
```

---

### **3. 纯虚函数与抽象类**
- **纯虚函数**：在基类中声明但没有实现的虚函数，派生类必须重写纯虚函数。
- **抽象类**：包含纯虚函数的类，不能实例化，只能作为基类。

#### **3.1 纯虚函数的声明**
```cpp
virtual 返回类型 函数名(参数列表) = 0;
```

#### **3.2 示例**
```cpp
#include <iostream>
using namespace std;

// 抽象类
class Animal {
public:
    virtual void speak() = 0; // 纯虚函数
};

// 派生类
class Dog : public Animal {
public:
    void speak() override {
        cout << "Dog barks." << endl;
    }
};

// 派生类
class Cat : public Animal {
public:
    void speak() override {
        cout << "Cat meows." << endl;
    }
};

int main() {
    Animal *animal1 = new Dog(); // 基类指针指向派生类对象
    Animal *animal2 = new Cat(); // 基类指针指向派生类对象

    animal1->speak(); // 调用 Dog 的 speak()
    animal2->speak(); // 调用 Cat 的 speak()

    delete animal1;
    delete animal2;
    return 0;
}
```

**输出**：
```
Dog barks.
Cat meows.
```



--------------------------------------------------------------------------------------------------

在 C++ 中，**虚函数** 是实现 **多态** 的关键机制。它允许派生类重写基类的函数，并通过基类的指针或引用调用派生类的实现。虚函数的核心在于 **动态绑定**，即在运行时根据对象的实际类型决定调用哪个函数。

以下是关于 C++ 虚函数的详细说明和示例：

---

### **1. 虚函数的基本概念**
- **虚函数**：在基类中使用 `virtual` 关键字声明的函数，派生类可以重写该函数。
- **动态绑定**：在运行时根据对象的实际类型调用相应的函数，而不是根据指针或引用的类型。
- **多态**：通过虚函数实现“一个接口，多种实现”的效果。

---

### **2. 虚函数的声明与使用**
#### **2.1 虚函数的声明**
在基类中使用 `virtual` 关键字声明虚函数：
```cpp
virtual 返回类型 函数名(参数列表);
```

#### **2.2 示例**
```cpp
#include <iostream>
using namespace std;

// 基类
class Animal {
public:
    virtual void speak() { // 虚函数
        cout << "Animal speaks." << endl;
    }
};

// 派生类
class Dog : public Animal {
public:
    void speak() override { // 重写基类的虚函数
        cout << "Dog barks." << endl;
    }
};

// 派生类
class Cat : public Animal {
public:
    void speak() override { // 重写基类的虚函数
        cout << "Cat meows." << endl;
    }
};

int main() {
    Animal *animal1 = new Dog(); // 基类指针指向派生类对象
    Animal *animal2 = new Cat(); // 基类指针指向派生类对象

    animal1->speak(); // 调用 Dog 的 speak()
    animal2->speak(); // 调用 Cat 的 speak()

    delete animal1;
    delete animal2;
    return 0;
}
```

**输出**：
```
Dog barks.
Cat meows.
```

---

### **3. 虚函数的工作原理**
- 当基类中的函数被声明为虚函数时，编译器会为该类生成一个 **虚函数表（vtable）**。
- 虚函数表是一个存储虚函数地址的数组，每个对象在运行时通过虚函数表查找要调用的函数。
- 派生类重写虚函数时，会更新虚函数表中的函数地址。

---

### **4. 纯虚函数与抽象类**
- **纯虚函数**：在基类中声明但没有实现的虚函数，派生类必须重写纯虚函数。
- **抽象类**：包含纯虚函数的类，不能实例化，只能作为基类。

#### **4.1 纯虚函数的声明**
```cpp
virtual 返回类型 函数名(参数列表) = 0;
```

#### **4.2 示例**
```cpp
#include <iostream>
using namespace std;

// 抽象类
class Shape {
public:
    virtual void draw() = 0; // 纯虚函数
};

// 派生类
class Circle : public Shape {
public:
    void draw() override {
        cout << "Drawing a circle." << endl;
    }
};

// 派生类
class Square : public Shape {
public:
    void draw() override {
        cout << "Drawing a square." << endl;
    }
};

int main() {
    Shape *shape1 = new Circle(); // 基类指针指向派生类对象
    Shape *shape2 = new Square(); // 基类指针指向派生类对象

    shape1->draw(); // 调用 Circle 的 draw()
    shape2->draw(); // 调用 Square 的 draw()

    delete shape1;
    delete shape2;
    return 0;
}
```

**输出**：
```
Drawing a circle.
Drawing a square.
```

---

### **5. 虚析构函数**
当基类的指针指向派生类对象时，如果基类的析构函数不是虚函数，则只会调用基类的析构函数，导致派生类的资源泄漏。因此，基类的析构函数应声明为虚函数。

#### **5.1 示例**
```cpp
#include <iostream>
using namespace std;

// 基类
class Base {
public:
    virtual ~Base() { // 虚析构函数
        cout << "Base destructor." << endl;
    }
};

// 派生类
class Derived : public Base {
public:
    ~Derived() {
        cout << "Derived destructor." << endl;
    }
};

int main() {
    Base *ptr = new Derived(); // 基类指针指向派生类对象
    delete ptr; // 调用派生类的析构函数
    return 0;
}
```

**输出**：
```
Derived destructor.
Base destructor.
```

---

### **6. 虚函数的限制**
- 虚函数不能是静态函数（`static`）。
- 构造函数不能是虚函数。
- 虚函数可以是内联函数（`inline`），但一旦被重写，内联特性将失效。

---

### **7. 总结**
- **虚函数** 是实现多态的关键，允许派生类重写基类的函数。
- **动态绑定** 在运行时根据对象的实际类型调用相应的函数。
- **纯虚函数** 和 **抽象类** 用于定义接口，派生类必须实现纯虚函数。
- **虚析构函数** 确保派生类的析构函数被正确调用，避免资源泄漏。

通过掌握虚函数的使用，你可以设计出更灵活、更可扩展的面向对象程序！


--------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------


第 1章 预备知识 1
1．1　C++简介　1
1．2　C++简史　2
1．2．1　C语言　2
1．2．2　C语言编程原理　2
1．2．3　面向对象编程　3
1．2．4　C++和泛型编程　3
1．2．5　C++的起源　4
1．3　可移植性和标准　5
1．3．1　C++的发展　5
1．3．2　本书遵循的C++标准　6
1．4　程序创建的技巧　6
1．4．1　创建源代码文件　6
1．4．2　编译和链接　7
1．5　总结　10
第　2章 开始学习C++　11
2．1　进入C++　11
2．1．1　main( )函数　12
2．1．2　C++注释　14
2．1．3　C++预处理器和iostream文件　14
2．1．4　头文件名　15
2．1．5　名称空间　15
2．1．6　使用cout进行C++输出　16
2．1．7　C++源代码的格式化　17
2．2　C++语句　18
2．2．1　声明语句和变量　19
2．2．2　赋值语句　20
2．2．3　cout的新花样　20
2．3　其他C++语句　21
2．3．1　使用cin　21
2．3．2　使用cout进行拼接　21
2．3．3　类简介　22
2．4　函数　23
2．4．1　使用有返回值的函数　23
2．4．2　函数变体　25
2．4．3　用户定义的函数　26
2．4．4　用户定义的有返回值的函数　28
2．4．5　在多函数程序中使用using编译指令　29
2．5　总结　30
2．6　复习题　30
2．7　编程练习　30
第3章　处理数据　32
3．1　简单变量　32
3．1．1　变量名　33
3．1．2　整型　34
3．1．3　整型short、int、long和long long　34
3．1．4　无符号类型　37
3．1．5　选择整型类型　38
3．1．6　整型字面值　39
3．1．7　C++如何确定常量的类型　40
3．1．8　char类型：字符和小整数　41
3．1．9　bool类型　46
3．2　const限定符　46
3．3　浮点数　47
3．3．1　书写浮点数　47
3．3．2　浮点类型　48
3．3．3　浮点常量　49
3．3．4　浮点数的优缺点　49
3．4　C++算术运算符　50
3．4．1　运算符优先级和结合性　51
3．4．2　除法分支　51
3．4．3　求模运算符　52
3．4．4　类型转换　53
3．4．5　C++11中的auto声明　56
3．5　总结　57
3．6　复习题　57
3．7　编程练习　58
第4章　复合类型　59
4．1　数组　59
4．1．1　程序说明　61
4．1．2　数组的初始化规则　61
4．1．3　C++11数组初始化方法　62
4．2　字符串　62
4．2．1　拼接字符串常量　63
4．2．2　在数组中使用字符串　63
4．2．3　字符串输入　64
4．2．4　每次读取一行字符串输入　65
4．2．5　混合输入字符串和数字　67
4．3　string类简介　68
4．3．1　C++11字符串初始化　69
4．3．2　赋值、拼接和附加　69
4．3．3　string类的其他操作　70
4．3．4　string类I/O　71
4．3．5　其他形式的字符串字面值　72
4．4　结构简介　73
4．4．1　在程序中使用结构　74
4．4．2　C++11结构初始化　75
4．4．3　结构可以将string类作为成员吗　75
4．4．4　其他结构属性　76
4．4．5　结构数组　77
4．4．6　结构中的位字段　78
4．5　共用体　78
4．6　枚举　79
4．6．1　设置枚举量的值　80
4．6．2　枚举的取值范围　80
4．7　指针和自由存储空间　80
4．7．1　声明和初始化指针　82
4．7．2　指针的危险　84
4．7．3　指针和数字　84
4．7．4　使用new来分配内存　84
4．7．5　使用delete释放内存　86
4．7．6　使用new来创建动态数组　86
4．8　指针、数组和指针算术　88
4．8．1　程序说明　89
4．8．2　指针小结　90
4．8．3　指针和字符串　92
4．8．4　使用new创建动态结构　94
4．8．5　自动存储、静态存储和动态存储　96
4．9　类型组合　97
4．10　数组的替代品　99
4．10．1　模板类vector　99
4．10．2　模板类array（C++11）　99
4．10．3　比较数组、vector对象和array对象　99
4．11　总结　101
4．12　复习题　101
4．13　编程练习　102
第5章　循环和关系表达式　104
5．1　for循环　104
5．1．1　for循环的组成部分　105
5．1．2　回到for循环　109
5．1．3　修改步长　110
5．1．4　使用for循环访问字符串　110
5．1．5　递增运算符（++）和递减运算符（ ）　111
5．1．6　副作用和顺序点　112
5．1．7　前缀格式和后缀格式　112
5．1．8　递增/递减运算符和指针　112
5．1．9　组合赋值运算符　113
5．1．10　复合语句（语句块）　113
5．1．11　其他语法技巧——逗号运算符　115
5．1．12　关系表达式　116
5．1．13　赋值、比较和可能犯的错误　117
5．1．14　C-风格字符串的比较　118
5．1．15　比较string类字符串　120
5．2　while循环　120
5．2．1　for与while　121
5．2．2　等待一段时间：编写延时循环　123
5．3　do while循环　124
5．4　基于范围的for循环（C++11）　125
5．5　循环和文本输入　125
5．5．1　使用原始的cin进行输入　126
5．5．2　使用cin．get(char)进行补救　126
5．5．3　使用哪一个cin．get( )　127
5．5．4　文件尾条件　128
5．5．5　另一个cin．get( )版本　129
5．6　嵌套循环和二维数组　131
5．6．1　初始化二维数组　132
5．6．2　使用二维数组　133
5．7　总结　134
5．8　复习题　134
5．9　编程练习　135
第6章　分支语句和逻辑运算符　137
6．1　if语句　137
6．1．1　if else语句　138
6．1．2　格式化if else语句　139
6．1．3　if else if else结构　140
6．2　逻辑表达式　141
6．2．1　逻辑OR运算符：||　141
6．2．2　逻辑AND运算符：&&　142
6．2．3　用&&来设置取值范围　144
6．2．4　逻辑NOT运算符：!　145
6．2．5　逻辑运算符细节　146
6．2．6　其他表示方式　146
6．3　字符函数库cctype　147
6．4　：运算符　148
6．5　switch语句　149
6．5．1　将枚举量用作标签　151
6．5．2　switch和if else　152
6．6　break和continue语句　153
6．7　读取数字的循环　154
6．8　简单文件输入/输出　157
6．8．1　文本I/O和文本文件　157
6．8．2　写入到文本文件中　158
6．8．3　读取文本文件　160
6．9　总结　163
6．10　复习题　163
6．11　编程练习　165
第7章　函数——C++的编程模块　167
7．1　复习函数的基本知识　167
7．1．1　定义函数　168
7．1．2　函数原型和函数调用　169
7．2　函数参数和按值传递　171
7．2．1　多个参数　172
7．2．2　另一个接受两个参数的函数　174
7．3　函数和数组　175
7．3．1　函数如何使用指针来处理数组　176
7．3．2　将数组作为参数意味着什么　176
7．3．3　更多数组函数示例　178
7．3．4　使用数组区间的函数　181
7．3．5　指针和const　182
7．4　函数和二维数组　185
7．5　函数和C-风格字符串　186
7．5．1　将C-风格字符串作为参数的函数　186
7．5．2　返回C-风格字符串的函数　187
7．6　函数和结构　188
7．6．1　传递和返回结构　188
7．6．2　另一个处理结构的函数示例　189
7．6．3　传递结构的地址　192
7．7　函数和string对象　194
7．8　函数与array对象　195
7．9　递归　196
7．9．1　包含一个递归调用的递归　196
7．9．2　包含多个递归调用的递归　197
7．10　函数指针　198
7．10．1　函数指针的基础知识　199
7．10．2　函数指针示例　200
7．10．3　深入探讨函数指针　201
7．10．4　使用typedef进行简化　204
7．11　总结　204
7．12　复习题　205
7．13　编程练习　206
第8章　函数探幽　208
8．1　C++内联函数　208
8．2　引用变量　210
8．2．1　创建引用变量　210
8．2．2　将引用用作函数参数　212
8．2．3　引用的属性和特别之处　214
8．2．4　将引用用于结构　216
8．2．5　将引用用于类对象　220
8．2．6　对象、继承和引用　222
8．2．7　何时使用引用参数　224
8．3　默认参数　225
8．4　函数重载　227
8．4．1　重载示例　228
8．4．2　何时使用函数重载　230
8．5　函数模板　230
8．5．1　重载的模板　232
8．5．2　模板的局限性　233
8．5．3　显式具体化　234
8．5．4　实例化和具体化　236
8．5．5　编译器选择使用哪个函数版本　237
8．5．6　模板函数的发展　241
8．6　总结　243
8．7　复习题　243
8．8　编程练习　244
第9章　内存模型和名称空间　246
9．1　单独编译　246
9．2　存储持续性、作用域和链接性　250
9．2．1　作用域和链接　250
9．2．2　自动存储持续性　250
9．2．3　静态持续变量　253
9．2．4　静态持续性、外部链接性　254
9．2．5　静态持续性、内部链接性　257
9．2．6　静态存储持续性、无链接性　258
9．2．7　说明符和限定符　260
9．2．8　函数和链接性　261
9．2．9　语言链接性　262
9．2．10　存储方案和动态分配　262
9．3　名称空间　266
9．3．1　传统的C++名称空间　266
9．3．2　新的名称空间特性　267
9．3．3　名称空间示例　271
9．3．4　名称空间及其前途　274
9．4　总结　274
9．5　复习题　275
9．6　编程练习　276
第　10章 对象和类　278
10．1　过程性编程和面向对象编程　278
10．2　抽象和类　279
10．2．1　类型是什么　279
10．2．2　C++中的类　280
10．2．3　实现类成员函数　283
10．2．4　使用类　285
10．2．5　修改实现　286
10．2．6　小结　287
10．3　类的构造函数和析构函数　288
10．3．1　声明和定义构造函数　288
10．3．2　使用构造函数　289
10．3．3　默认构造函数　290
10．3．4　析构函数　290
10．3．5　改进Stock类　291
10．3．6　构造函数和析构函数小结　296
10．4　this指针　296
10．5　对象数组　300
10．6　类作用域　302
10．6．1　作用域为类的常量　302
10．6．2　作用域内枚举（C++11）　303
10．7　抽象数据类型　304
10．8　总结　307
10．9　复习题　307
10．10　编程练习　308
第　11章 使用类　310
11．1　运算符重载　310
11．2　计算时间：一个运算符重载示例　311
11．2．1　添加加法运算符　313
11．2．2　重载限制　315
11．2．3　其他重载运算符　316
11．3　友元　318
11．3．1　创建友元　319
11．3．2　常用的友元：重载<<运算符　320
11．4　重载运算符：作为成员函数还是非成员函数　324
11．5　再谈重载：一个矢量类　324
11．5．1　使用状态成员　329
11．5．2　为Vector类重载算术运算符　331
11．5．3　对实现的说明　332
11．5．4　使用Vector类来模拟随机漫步　332
11．6　类的自动转换和强制类型转换　334
11．6．1　转换函数　338
11．6．2　转换函数和友元函数　341
11．7　总结　343
11．8　复习题　344
11．9　编程练习　344
第　12章 类和动态内存分配　346
12．1　动态内存和类　346
12．1．1　复习示例和静态类成员　346
12．1．2　特殊成员函数　352
12．1．3　回到Stringbad：复制构造函数的哪里出了问题　354
12．1．4　Stringbad的其他问题：赋值运算符　356
12．2　改进后的新String类　357
12．2．1　修订后的默认构造函数　357
12．2．2　比较成员函数　358
12．2．3　使用中括号表示法访问字符　359
12．2．4　静态类成员函数　360
12．2．5　进一步重载赋值运算符　360
12．3　在构造函数中使用new时应注意的事项　364
12．3．1　应该和不应该　365
12．3．2　包含类成员的类的逐成员复制　365
12．4　有关返回对象的说明　366
12．4．1　返回指向const对象的引用　366
12．4．2　返回指向非const对象的引用　366
12．4．3　返回对象　367
12．4．4　返回const对象　367
12．5　使用指向对象的指针　368
12．5．1　再谈new和delete　369
12．5．2　指针和对象小结　370
12．5．3　再谈定位new运算符　371
12．6　复习各种技术　374
12．6．1　重载<<运算符　374
12．6．2　转换函数　375
12．6．3　其构造函数使用new的类　375
12．7　队列模拟　375
12．7．1　队列类　376
12．7．2　Customer类　382
12．7．3　ATM模拟　384
12．8　总结　387
12．9　复习题　388
12．10　编程练习　389
第　13章 类继承　392
13．1　一个简单的基类　393
13．1．1　派生一个类　394
13．1．2　构造函数：访问权限的考虑　395
13．1．3　使用派生类　397
13．1．4　派生类和基类之间的特殊关系　398
13．2　继承：is-a关系　400
13．3　多态公有继承　400
13．4　静态联编和动态联编　409
13．4．1　指针和引用类型的兼容性　409
13．4．2　虚成员函数和动态联编　411
13．4．3　有关虚函数注意事项　412
13．5　访问控制：protected　414
13．6　抽象基类　415
13．6．1　应用ABC概念　416
13．6．2　ABC理念　421
13．7　继承和动态内存分配　421
13．7．1　第 一种情况：派生类不使用new　421
13．7．2　第二种情况：派生类使用new　422
13．7．3　使用动态内存分配和友元的继承示例　423
13．8　类设计回顾　427
13．8．1　编译器生成的成员函数　427
13．8．2　其他的类方法　428
13．8．3　公有继承的考虑因素　429
13．8．4　类函数小结　432
13．9　总结　433
13．10　复习题　433
13．11　编程练习　434
第　14章 C++中的代码重用　436
14．1　包含对象成员的类　436
14．1．1　valarray类简介　437
14．1．2　Student类的设计　437
14．1．3　Student类示例　438
14．2　私有继承　443
14．2．1　Student类示例（新版本）　443
14．2．2　使用包含还是私有继承　447
14．2．3　保护继承　448
14．2．4　使用using重新定义访问权限　448
14．3　多重继承　449
14．3．1　有多少Worker　452
14．3．2　哪个方法　455
14．3．3　MI小结　461
14．4　类模板　462
14．4．1　定义类模板　462
14．4．2　使用模板类　464
14．4．3　深入探讨模板类　465
14．4．4　数组模板示例和非类型参数　469
14．4．5　模板多功能性　470
14．4．6　模板的具体化　473
14．4．7　成员模板　474
14．4．8　将模板用作参数　476
14．4．9　模板类和友元　477
14．4．10　模板别名（C++11）　482
14．5　总结　482
14．6　复习题　483
14．7　编程练习　484
第　15章 友元、异常和其他　488
15．1　友元　488
15．1．1　友元类　488
15．1．2　友元成员函数　492
15．1．3　其他友元关系　494
15．1．4　共同的友元　495
15．2　嵌套类　495
15．2．1　嵌套类和访问权限　496
15．2．2　模板中的嵌套　497
15．3　异常　499
15．3．1　调用abort()　500
15．3．2　返回错误码　500
15．3．3　异常机制　501
15．3．4　将对象用作异常类型　504
15．3．5　异常规范和C++11　506
15．3．6　栈解退　506
15．3．7　其他异常特性　510
15．3．8　exception类　511
15．3．9　异常、类和继承　514
15．3．10　异常何时会迷失方向　517
15．3．11　有关异常的注意事项　519
15．4　RTTI　520
15．4．1　RTTI的用途　520
15．4．2　RTTI的工作原理　521
15．5　类型转换运算符　526
15．6　总结　528
15．7　复习题　528
15．8　编程练习　529
第　16章 string类和标准模板库　530
16．1　string类　530
16．1．1　构造字符串　530
16．1．2　string类输入　533
16．1．3　使用字符串　535
16．1．4　string还提供了哪些功能　538
16．1．5　字符串种类　539
16．2　智能指针模板类　539
16．2．1　使用智能指针　540
16．2．2　有关智能指针的注意事项　542
16．2．3　unique_ptr为何优于auto_ptr　543
16．2．4　选择智能指针　545
16．3　标准模板库　545
16．3．1　模板类vector　546
16．3．2　可对矢量执行的操作　547
16．3．3　对矢量可执行的其他操作　550
16．3．4　基于范围的for循环（C++11）　553
16．4　泛型编程　553
16．4．1　为何使用迭代器　553
16．4．2　迭代器类型　556
16．4．3　迭代器层次结构　557
16．4．4　概念、改进和模型　558
16．4．5　容器种类　562
16．4．6　关联容器　568
16．4．7　无序关联容器（C++11）　572
16．5　函数对象　572
16．5．1　函数符概念　573
16．5．2　预定义的函数符　575
16．5．3　自适应函数符和函数适配器　576
16．6　算法　577
16．6．1　算法组　578
16．6．2　算法的通用特征　578
16．6．3　STL和string类　579
16．6．4　函数和容器方法　579
16．6．5　使用STL　581
16．7　其他库　583
16．7．1　vector、valarray和array　583
16．7．2　模板initializer_list（C++11）　586
16．7．3　使用initializer_list　587
16．8　总结　588
16．9　复习题　589
16．10　编程练习　590
第　17章 输入、输出和文件　592
17．1　C++输入和输出概述　592
17．1．1　流和缓冲区　593
17．1．2　流、缓冲区和iostream文件　594
17．1．3　重定向　596
17．2　使用cout进行输出　597
17．2．1　重载的<<运算符　597
17．2．2　其他ostream方法　598
17．2．3　刷新输出缓冲区　600
17．2．4　用cout进行格式化　601
17．3　使用cin进行输入　611
17．3．1　cin>>如何检查输入　612
17．3．2　流状态　613
17．3．3　其他istream类方法　616
17．3．4　其他istream方法　620
17．4　文件输入和输出　622
17．4．1　简单的文件I/O　623
17．4．2　流状态检查和is_open()　625
17．4．3　打开多个文件　625
17．4．4　命令行处理技术　626
17．4．5　文件模式　627
17．4．6　随机存取　633
17．5　内核格式化　638
17．6　总结　640
17．7　复习题　641
17．8　编程练习　642
第　18章 探讨C++新标准　644
18．1　复习前面介绍过的C++11功能　644
18．1．1　新类型　644
18．1．2　统一的初始化　644
18．1．3　声明　645
18．1．4　智能指针　646
18．1．5　异常规范方面的修改　647
18．1．6　作用域内枚举　647
18．1．7　对类的修改　647
18．1．8　模板和STL方面的修改　648
18．1．9　右值引用　649
18．2　移动语义和右值引用　650
18．2．1　为何需要移动语义　650
18．2．2　一个移动示例　651
18．2．3　移动构造函数解析　654
18．2．4　赋值　655
18．2．5　强制移动　655
18．3　新的类功能　658
18．3．1　特殊的成员函数　658
18．3．2　默认的方法和禁用的方法　659
18．3．3　委托构造函数　660
18．3．4　继承构造函数　660
18．3．5　管理虚方法：override和final　661
18．4　Lambda函数　662
18．4．1　比较函数指针、函数符和Lambda函数　662
18．4．2　为何使用lambda　664
18．5　包装器　666
18．5．1　包装器function及模板的低效性　666
18．5．2　修复问题　667
18．5．3　其他方式　669
18．6　可变参数模板　669
18．6．1　模板和函数参数包　669
18．6．2　展开参数包　670
18．6．3　在可变参数模板函数中使用递归　670
18．7　C++11新增的其他功能　672
18．7．1　并行编程　672
18．7．2　新增的库　672
18．7．3　低级编程　673
18．7．4　杂项　673
18．8　语言变化　674
18．8．1　Boost项目　674
18．8．2　TR1　674
18．8．3　使用Boost　674
18．9　接下来的任务　675
18．10　总结　675
18．11　复习题　676
18．12　编程练习　678







