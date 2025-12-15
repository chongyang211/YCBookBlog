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




            下面是一个简单的案例，帮助你理解 **类型导入** 和 **普通导入** 的区别，以及如何在函数中使用类型导入来定义 `this` 的类型。

---

### **案例：类型导入 vs 普通导入**

#### **1. 定义类和类型**
在 `index-session.ts` 文件中，我们定义一个类 `MainSession` 和一个函数 `handleQRRegister`。

```typescript
// index-session.ts
export default class MainSession {
  logger: string = "Logger initialized";
  sessionKey: string = "SessionKey123";

  async handleQRRegister(qrCodeContent: string): Promise<void> {
    // 使用 call 绑定 this
    return handleQRRegister.call(this, qrCodeContent);
  }
}

// 导出一个独立的函数
export async function handleQRRegister(
  this: MainSession, // 使用 MainSession 作为 this 的类型
  qrCodeContent: string
): Promise<void> {
  const logger = this.logger;
  console.log(`Logging from handleQRRegister: ${logger}`);
  console.log(`QR Code Content: ${qrCodeContent}`);
}
```

---

#### **2. 使用类型导入**
在另一个文件中，我们使用 **类型导入** 来引用 `MainSession` 类型，而不导入实际的类。

```typescript
// main.ts
import type MainSession from "./index-session"; // 类型导入
import { handleQRRegister } from "./index-session"; // 普通导入（函数）

// 创建一个 MainSession 实例
const session = new (require("./index-session").default)();

// 调用 handleQRRegister 函数，绑定 this
handleQRRegister.call(session, "Sample QR Code Content");
```

---

### **关键点解释**

#### **1. 类型导入 (`import type`)**

#### **2. 普通导入 (`import`)**
- **作用**：导入实际的代码或值，会在编译后的 JavaScript 代码中包含这些内容。
- **使用场景**：当你需要实际使用类、函数或变量时。
- **示例**：
  ```typescript
  import { handleQRRegister } from "./index-session";
  ```

#### **3. 函数绑定机制**
- 在 `handleQRRegister` 函数中，`this` 的类型被定义为 `MainSession`。
- 使用 `call` 方法将 `this` 绑定到 `MainSession` 的实例。
- **示例**：
  ```typescript
  handleQRRegister.call(session, "Sample QR Code Content");
  ```

---

### **编译后的 JavaScript 代码**
编译后的 `main.js` 文件如下：

```javascript
// main.js
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const index_session_1 = require("./index-session");

// 创建一个 MainSession 实例
const session = new index_session_1.default();

// 调用 handleQRRegister 函数，绑定 this
(0, index_session_1.handleQRRegister).call(session, "Sample QR Code Content");
```

可以看到，`import type` 的代码被完全移除，因为它只在编译时存在。

---

### **总结**
- **类型导入**：仅用于类型检查，不会生成运行时代码。
- **普通导入**：导入实际的代码或值，会生成运行时代码。
- **函数绑定**：使用 `call` 方法将 `this` 绑定到类的实例。

通过这个案例，你可以清晰地理解类型导入的作用和使用场景。


