# 专栏笔记总结大全


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
- 4.1.4 函数重复声明：如果一个函数重复声明，后面的声明就会覆盖前面的声明。
- 4.5.1 基础用法：使用 `return` 语句返回函数的结果。如果没有 `return`，函数默认返回 `undefined`。
- 4.5.2 返回函数：JavaScript 函数可以返回另一个函数，这种特性称为 高阶函数。
- 4.5.3 返回Promise：在异步编程中，函数可以返回一个 Promise 对象，用于处理异步操作的结果。
- 4.6 对象中函数：对象的属性值可以是任何的数据类型，也可以是个函数。
- 4.11 this对象：1.以函数的形式调用时，this永远都是window；2.以方法的形式调用时，this就是调用方法的那个对象。




            JavaScript 中的 **原型对象（Prototype）** 是实现继承和共享属性的核心机制。每个对象都有一个内部属性 `[[Prototype]]`（可以通过 `__proto__` 访问），指向其原型对象。理解原型对象是掌握 JavaScript 面向对象编程的关键。

---

### **1. 原型对象的基本概念**


---

### **2. 原型对象的创建与访问**
#### **(1) 使用 `__proto__` 访问原型对象**
`__proto__` 是一个非标准属性，用于访问对象的原型对象。

##### **示例：**
```javascript
const obj = {};
console.log(obj.__proto__); // 输出: Object.prototype
```

#### **(2) 使用 `Object.getPrototypeOf()` 获取原型对象**
`Object.getPrototypeOf()` 是标准方法，用于获取对象的原型对象。

##### **示例：**
```javascript
const obj = {};
console.log(Object.getPrototypeOf(obj)); // 输出: Object.prototype
```

#### **(3) 使用 `Object.setPrototypeOf()` 设置原型对象**
`Object.setPrototypeOf()` 用于设置对象的原型对象。

##### **示例：**
```javascript
const parent = { name: "Parent" };
const child = {};

Object.setPrototypeOf(child, parent); // 设置 child 的原型为 parent
console.log(child.name); // 输出: Parent（通过原型链访问）
```

---

### **3. 构造函数与原型对象**
#### **(1) 构造函数**
构造函数是用于创建对象的函数。通过 `new` 关键字调用构造函数时，会创建一个新对象，并将该对象的原型指向构造函数的 `prototype` 属性。

##### **示例：**
```javascript
function Person(name) {
    this.name = name;
}

const alice = new Person("Alice");
console.log(alice.name); // 输出: Alice
```

#### **(2) 构造函数的 `prototype` 属性**
每个构造函数都有一个 `prototype` 属性，指向一个对象。通过构造函数创建的对象会继承 `prototype` 对象的属性和方法。

##### **示例：**
```javascript
function Person(name) {
    this.name = name;
}

Person.prototype.greet = function () {
    console.log(`Hello, my name is ${this.name}`);
};

const alice = new Person("Alice");
alice.greet(); // 输出: Hello, my name is Alice
```

#### **(3) 原型链验证**

---

### **4. 原型继承**
#### **(1) 基于原型的继承**
通过设置对象的原型对象，可以实现继承。

##### **示例：**
```javascript
const parent = {
    greet() {
        console.log("Hello from parent!");
    },
};

const child = Object.create(parent); // child 的原型是 parent
child.greet(); // 输出: Hello from parent!
```

#### **(2) 构造函数继承**
通过调用父构造函数并设置子构造函数的 `prototype`，可以实现构造函数继承。

##### **示例：**
```javascript
function Parent(name) {
    this.name = name;
}

Parent.prototype.greet = function () {
    console.log(`Hello, my name is ${this.name}`);
};

function Child(name, age) {
    Parent.call(this, name); // 调用父构造函数
    this.age = age;
}

Child.prototype = Object.create(Parent.prototype); // 设置原型链
Child.prototype.constructor = Child; // 修复构造函数指向

const alice = new Child("Alice", 25);
alice.greet(); // 输出: Hello, my name is Alice
```

---

### **5. 原型对象的注意事项**
- **原型链的终点**：原型链的终点是 `null`，即 `Object.prototype.__proto__ === null`。
- **性能问题**：过长的原型链会影响属性查找的性能。
- **共享属性**：原型对象的属性和方法会被所有实例共享，如果属性是引用类型（如数组或对象），修改会影响所有实例。

---

### **总结**
JavaScript 的原型对象是实现继承和共享属性的核心机制。通过原型链，对象可以访问其原型对象的属性和方法。掌握原型对象的使用可以帮助你更好地理解 JavaScript 的面向对象编程和继承机制。

