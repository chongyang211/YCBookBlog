# 专栏笔记总结大全



## QT核心库实践

1. [01.核心功能基础]()
2. [02.并发与多线程]()
3. [03.文件与IO系统]()
4. [06.QT事件系统]()


## 01.核心功能基础

- 1.1.1 对象模型：`QObject`是所有 Qt 对象的基类，提供信号槽机制、对象树管理、事件处理等核心功能。
- 1.1.2 元对象系统：提供运行时类型信息（RTTI）、信号与槽机制、动态属性系统等。
- 1.2.1 QObject说明：是 Qt 框架中最基础的类，几乎所有 Qt 类都直接或间接继承自 `QObject`。
- 1.2.2 树与父子关系：Qt 对象可以组织成树形结构，每个对象可以有一个父对象和多个子对象。
- 1.2.3 对象名称与查找：每个 `QObject` 对象可以有一个唯一的名称（`objectName`），用于在对象树中查找。
- 1.2.4 动态属性：Qt 对象可以动态添加属性，这些属性可以在运行时查询和修改。
- 1.2.5 信号与槽：信号是事件的通知，槽是事件的响应函数。使用 `QObject::connect()` 连接信号与槽。信号和槽用于对象之间通信。
- 1.2.6 事件处理：
- 1.2.7 线程支持：
- 1.2.8 注意事项：所有使用信号与槽、动态属性等特性的类必须包含 `Q_OBJECT` 宏。
- 1.3.1 元对象说明：每个 `QObject` 派生类都有一个对应的元对象，用于描述类的元信息（如类名、信号、槽、属性等）。
- 1.3.2 元对象编译器：
- 1.3.3 反射（Reflection）：
- 1.3.4 动态调用：通过元对象系统，可以在运行时动态调用对象的信号、槽和属性。
- 1.4.1 Q_OBJECT：所有使用元对象系统的类必须包含 `Q_OBJECT` 宏。该宏会触发 `moc` 生成元对象代码。
- 1.4.2 QMetaObject：是 Qt 元对象系统的核心类，提供了对类元信息的访问。
- 1.4.3 Q_PROPERTY：是一个宏，用于声明类的属性，支持属性绑定、动画、序列化等功能。
- 1.4.4 Q_INVOKABLE：是一个宏，用于将方法标记为可通过元对象系统调用的方法。

## 02.并发与多线程

- 2.2.1 QThread类：是 Qt 中用于管理线程的类。每个 `QThread` 对象代表一个线程，可以通过 `start()` 启动线程。
- 2.2.2 线程与事件循环：Qt 的事件循环（`QEventLoop`）是线程的核心，用于处理信号与槽、定时器、网络事件等。
- 2.2.3 线程安全：
- 2.3.1 继承QThread：通过继承 `QThread` 并重写 `run()` 方法来实现自定义线程逻辑。
- 2.3.2 moveToThread：
- 2.4.1 并行计算：
- 2.4.2 并行映射：
- 2.5.1 互斥锁QMutex：`QMutex` 用于保护共享资源，防止多个线程同时访问。
- 2.5.2 QReadWriteLock：`QReadWriteLock` 允许多个线程同时读取共享资源，但写操作是独占的。
- 2.5.3 QSemaphore：`QSemaphore` 用于控制对共享资源的访问数量。
- 2.5.4 QWaitCondition：`QWaitCondition` 用于线程间的条件等待和唤醒。
- 2.5.5 QAtomic：

## 03.文件与IO系统

- 

## 06.QT事件系统



————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————







————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————



————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————




————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————


————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————




---

### **常用场景**
#### 1. **跨线程调用**
在 Qt 中，如果需要在非主线程中调用主线程的对象方法，可以使用 `QMetaObject::invokeMethod` 并指定 `Qt::QueuedConnection`。

```cpp
QMetaObject::invokeMethod(
    targetObject,       // 目标对象
    "methodName",       // 方法名
    Qt::QueuedConnection // 异步调用
);
```

#### 2. **延迟调用**
通过 `Qt::QueuedConnection`，可以实现延迟调用。

```cpp
QMetaObject::invokeMethod(
    targetObject,
    "methodName",
    Qt::QueuedConnection
);
```

#### 3. **调用带参数的方法**
如果方法需要参数，可以使用 `Q_ARG` 宏传递参数。

```cpp
QMetaObject::invokeMethod(
    targetObject,
    "setValue",
    Qt::AutoConnection,
    Q_ARG(int, 42),     // 参数 1
    Q_ARG(QString, "Hello") // 参数 2
);
```

#### 4. **调用带返回值的方法**
如果方法有返回值，可以使用 `Q_RETURN_ARG` 宏。

```cpp
int result;
QMetaObject::invokeMethod(
    targetObject,
    "calculate",
    Qt::DirectConnection,
    Q_RETURN_ARG(int, result), // 返回值
    Q_ARG(int, 10),            // 参数 1
    Q_ARG(int, 20)             // 参数 2
);
qDebug() << "Result:" << result;
```

---

### **示例代码**
以下是一个完整的示例，展示如何使用 `QMetaObject::invokeMethod`。

```cpp
#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QThread>

class Worker : public QObject {
    Q_OBJECT
public slots:
    void doWork(int value) {
        qDebug() << "Working with value:" << value;
    }

    int calculate(int a, int b) {
        return a + b;
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Worker worker;
    QThread thread;
    worker.moveToThread(&thread);
    thread.start();

    // 异步调用 doWork
    QMetaObject::invokeMethod(
        &worker,
        "doWork",
        Qt::QueuedConnection,
        Q_ARG(int, 42)
    );

    // 同步调用 calculate
    int result;
    QMetaObject::invokeMethod(
        &worker,
        "calculate",
        Qt::DirectConnection,
        Q_RETURN_ARG(int, result),
        Q_ARG(int, 10),
        Q_ARG(int, 20)
    );
    qDebug() << "Calculate result:" << result;

    thread.quit();
    thread.wait();
    return app.exec();
}

#include "main.moc"
```

---

### **注意事项**
1. **方法名必须匹配**：`method` 参数必须与目标对象的方法名完全一致，包括大小写。
2. **参数类型必须匹配**：使用 `Q_ARG` 传递参数时，参数类型必须与方法签名一致。
3. **线程安全**：跨线程调用时，确保目标对象是线程安全的。
4. **返回值**：如果方法有返回值，必须使用 `Q_RETURN_ARG` 宏。

---

### **总结**
`QMetaObject::invokeMethod` 是 Qt 中非常强大的工具，适用于跨线程调用、延迟执行和动态调用方法。通过合理使用，可以简化代码并提高程序的灵活性和可维护性。如果有更多问题，欢迎随时提问！


————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

