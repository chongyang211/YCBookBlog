# 专栏笔记总结大全


## 01.入门介绍

- 1.1.1 JavaScript语言：是一种轻量级的脚本语言。没有io相关操作，调用宿主环境提供的底层 API。
- 1.1.2 核心语法：基本的语法构造（比如操作符、控制结构、语句）和标准库。
- 1.2.1 操控浏览器能力：它是目前唯一一种通用的浏览器脚本语言，所有浏览器都支持。
- 1.2.2 广泛的使用领域：1.浏览器的平台化；2.移动平台开发；3.内嵌脚本语言；4.跨平台的桌面应用程序。
- 1.2.3 易学性：
- 1.2.4 强大的性能：1.灵活的语法，表达力强；2.支持编译运行；3.事件驱动和非阻塞式设计。
- 1.2.6 实验环境：Chrome 浏览器，它的“开发者工具”（Developer Tools）里面的“控制台”（console），就是运行 JavaScript 代码的理想环境。
- 1.3.1 语句：语句为了完成某项操作而完成的任务，其实可以理解每一行就是语句。
- 1.3.2 变量：变量就是为这个值取一个名字，然后引用这个名字就可以引用这个值。
- 1.3.3 变量提升：
- 1.3.4 标识符：
- 1.3.5 注释：
- 1.3.6 区块：使用大括号，将多个相关的语句组合在一起，称为“区块”（block）。
- 1.4.1 if 结构：
- 1.4.2 if...else结构：
- 1.4.3 switch结构：
- 1.4.4 三元运算符表达式：(条件) ? 表达式1 : 表达式2，如果“条件”为`true`，则返回“表达式1”的值，否则返回“表达式2”的值。
- 1.5.1 while循环：
- 1.5.2 for循环：
- 1.5.3 do...while循环：
- 1.5.4 break和continue：
- 1.5.5 标签（label）：

## 02.数据类型

- 2.1.1 数据类型简介：数值、字符串、布尔值这三种类型，合称为原始类型（primitive type）的值。
- 2.1.2 typeof运算符：可以返回一个值的数据类型。`typeof`可以用来检查一个没有声明的变量，而不报错。
- 2.2.1 null和undefined概述：将一个变量赋值为`undefined`或`null`，老实说，语法效果几乎没区别。
- 2.2.2 undefined演变：`null`是一个表示“空”的对象，转为数值时为`0`；`undefined`是一个表示“此处无定义”的原始值，转为数值时为`NaN`。


## 07.异步操作

- 7.1.1 单线程含义：
- 


### **1. `console.log()`**
- **描述**：输出普通日志信息。
- **示例**：
  ```javascript
  console.log('Hello, World!'); // 输出: Hello, World!
  console.log(42); // 输出: 42
  console.log({ name: 'Alice', age: 25 }); // 输出: { name: 'Alice', age: 25 }
  ```

---

### **2. `console.info()`**
- **描述**：输出信息性消息（通常与 `console.log()` 相同，但在某些浏览器中会显示不同的图标）。
- **示例**：
  ```javascript
  console.info('This is an info message.'); // 输出: This is an info message.
  ```

---

### **3. `console.warn()`**
- **描述**：输出警告信息（通常以黄色背景显示）。
- **示例**：
  ```javascript
  console.warn('This is a warning!'); // 输出: This is a warning!
  ```

---

### **4. `console.error()`**
- **描述**：输出错误信息（通常以红色背景显示）。
- **示例**：
  ```javascript
  console.error('This is an error!'); // 输出: This is an error!
  ```

---

### **5. `console.debug()`**
- **描述**：输出调试信息（通常与 `console.log()` 相同，但在某些浏览器中需要启用调试模式才能看到）。
- **示例**：
  ```javascript
  console.debug('Debugging information.'); // 输出: Debugging information.
  ```

---

### **6. `console.table()`**
- **描述**：以表格形式输出数组或对象。
- **示例**：
  ```javascript
  const users = [
    { name: 'Alice', age: 25 },
    { name: 'Bob', age: 30 },
  ];
  console.table(users);
  ```
  **输出**：
  ```
  ┌─────────┬─────────┬─────┐
  │ (index) │  name   │ age │
  ├─────────┼─────────┼─────┤
  │    0    │ 'Alice' │ 25  │
  │    1    │  'Bob'  │ 30  │
  └─────────┴─────────┴─────┘
  ```

---

### **7. `console.group()` 和 `console.groupEnd()`**
- **描述**：将日志信息分组显示。
- **示例**：
  ```javascript
  console.group('User Details');
  console.log('Name: Alice');
  console.log('Age: 25');
  console.groupEnd();
  ```
  **输出**：
  ```
  User Details
    Name: Alice
    Age: 25
  ```

---

### **8. `console.time()` 和 `console.timeEnd()`**
- **描述**：用于计算代码执行时间。
- **示例**：
  ```javascript
  console.time('Timer');
  for (let i = 0; i < 1000000; i++) {}
  console.timeEnd('Timer'); // 输出: Timer: 0.123ms
  ```

---

### **9. `console.assert()`**
- **描述**：如果断言为 `false`，则输出错误信息。
- **示例**：
  ```javascript
  console.assert(2 + 2 === 5, 'Math is broken!'); // 输出: Assertion failed: Math is broken!
  ```

---

### **10. `console.clear()`**
- **描述**：清空控制台。
- **示例**：
  ```javascript
  console.clear(); // 清空控制台
  ```

---

### **11. `console.count()`**
- **描述**：记录调用次数。
- **示例**：
  ```javascript
  console.count('Counter'); // 输出: Counter: 1
  console.count('Counter'); // 输出: Counter: 2
  ```

---

### **12. `console.dir()`**
- **描述**：以对象树的形式输出对象。
- **示例**：
  ```javascript
  console.dir(document.body); // 输出: <body> 的 DOM 对象树
  ```

---

### **13. `console.trace()`**
- **描述**：输出调用堆栈。
- **示例**：
  ```javascript
  function foo() {
    console.trace('Trace');
  }
  foo();
  ```
  **输出**：
  ```
  Trace
    at foo (script.js:2)
    at script.js:5
  ```

---

### **14. 格式化输出**
`console.log()` 支持格式化输出，使用占位符：
- `%s`：字符串
- `%d` 或 `%i`：整数
- `%f`：浮点数
- `%o`：对象
- `%c`：CSS 样式

**示例**：
```javascript
console.log('Name: %s, Age: %d', 'Alice', 25); // 输出: Name: Alice, Age: 25
console.log('%cStyled Text', 'color: red; font-size: 20px;'); // 输出红色大字
```

---

### **15. 示例汇总**
```javascript
console.log('Hello, World!');
console.info('This is an info message.');
console.warn('This is a warning!');
console.error('This is an error!');
console.table([{ name: 'Alice', age: 25 }]);
console.group('Group');
console.log('Inside group');
console.groupEnd();
console.time('Timer');
for (let i = 0; i < 1000000; i++) {}
console.timeEnd('Timer');
console.assert(2 + 2 === 5, 'Math is broken!');
console.clear();
console.count('Counter');
console.dir(document.body);
console.trace('Trace');
```

---

### **总结**
`console` 是 JavaScript 开发中不可或缺的调试工具。通过灵活使用 `console` 的各种方法，可以更高效地调试代码、分析性能、记录日志等。掌握这些方法，可以显著提升开发效率。






