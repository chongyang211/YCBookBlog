# 专栏笔记总结大全



## QT核心库实践

1. [01.核心功能基础]()
2. 


## 01.核心功能基础

- 1.1.1 对象模型：`QObject`是所有 Qt 对象的基类，提供信号槽机制、对象树管理、事件处理等核心功能。
- 1.1.2 元对象系统：提供运行时类型信息（RTTI）、信号与槽机制、动态属性系统等。
- 








————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

`QProcess` 是 Qt 中用于启动和控制外部进程的类。它允许你运行系统命令、与其他程序交互，并获取其输出和错误信息。`QProcess` 是跨平台的，适用于 Windows、macOS 和 Linux 等操作系统。以下是 `QProcess` 的详细介绍和用法：

---

### **1. 基本用法**

#### **1.1 启动外部进程**
使用 `start()` 方法启动外部进程。

```cpp
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QProcess process;
    process.start("ls", QStringList() << "-l" << "/"); // 在 Linux/macOS 下列出根目录

    if (!process.waitForStarted()) {
        qDebug() << "Failed to start process!";
        return 1;
    }

    process.waitForFinished(); // 等待进程结束
    qDebug() << "Output:" << process.readAllStandardOutput();

    return app.exec();
}
```

#### **1.2 启动带参数的命令**
可以通过 `QStringList` 传递命令行参数。

```cpp
QProcess process;
process.start("echo", QStringList() << "Hello, Qt!");
process.waitForFinished();
qDebug() << "Output:" << process.readAllStandardOutput();
```

---

### **2. 获取进程输出**

#### **2.1 标准输出**
使用 `readAllStandardOutput()` 读取进程的标准输出。

```cpp
QProcess process;
process.start("ls", QStringList() << "-l");
process.waitForFinished();
QString output = process.readAllStandardOutput();
qDebug() << "Output:" << output;
```

#### **2.2 标准错误**
使用 `readAllStandardError()` 读取进程的标准错误。

```cpp
QProcess process;
process.start("invalid_command");
process.waitForFinished();
QString error = process.readAllStandardError();
qDebug() << "Error:" << error;
```

---

### **3. 异步处理**

#### **3.1 使用信号与槽**
`QProcess` 提供了多个信号，可以用于异步处理进程的输出和状态。

```cpp
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

class ProcessHandler : public QObject
{
    Q_OBJECT

public:
    ProcessHandler() {
        connect(&process, &QProcess::readyReadStandardOutput, this, &ProcessHandler::readOutput);
        connect(&process, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred), this, &ProcessHandler::handleError);
        process.start("ping", QStringList() << "google.com");
    }

private slots:
    void readOutput() {
        qDebug() << "Output:" << process.readAllStandardOutput();
    }

    void handleError(QProcess::ProcessError error) {
        qDebug() << "Error occurred:" << error;
    }

private:
    QProcess process;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    ProcessHandler handler;

    return app.exec();
}
```

#### **3.2 常用信号**
- **`readyReadStandardOutput()`**：当有标准输出时触发。
- **`readyReadStandardError()`**：当有标准错误时触发。
- **`finished(int exitCode, QProcess::ExitStatus status)`**：当进程结束时触发。
- **`errorOccurred(QProcess::ProcessError error)`**：当进程发生错误时触发。

---

### **4. 进程控制**

#### **4.1 终止进程**
使用 `kill()` 或 `terminate()` 终止进程。

```cpp
QProcess process;
process.start("sleep", QStringList() << "10");
process.waitForStarted();

process.kill(); // 强制终止
// process.terminate(); // 尝试优雅终止
```

#### **4.2 检查进程状态**
使用 `state()` 检查进程的当前状态。

```cpp
QProcess process;
process.start("sleep", QStringList() << "5");

if (process.state() == QProcess::Running) {
    qDebug() << "Process is running";
}
```

---

### **5. 环境变量**

#### **5.1 设置环境变量**
使用 `setProcessEnvironment()` 设置进程的环境变量。

```cpp
QProcess process;
QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
env.insert("MY_VAR", "Hello, Qt!");
process.setProcessEnvironment(env);

process.start("printenv", QStringList() << "MY_VAR");
process.waitForFinished();
qDebug() << "Output:" << process.readAllStandardOutput();
```

---

### **6. 输入输出重定向**

#### **6.1 重定向输入**
使用 `write()` 向进程的标准输入写入数据。

```cpp
QProcess process;
process.start("grep", QStringList() << "Qt");
process.write("Hello, Qt!\nThis is a test.\n");
process.closeWriteChannel(); // 关闭输入通道
process.waitForFinished();
qDebug() << "Output:" << process.readAllStandardOutput();
```

#### **6.2 重定向输出**
可以将输出重定向到文件。

```cpp
QProcess process;
process.setStandardOutputFile("output.txt");
process.start("ls", QStringList() << "-l");
process.waitForFinished();
```

---

### **7. 总结**
- **启动进程**：使用 `start()` 启动外部进程。
- **获取输出**：使用 `readAllStandardOutput()` 和 `readAllStandardError()` 获取进程输出。
- **异步处理**：通过信号与槽机制处理进程的输出和状态。
- **进程控制**：使用 `kill()` 或 `terminate()` 终止进程。
- **环境变量**：使用 `setProcessEnvironment()` 设置环境变量。
- **输入输出重定向**：使用 `write()` 和 `setStandardOutputFile()` 重定向输入输出。

`QProcess` 是 Qt 中处理外部进程的强大工具，适用于各种系统命令和程序交互场景。如果还有其他问题，请随时提问！

————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————



————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————




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

