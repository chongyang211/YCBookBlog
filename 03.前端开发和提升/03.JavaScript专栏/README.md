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



JavaScript 中的函数是一等公民，可以像变量一样被传递、赋值和调用。函数是 JavaScript 的核心概念之一，用于封装可重用的代码块。以下是 JavaScript 函数的详细介绍：

---

### **1. 函数定义**
#### **1.1 函数声明**


#### **1.2 函数表达式**
- 将函数赋值给变量。
- 函数表达式不会被提升，必须在定义之后调用。
  ```javascript
  const greet = function(name) {
      return `Hello, ${name}!`;
  };

  console.log(greet("Bob")); // Hello, Bob!
  ```

#### **1.3 箭头函数（ES6）**
- 使用 `=>` 语法定义函数。
- 箭头函数没有自己的 `this`，适合用于回调函数。
  ```javascript
  const greet = (name) => `Hello, ${name}!`;

  console.log(greet("Charlie")); // Hello, Charlie!
  ```

---

### **2. 函数参数**
#### **2.1 默认参数（ES6）**
- 为参数设置默认值。
  ```javascript
  function greet(name = "Guest") {
      return `Hello, ${name}!`;
  }

  console.log(greet()); // Hello, Guest!
  ```

#### **2.2 剩余参数（ES6）**
- 使用 `...` 语法将多个参数收集到一个数组中。
  ```javascript
  function sum(...numbers) {
      return numbers.reduce((acc, num) => acc + num, 0);
  }

  console.log(sum(1, 2, 3, 4)); // 10
  ```

#### **2.3 解构参数**
- 使用解构语法从对象或数组中提取参数。
  ```javascript
  function greet({ name, age }) {
      return `Hello, ${name}! You are ${age} years old.`;
  }

  console.log(greet({ name: "Alice", age: 25 })); // Hello, Alice! You are 25 years old.
  ```

---

### **3. 返回值**
- 使用 `return` 语句返回函数的结果。
- 如果没有 `return`，函数默认返回 `undefined`。
  ```javascript
  function add(a, b) {
      return a + b;
  }

  console.log(add(2, 3)); // 5
  ```

---

### **4. 高阶函数**
- 高阶函数是接受函数作为参数或返回函数的函数。
  ```javascript
  function higherOrderFunction(callback) {
      return callback(10);
  }

  const result = higherOrderFunction((num) => num * 2);
  console.log(result); // 20
  ```

---

### **5. 闭包**
- 闭包是函数与其词法环境的组合，可以访问外部函数的作用域。
  ```javascript
  function outer() {
      let count = 0;

      return function inner() {
          count++;
          return count;
      };
  }

  const counter = outer();
  console.log(counter()); // 1
  console.log(counter()); // 2
  ```

---

### **6. 立即执行函数（IIFE）**
- 定义后立即执行的函数。
  ```javascript
  (function() {
      console.log("IIFE executed!");
  })();
  ```

---

### **7. 方法**
- 方法是对象的属性，值为函数。
  ```javascript
  const person = {
      name: "Alice",
      greet: function() {
          return `Hello, ${this.name}!`;
      },
  };

  console.log(person.greet()); // Hello, Alice!
  ```

---

### **8. 箭头函数的特点**
- 没有自己的 `this`，继承外层作用域的 `this`。
- 不能用作构造函数。
- 没有 `arguments` 对象。
  ```javascript
  const obj = {
      value: 10,
      getValue: function() {
          return () => this.value;
      },
  };

  const getValue = obj.getValue();
  console.log(getValue()); // 10
  ```

---

### **9. 递归函数**
- 函数调用自身。
  ```javascript
  function factorial(n) {
      if (n === 0) return 1;
      return n * factorial(n - 1);
  }

  console.log(factorial(5)); // 120
  ```

---

### **10. 回调函数**
- 作为参数传递给其他函数的函数。
  ```javascript
  function fetchData(callback) {
      setTimeout(() => {
          callback("Data received!");
      }, 1000);
  }

  fetchData((data) => {
      console.log(data); // Data received!
  });
  ```

---

### **11. Promise 和异步函数**
#### **11.1 Promise**
- 用于处理异步操作。
  ```javascript
  const promise = new Promise((resolve, reject) => {
      setTimeout(() => {
          resolve("Success!");
      }, 1000);
  });

  promise.then((result) => {
      console.log(result); // Success!
  });
  ```

#### **11.2 异步函数（async/await）**
- 使用 `async` 和 `await` 简化异步代码。
  ```javascript
  async function fetchData() {
      return new Promise((resolve) => {
          setTimeout(() => {
              resolve("Data received!");
          }, 1000);
      });
  }

  async function main() {
      const data = await fetchData();
      console.log(data); // Data received!
  }

  main();
  ```

---

### **12. 总结**
JavaScript 函数是灵活且强大的工具，支持多种定义方式（函数声明、函数表达式、箭头函数）和特性（默认参数、剩余参数、闭包、高阶函数等）。通过函数，可以封装逻辑、处理异步操作、实现递归等，是 JavaScript 编程的核心。










