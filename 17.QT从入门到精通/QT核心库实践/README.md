# 专栏笔记总结大全


————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————


---


---


---

### **6. 容器类（Containers）**
- **功能**：提供高效的数据结构。
- **核心类**：
   - `QList`、`QVector`：动态数组。
   - `QMap`、`QHash`：键值对容器。
   - `QSet`：集合。
   - `QStack`、`QQueue`：栈和队列。

---


---

### **8. 字符串处理（String Handling）**
- **功能**：提供高效的字符串操作。
- **核心类**：
   - `QString`：Unicode 字符串。
   - `QStringList`：字符串列表。
   - `QRegularExpression`：正则表达式。

---

### **9. 日期与时间（Date & Time）**
- **功能**：处理日期、时间、定时器。
- **核心类**：
   - `QDate`、`QTime`、`QDateTime`：日期和时间。
   - `QTimer`：定时器。

---

### **10. 资源管理（Resource Management）**
- **功能**：将资源（如图片、文件）嵌入到应用程序中。
- **核心类**：
   - `QResource`：访问嵌入资源。
   - `qrc` 文件：定义资源文件。

---

### **11. 插件系统（Plugin System）**
- **功能**：支持动态加载插件。
- **核心类**：
   - `QPluginLoader`：加载插件。
   - `QGenericPlugin`、`QGenericPluginFactory`：插件基类。

---

### **12. 国际化（Internationalization）**
- **功能**：支持多语言翻译。
- **核心类**：
   - `QTranslator`：加载翻译文件。
   - `QCoreApplication::translate()`：翻译字符串。

---

### **13. 日志与调试（Logging & Debugging）**
- **功能**：提供日志输出和调试工具。
- **核心类**：
   - `qDebug()`、`qInfo()`、`qWarning()`、`qCritical()`：日志输出。
   - `QDebug`：自定义调试输出。

---

### **14. 工具类（Utility Classes）**
- **功能**：提供常用的工具函数和类。
- **核心类**：
   - `QPoint`、`QSize`、`QRect`：几何类。
   - `QVariant`：通用数据类型。
   - `QSettings`：应用程序配置管理。

---

### **15. 网络支持（Network Support）**
- **功能**：提供基本的网络功能。
- **核心类**：
   - `QUrl`：URL 处理。
   - `QHostAddress`：IP 地址处理。
   - `QNetworkRequest`、`QNetworkReply`：网络请求和响应。

---

### **总结**
Qt Core 是 Qt 框架的核心模块，提供了丰富的功能和技术栈，涵盖了对象模型、事件处理、线程、容器、文件 I/O、字符串处理、国际化等多个方面。它是构建 Qt 应用程序的基础，几乎所有 Qt 模块都依赖于 Qt Core。如果你有更多关于 Qt Core 或其他 Qt 模块的问题，欢迎随时提问！


————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————


`QMetaObject::invokeMethod` 是 Qt 框架中的一个方法，用于在运行时动态调用对象的成员函数。它通常用于跨线程调用、延迟执行或调用信号和槽。以下是关于 `QMetaObject::invokeMethod` 的详细说明和用法。

---

### **函数原型**
```cpp
static bool QMetaObject::invokeMethod(
    QObject *object,                // 目标对象
    const char *method,             // 方法名
    Qt::ConnectionType type,        // 连接类型
    QGenericReturnArgument ret,     // 返回值
    QGenericArgument val0 = QGenericArgument(), // 参数 1
    QGenericArgument val1 = QGenericArgument(), // 参数 2
    QGenericArgument val2 = QGenericArgument(), // 参数 3
    QGenericArgument val3 = QGenericArgument(), // 参数 4
    QGenericArgument val4 = QGenericArgument(), // 参数 5
    QGenericArgument val5 = QGenericArgument(), // 参数 6
    QGenericArgument val6 = QGenericArgument(), // 参数 7
    QGenericArgument val7 = QGenericArgument(), // 参数 8
    QGenericArgument val8 = QGenericArgument(), // 参数 9
    QGenericArgument val9 = QGenericArgument()  // 参数 10
);
```

---

### **参数说明**
1. **`object`**：目标对象，即调用方法的对象。
2. **`method`**：方法名，可以是信号、槽或普通成员函数。方法名需要以字符串形式提供，例如 `"methodName"`。
3. **`type`**：连接类型，决定方法调用的方式。常用值：
    - `Qt::AutoConnection`：自动选择（默认）。
    - `Qt::DirectConnection`：直接调用（同步）。
    - `Qt::QueuedConnection`：队列调用（异步）。
4. **`ret`**：返回值，使用 `Q_RETURN_ARG` 宏定义。
5. **`val0` 到 `val9`**：方法参数，使用 `Q_ARG` 宏定义。

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

