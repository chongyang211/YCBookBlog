# 专栏笔记总结大全


**01.基础语法**

- 1.1.5 C语⾔特点：C 语言的语法都是基础语法，不提供高级的数据结构，比如 C 语言没有“类”class，复杂结构需要自己构造。
- 1.1.6 C语言版本：
- 1.3.3 编译源文件：使用gcc编译器，将源文件hello.c编译成二进制代码。
- 1.4.2 语句块：使用一对大括号{}，组成一个块，也称为复合语句
- 1.4.5 标准库头文件：“标准库”，因为它们是写入标准的，到底包括哪些功能，应该怎么使用的，都是规定好的，这样才能保证代码的规范和可移植。
- 1.5.1 修饰符：修饰符是用来修改基本数据类型的含义的关键字，通常用于声明变量或函数的类型。
- 1.5.2 标识符：
- 1.5.3 常见关键字：


**03.运算符**

- 3.1 自增运算符：这个 ++：是自增运算符，这个 --：是自减运算符。
- 3.6 位运算符：



---

### **2. `sizeof` 的语法**
`sizeof` 有两种使用方式：

#### **(1) 获取数据类型的大小**
```c
sizeof(type)
```
- `type` 是数据类型，如 `int`、`float`、`char` 等。

#### **(2) 获取变量的大小**
```c
sizeof(variable)
```
- `variable` 是变量名，可以是基本类型变量、数组、结构体等。

---

### **3. `sizeof` 的返回值**
- `sizeof` 返回一个 `size_t` 类型的值，表示数据类型或变量占用的字节数。
- `size_t` 是一个无符号整数类型，通常定义在 `<stddef.h>` 头文件中。

---

### **4. `sizeof` 的使用示例**

#### **(1) 获取基本数据类型的大小**
```c
#include <stdio.h>

int main() {
    printf("Size of int: %zu bytes\n", sizeof(int));
    printf("Size of float: %zu bytes\n", sizeof(float));
    printf("Size of char: %zu bytes\n", sizeof(char));
    printf("Size of double: %zu bytes\n", sizeof(double));
    return 0;
}
```

**输出：**
```
Size of int: 4 bytes
Size of float: 4 bytes
Size of char: 1 bytes
Size of double: 8 bytes
```

#### **(2) 获取变量的大小**
```c
#include <stdio.h>

int main() {
    int a = 10;
    double b = 3.14;
    char c = 'A';

    printf("Size of a: %zu bytes\n", sizeof(a));
    printf("Size of b: %zu bytes\n", sizeof(b));
    printf("Size of c: %zu bytes\n", sizeof(c));
    return 0;
}
```

**输出：**
```
Size of a: 4 bytes
Size of b: 8 bytes
Size of c: 1 bytes
```

#### **(3) 获取数组的大小**
```c
#include <stdio.h>

int main() {
    int arr[10];
    printf("Size of arr: %zu bytes\n", sizeof(arr)); // 数组总大小
    printf("Size of arr[0]: %zu bytes\n", sizeof(arr[0])); // 单个元素大小
    printf("Number of elements in arr: %zu\n", sizeof(arr) / sizeof(arr[0])); // 数组元素个数
    return 0;
}
```

**输出：**
```
Size of arr: 40 bytes
Size of arr[0]: 4 bytes
Number of elements in arr: 10
```

#### **(4) 获取结构体的大小**
```c
#include <stdio.h>

struct Student {
    int id;
    char name[20];
    float score;
};

int main() {
    struct Student s;
    printf("Size of struct Student: %zu bytes\n", sizeof(s));
    return 0;
}
```

**输出：**
```
Size of struct Student: 28 bytes
```

#### **(5) 获取指针的大小**
```c
#include <stdio.h>

int main() {
    int *ptr;
    printf("Size of ptr: %zu bytes\n", sizeof(ptr)); // 指针的大小
    return 0;
}
```

**输出：**
```
Size of ptr: 8 bytes
```

---

### **5. `sizeof` 的注意事项**
1. **`sizeof` 是编译时运算符**：
    - `sizeof` 在编译时计算，不会在运行时执行。
    - 例如，`sizeof(int)` 在编译时就已经确定。

2. **`sizeof` 与数组**：
    - `sizeof` 可以用于获取数组的总大小，但不能用于获取动态分配数组的大小。
    - 例如：
      ```c
      int *arr = malloc(10 * sizeof(int));
      printf("%zu\n", sizeof(arr)); // 输出指针的大小，而不是数组的大小
      ```

3. **`sizeof` 与结构体**：
    - 结构体的大小可能包含填充字节（Padding），因此 `sizeof` 返回的值可能大于成员大小的总和。

4. **`sizeof` 与字符串**：
    - `sizeof` 可以用于获取字符数组的大小，但不能用于获取字符串的长度。
    - 例如：
      ```c
      char str[] = "Hello";
      printf("%zu\n", sizeof(str)); // 输出 6（包括 '\0'）
      printf("%zu\n", strlen(str)); // 输出 5（不包括 '\0'）
      ```

---

### **6. `sizeof` 的常见用途**
- **动态内存分配**：
  ```c
  int *arr = malloc(10 * sizeof(int));
  ```
- **计算数组元素个数**：
  ```c
  int arr[] = {1, 2, 3, 4, 5};
  int n = sizeof(arr) / sizeof(arr[0]);
  ```
- **跨平台兼容性**：
  使用 `sizeof` 可以确保代码在不同平台上正确运行，因为数据类型的大小可能因平台而异。

---

### **总结**
- `sizeof` 用于获取数据类型或变量在内存中的大小。
- 它是编译时运算符，不会影响运行时性能。
- 常用于动态内存分配、计算数组元素个数和跨平台兼容性。
- 熟练掌握 `sizeof` 的用法有助于编写高效、可移植的 C 程序。