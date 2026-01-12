# 专栏笔记总结大全


基础笔记

https://blog.csdn.net/qq_38490457/article/details/109257751?ops_request_misc=%257B%2522request%255Fid%2522%253A%252219be579ab163d0c673158e2976c6aa0d%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=19be579ab163d0c673158e2976c6aa0d&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-109257751-null-null.142^v102^pc_search_result_base1&utm_term=javascript&spm=1018.2226.3001.4187




## 2.数据类型

- 2.1.1 类型分类：共有六种，字符串型（String）；数值型（Number）；布尔型（Boolean）；undefined型（Undefined）；null型（Null）；对象（object）
- 2.2.1 number：用来表示整数和浮点数，最常用的功能就是用来表示10进制的整数和浮点数。
- 2.2.2 string：String用于表示一个字符序列，即字符串。字符串需要使用 单引号 或 双引号 括起来。
- 2.2.3 boolean：
- 2.4.1 typeof：是一种运算符，它表示返回一个值的数据类型。检测变量的原始类型。
- 2.5.1 对象概述：JavaScript 中的对象（`Object`）是一种复合数据类型，用于存储键值对（key-value pairs）。
- 2.5.2 对象定义：1.对象字面量，使用花括号 `{}` 定义对象。2.使用 `new Object()`使用构造函数创建对象。
- 2.5.3 访问对象属性：第一种方式：使用`.`来访问。第二种方式：使用 [] 来访问。
- 2.5.6 删除属性：使用 `delete` 关键字删除属性。


## 4.函数

- 4.1.1 函数声明：使用 `function` 关键字定义函数。
- 4.1.2 函数表达式：函数表达式不会被提升，必须在定义之后调用。将函数赋值给变量。
- 4.1.3 箭头函数：使用 `=>` 语法定义函数。
- 4.1.4 函数重复声明：如果一个函数重复声明，后面的声明就会覆盖前面的声明。
- 4.5.1 基础用法：使用 `return` 语句返回函数的结果。如果没有 `return`，函数默认返回 `undefined`。
- 4.5.2 返回函数：JavaScript 函数可以返回另一个函数，这种特性称为 高阶函数。
- 4.5.3 返回Promise：在异步编程中，函数可以返回一个 Promise 对象，用于处理异步操作的结果。
- 4.6 对象中函数：对象的属性值可以是任何的数据类型，也可以是个函数。
- 4.11 this对象：1.以函数的形式调用时，this永远都是window；2.以方法的形式调用时，this就是调用方法的那个对象。

## 5.面向对象

- 5.2.1 原型对象：
- 5.2.2 原型对象概念：每个 JavaScript 对象都有一个原型对象（`[[Prototype]]`）。它包含可以被其他对象共享的属性和方法。
- 5.2.3 原型链：
- 

## 10.模块开发

- 10.1.1 模块化概念：模块化是指将一个大的程序文件，拆分成许多小的文件，然后将小文件组合起来。






以下是未捕获错误的处理流程及其核心原理：

---

### **1. 未捕获错误处理流程**

#### **（1）错误抛出**
当代码中发生错误（如 `throw new Error("Something went wrong")`）且没有被 `try...catch` 捕获时，错误会被抛出。

#### **（2）错误传播**
错误会沿着调用栈向上传播，直到找到最近的 `try...catch` 块。如果调用栈中没有 `try...catch` 块，错误会继续传播到全局作用域。

#### **（3）全局错误处理**
如果错误传播到全局作用域，JavaScript 引擎会触发全局错误处理机制：
- 在浏览器中，会触发 `window.onerror` 事件。
- 在 Node.js 中，会触发 `process.on('uncaughtException')` 事件。

#### **（4）程序终止**
如果全局错误处理机制也没有捕获错误，JavaScript 引擎会终止程序的执行，并打印错误信息到控制台。

---

### **2. 核心原理**

#### **（1）调用栈（Call Stack）**
JavaScript 使用调用栈来管理函数的执行顺序。当发生错误时，引擎会沿着调用栈向上查找是否有 `try...catch` 块来处理错误。

#### **（2）事件循环（Event Loop）**
在异步代码中，错误可能发生在事件循环的不同阶段。如果错误未被捕获，它会被传递到全局作用域。

#### **（3）全局错误事件**
JavaScript 提供了全局错误事件来捕获未处理的错误：
- **浏览器**：`window.onerror` 和 `window.addEventListener('error', ...)`。
- **Node.js**：`process.on('uncaughtException')` 和 `process.on('unhandledRejection')`。

---

### **3. 代码示例**

#### **（1）同步代码中的未捕获错误**
```javascript
function foo() {
    throw new Error("Oops!");
}

function bar() {
    foo();
}

bar(); // 未捕获错误，程序终止
```

#### **（2）异步代码中的未捕获错误**
```javascript
setTimeout(() => {
    throw new Error("Oops!");
}, 1000); // 未捕获错误，程序终止
```

#### **（3）全局错误处理（浏览器）**
```javascript
window.onerror = function (message, source, lineno, colno, error) {
    console.log("捕获到全局错误:", message);
    return true; // 阻止默认错误处理
};

throw new Error("Oops!"); // 错误被全局处理，程序不会终止
```

#### **（4）全局错误处理（Node.js）**
```javascript
process.on('uncaughtException', (err) => {
    console.log("捕获到未处理的异常:", err.message);
});

throw new Error("Oops!"); // 错误被全局处理，程序不会终止
```

---

### **4. 未捕获错误的处理建议**

#### **（1）使用 `try...catch`**
在可能抛出错误的代码块中使用 `try...catch` 捕获错误。

```javascript
try {
    throw new Error("Oops!");
} catch (error) {
    console.log("捕获到错误:", error.message);
}
```

#### **（2）全局错误处理**
在全局作用域中注册错误处理函数，捕获未处理的错误。

```javascript
// 浏览器
window.addEventListener('error', (event) => {
    console.log("捕获到全局错误:", event.message);
});

// Node.js
process.on('uncaughtException', (err) => {
    console.log("捕获到未处理的异常:", err.message);
});
```

#### **3）Promise 错误处理**
使用 `.catch()` 或 `try...catch`（在 `async/await` 中）捕获 Promise 中的错误。

```javascript
// 使用 .catch()
Promise.reject(new Error("Oops!")).catch((error) => {
    console.log("捕获到 Promise 错误:", error.message);
});

// 使用 async/await
(async () => {
    try {
        await Promise.reject(new Error("Oops!"));
    } catch (error) {
        console.log("捕获到错误:", error.message);
    }
})();
```

#### **4）避免静默失败**
确保所有错误都被捕获和处理，避免程序静默失败。

---

### **5. 总结**

| 阶段               | 描述                                                                 |
|--------------------|----------------------------------------------------------------------|
| **错误抛出**       | 代码中发生错误且未被捕获。                                           |
| **错误传播**       | 错误沿着调用栈向上传播，寻找 `try...catch` 块。                      |
| **全局错误处理**   | 如果错误未被捕获，触发全局错误事件（如 `window.onerror`）。          |
| **程序终止**       | 如果全局错误处理也未捕获错误，程序终止并打印错误信息。               |

未捕获错误的处理是 JavaScript 开发中的重要环节。通过合理使用 `try...catch`、全局错误处理和 Promise 错误捕获，可以有效避免程序崩溃，提高代码的健壮性。



