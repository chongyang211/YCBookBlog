



### 四、运行时隐式触发异常

很多异常看起来是"自动发生的"，实际上是运行时/编译器**插入了检查代码**：

#### 4.1 空指针检测

```java
// 你写的代码
obj.method();

// JVM 实际执行（两种策略）

// 策略A：显式检查（解释执行时）
if (obj == null) throw new NullPointerException();
invokevirtual obj.method

// 策略B：硬件陷阱（JIT编译后）
// 直接访问 obj 的内存，如果 obj == null（地址0）
// → CPU 触发 SIGSEGV
// → JVM 的信号处理器捕获 SIGSEGV
// → 判断是 null 访问
// → 将 SIGSEGV 转换为 NullPointerException
// 这样正常路径零开销（不需要 if null 检查）！
```

#### 4.2 数组越界检测

```java
// 你写的代码
arr[i] = 10;

// JVM 实际生成
if (i < 0 || i >= arr.length)
    throw new ArrayIndexOutOfBoundsException(i);
arr[i] = 10;

// Rust 也一样
let v = vec![1, 2, 3];
v[5]; // 编译器插入边界检查，越界触发 panic
```

```c
// C 语言不检查！
int arr[3];
arr[10] = 42; // 未定义行为，可能：
               // - 覆盖其他变量的内存
               // - 段错误（如果碰巧越界到未映射页）
               // - 看起来正常运行但数据已损坏
               // 这就是 C 不安全的根源
```

#### 4.3 类型转换检测

```java
// Java
Object obj = "hello";
Integer i = (Integer) obj;
// JVM 检查 obj 的实际类型，不匹配 → ClassCastException

// C++ dynamic_cast
Base* b = new Derived();
Other* o = dynamic_cast<Other*>(b);
// RTTI 检查类型，不匹配 → 返回 nullptr（指针）或抛 bad_cast（引用）

// C 语言
void* p = something;
int* ip = (int*)p;  // 不检查！直接强转，错了就是未定义行为
```

---

### 五、异常触发的完整分层模型


**异常转换示例**：

| 底层事件 | OS 层 | Java | C++ | Go | Rust |
|---------|-------|------|-----|-----|------|
| CPU 除零 | SIGFPE | ArithmeticException | 未定义行为(整数) | panic | panic |
| 访问地址0 | SIGSEGV | NullPointerException | 未定义行为 | panic: nil pointer | 编译期禁止 |
| 栈空间耗尽 | SIGSEGV | StackOverflowError | SIGSEGV(进程终止) | runtime: stack overflow | stack overflow |
| 堆内存耗尽 | malloc返回NULL | OutOfMemoryError | std::bad_alloc | runtime: out of memory | abort |

---

### 六、核心原理总结

异常的引起归根结底只有两条路径：

```
路径 A：硬件 → 内核 → 运行时 → 语言异常（被动发现）
         CPU 执行指令失败，硬件中断，内核转信号，运行时包装为异常对象

路径 B：代码 → 运行时 → 语言异常（主动抛出）
         运行时检查失败（null/越界/类型）或程序员显式 throw/panic
```

两条路径最终都汇聚到同一个机制：**栈展开（Stack Unwinding）** ——从异常发生点沿调用栈逆向回溯，逐帧执行清理代码，直到找到匹配的处理器或到达栈顶。

这就是异常的核心原理：**检测 → 创建 → 传播 → 匹配 → 处理**，五个步骤，横跨硬件到应用层，所有语言概莫能外。


