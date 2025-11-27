# 专栏笔记总结大全




- **`Promise`**：处理异步操作。
    - 方法：`then()`、`catch()`、`finally()`、`Promise.all()`。
- **`Proxy`**：创建对象的代理。
- **`Reflect`**：提供操作对象的静态方法。

#### **2.2 全局函数**
- **`eval()`**：执行字符串中的 JavaScript 代码。
- **`isNaN()`**：检查值是否为 `NaN`。
- **`isFinite()`**：检查值是否为有限数。
- **`parseInt()` 和 `parseFloat()`**：将字符串转换为整数或浮点数。
- **`encodeURI()` 和 `decodeURI()`**：编码或解码 URI。
- **`encodeURIComponent()` 和 `decodeURIComponent()`**：编码或解码 URI 组件。

---

### **3. 异步与并发**
#### **3.1 `Promise`**
- 用于处理异步操作。
- 方法：`then()`、`catch()`、`finally()`、`Promise.all()`、`Promise.race()`。

#### **3.2 `Async/Await`**
- 基于 `Promise` 的语法糖，简化异步代码。

#### **3.3 `Web Workers`**
- 在后台运行脚本，实现多线程。

#### **3.4 `Event Loop`**
- JavaScript 的异步执行机制。

---

### **4. 浏览器相关**
#### **4.1 `Window`**
- 表示浏览器窗口。
- 属性：`window.document`、`window.location`、`window.localStorage`。

#### **4.2 `Document`**
- 表示 HTML 文档。
- 方法：`getElementById()`、`querySelector()`、`createElement()`。

#### **4.3 `Navigator`**
- 提供浏览器信息。
- 属性：`navigator.userAgent`、`navigator.language`。

#### **4.4 `Location`**
- 提供当前页面的 URL 信息。
- 属性：`location.href`、`location.pathname`。

#### **4.5 `History`**
- 提供浏览器历史记录。
- 方法：`history.back()`、`history.pushState()`。

#### **4.6 `Storage`**
- 提供本地存储功能。
- 方法：`localStorage.setItem()`、`sessionStorage.getItem()`。

#### **4.7 `Fetch API`**
- 用于发起网络请求。
- 方法：`fetch()`。

#### **4.8 `WebSocket`**
- 用于实现 WebSocket 通信。

---

### **5. 国际化与本地化**
#### **5.1 `Intl`**
- 提供国际化功能。
- 对象：`Intl.DateTimeFormat`、`Intl.NumberFormat`。

#### **5.2 `toLocaleString()`**
- 根据本地化规则格式化数据。

---

### **6. 二进制与文件处理**
#### **6.1 `ArrayBuffer`**
- 表示通用的二进制数据缓冲区。

#### **6.2 `Blob`**
- 表示不可变的二进制数据。

#### **6.3 `File`**
- 表示文件对象。

#### **6.4 `FileReader`**
- 读取文件内容。

---

### **7. 其他**
#### **7.1 `URL` 和 `URLSearchParams`**
- 处理 URL 和查询参数。

#### **7.2 `console`**
- 提供调试功能。
- 方法：`console.log()`、`console.error()`、`console.table()`。

#### **7.3 `performance`**
- 提供性能测量功能。
- 方法：`performance.now()`。

---

### **8. 示例**
```javascript
// 数组操作
const numbers = [1, 2, 3];
const doubled = numbers.map(n => n * 2);
console.log(doubled); // 输出: [2, 4, 6]

// 字符串操作
const str = 'Hello, World!';
console.log(str.slice(0, 5)); // 输出: Hello

// 日期操作
const now = new Date();
console.log(now.getFullYear()); // 输出: 当前年份

// Promise 示例
const promise = new Promise((resolve, reject) => {
  setTimeout(() => resolve('Done!'), 1000);
});
promise.then(result => console.log(result)); // 输出: Done!

// Fetch API 示例
fetch('https://api.example.com/data')
  .then(response => response.json())
  .then(data => console.log(data));
```

---

### **总结**
JavaScript 标准库涵盖了数据类型、异步操作、浏览器交互、国际化、二进制处理等多个方面。熟练掌握这些功能可以极大地提高开发效率，并解决各种编程问题。


