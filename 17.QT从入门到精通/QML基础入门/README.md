# 专栏笔记总结大全


以下是对 `QGuiApplication` 的详细介绍及其工作原理。

---

### **1. `QGuiApplication` 的作用**
`QGuiApplication` 是 `QCoreApplication` 的子类，专门用于 GUI 应用程序。


---

### **2. `QGuiApplication` 的基本用法**
以下是一个简单的 `QGuiApplication` 使用示例：

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[]) {
    // 创建 QGuiApplication 实例
    QGuiApplication app(argc, argv);

    // 创建 QML 引擎并加载 QML 文件
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    // 进入事件循环
    return app.exec();
}
```

#### **代码解析**
1. **`QGuiApplication app(argc, argv);`**  
   创建 `QGuiApplication` 实例，初始化应用程序环境。

2. **`QQmlApplicationEngine engine;`**  
   创建 QML 引擎，用于加载和运行 QML 文件。

3. **`engine.load(QUrl(QStringLiteral("qrc:/main.qml")));`**  
   加载 QML 文件（通常是应用程序的主界面）。

4. **`return app.exec();`**  
   进入事件循环，等待用户输入和其他事件。

---

### **3. `QGuiApplication` 的工作原理**
#### **(1) 初始化**
- `QGuiApplication` 在构造函数中完成以下初始化工作：
    - 设置应用程序的名称、版本等元信息。
    - 初始化 GUI 相关的子系统，如字体、调色板、屏幕等。
    - 解析命令行参数，处理与 GUI 相关的选项（如窗口大小、位置等）。

#### **(2) 事件循环**
- `QGuiApplication` 的核心是事件循环（Event Loop），通过 `exec()` 方法启动。
- 事件循环不断从事件队列中获取事件（如鼠标点击、键盘输入、窗口事件等），并将其分发给相应的对象（如窗口、控件等）进行处理。
- 事件循环还负责处理定时器、网络事件等其他异步操作。

#### **(3) 窗口管理**
- `QGuiApplication` 管理应用程序的所有窗口（`QWindow` 实例）。
- 它负责窗口的创建、显示、隐藏、关闭等操作。
- 它还处理与屏幕相关的逻辑，如多屏幕支持、屏幕分辨率变化等。

#### **(4) 资源管理**
- `QGuiApplication` 管理应用程序的共享资源，如字体、图标、样式等。
- 它提供了统一的接口来加载和使用这些资源。

#### **(5) 退出机制**
- 当调用 `QGuiApplication::quit()` 或最后一个窗口关闭时，`QGuiApplication` 会退出事件循环并清理资源。
- 在退出前，它会触发 `aboutToQuit()` 信号，允许应用程序执行清理操作。

---

### **4. `QGuiApplication` 的主要功能**
#### **(1) 事件处理**
- `QGuiApplication` 提供了事件过滤器（Event Filter）和事件处理器（Event Handler）机制，允许开发者拦截和处理特定事件。
- 例如，可以重写 `QGuiApplication::event()` 方法来处理自定义事件。

#### **(2) 多屏幕支持**
- `QGuiApplication` 提供了对多屏幕的支持，可以通过 `screens()` 方法获取所有屏幕的信息。
- 它还处理屏幕的动态变化（如屏幕分辨率调整、屏幕添加或移除）。

#### **(3) 样式与主题**
- `QGuiApplication` 管理应用程序的样式和主题，可以通过 `setStyle()` 方法设置应用程序的样式（如 Fusion、Windows 等）。
- 它还支持自定义样式和主题。

#### **(4) 国际化**
- `QGuiApplication` 支持国际化（i18n），可以通过 `installTranslator()` 方法加载翻译文件。
- 它还提供了与语言和区域设置相关的功能。

---

### **5. `QGuiApplication` 与 `QApplication` 的区别**
- `QGuiApplication` 是 `QApplication` 的轻量级版本，专门用于不需要完整 Widget 支持的 GUI 应用程序（如基于 QML 的应用程序）。
- `QApplication` 提供了更多的功能，如完整的 Widget 支持、菜单、工具栏等，适用于传统的桌面应用程序。
- 如果应用程序仅使用 QML 而不使用 Widget，建议使用 `QGuiApplication`，以减少内存占用和启动时间。

---

### **6. 实际应用场景**
#### **(1) QML 应用程序**
- `QGuiApplication` 是 QML 应用程序的入口点，通常与 `QQmlApplicationEngine` 配合使用。
- 例如：
  ```cpp
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  return app.exec();
  ```

#### **(2) 自定义窗口管理**
- 如果需要自定义窗口管理逻辑，可以继承 `QGuiApplication` 并重写相关方法。
- 例如：
  ```cpp
  class MyApplication : public QGuiApplication {
  public:
      MyApplication(int &argc, char **argv) : QGuiApplication(argc, argv) {}
      bool notify(QObject *receiver, QEvent *event) override {
          // 自定义事件处理逻辑
          return QGuiApplication::notify(receiver, event);
      }
  };
  ```

#### **(3) 多屏幕支持**
- 在多屏幕环境中，可以使用 `QGuiApplication` 的 `screens()` 方法获取屏幕信息，并根据需要调整窗口位置和大小。
- 例如：
  ```cpp
  QList<QScreen*> screens = QGuiApplication::screens();
  for (QScreen *screen : screens) {
      qDebug() << "Screen:" << screen->name() << "Geometry:" << screen->geometry();
  }
  ```

---

### **7. 总结**
`QGuiApplication` 是 Qt GUI 应用程序的核心类，负责初始化、事件循环、窗口管理和资源管理。它是基于 QML 的应用程序的入口点，提供了轻量级的 GUI 支持。通过理解 `QGuiApplication` 的工作原理和功能，开发者可以更好地构建和管理 Qt GUI 应用程序。


## 01.QT基础概念

- 1.1.1 什么是QT：Qt是一个跨平台的C++图形用户界面应用程序框架。
- 1.1.3 QT的版本：
- 1.2.1 QT核心模块：core是核心框架，GUI是图形界面框架，Network是网络编程框架，SQL数据库框架等。
- 1.2.2 安装QT框架：用于开发和运行基于 Qt 的应用程序。开发者可以使用 Qt 框架编写跨平台的 C++ 程序。
- 1.2.3 开发须知：开发者需要同时安装 **Qt 框架** 和 **Qt Creator**，因为 Qt Creator 依赖于 Qt 框架来编译和运行程序。
- 1.4.1 环境准备：
- 1.6.1 Quick概念：它专注于快速开发流畅、动态的 UI，特别适合移动应用、嵌入式设备和桌面应用。用QML！
- 1.6.2 Quick模块：提供了基本的 UI 元素（如 `Rectangle`、`Text`、`Image` 等）和布局功能。
- 1.6.3 Quick优势：1.声明式语法和丰富的 UI 元素加速开发；2.基于 OpenGL 渲染，适合高性能 UI。
- 1.6.4 Quick技术：1.QML是用户界面的标记语言；2.javaScript是动态脚本语言；3.QT C++是高度可移植的增强型C++库。
- 1.7.2 编写QML代码：打开 `main.qml` 文件（位于 `Sources` 目录下）开始写代码。这个是入口！
- 1.7.3 编写Qt代码：
- 1.7.5 CMakeList：`CMakeLists.txt` 通过这个文件，CMake 可以正确配置和构建你的 Qt Quick 应用程序。

## 02.Qml基础语法

- 2.1.1 QML是什么：它是一种声明式语言，语法简洁直观，用于描述用户界面和交互逻辑。
- 2.1.2 QML文件结构：一个 QML 文件通常由以下部分组成，1.导入语句；2.根元素，3.子元素。
- 2.2.1 **import导入**——`import` 语句通常遵循以下格式：基本语法：import <模块名> [版本号] [as <别名>]
- 2.2.2 导入标准QML模块：例如 `QtQuick` 和 `QtQuick.Controls`。这些模块通常包含常用的 QML 类型和组件。
- 2.2.3 导入自定义QML文件：
- 2.2.4 使用别名导入模块：通过 `as` 关键字为模块指定别名，避免命名冲突或简化使用。
- 2.2.5 导入JavaScript文件：假设有一个名为 `utils.js` 的文件，可以这样导入：`import "utils.js" as Utils`
- 2.3.1 **QML类型系统**——基本类型：QML 提供了一些基本的内置类型，用于表示常见的数据类型和 UI 组件。主要有：数据类型，几何类型等。
- 2.3.2 JavaScript类型：
- 2.3.3 对象类型：QML 对象通常由类型名称、属性、信号和方法组成。一个对象可以嵌套其他对象，形成层次结构。
- 2.4 **对象特性**：对象特性包括：属性、信号、方法、绑定、状态等。
- 2.4.1 id特性：在 QML 中，每个对象都可以通过 `id` 属性来标识自己。`id` 必须是唯一的，且在同一作用域内不能重复。
- 2.4.2 属性特性：属性是对象的核心特性，用于存储数据或配置对象的行为。比如：`width`，`height`，`visible`等。
- 2.4.3 信号和信号处理器特性：信号是 QML 对象的事件机制，用于通知其他对象某些事件发生。
- 2.4.4 方法特性：方法是对象的行为特性，用于定义可执行的操作。可以通过 `function` 关键字定义方法。
- 2.4.7 状态控制：状态是 QML 对象的特性之一，用于定义对象的不同配置。通过状态，可以轻松切换对象的属性值或行为。
- 2.5.1 在QML中用JavaScript：可以在 QML 对象中定义 JavaScript 函数，用于封装逻辑。
- 2.5.2 导入JavaScript文件：在qml中导入JavaScript文件。使用 `import` 关键字导入外部 JavaScript 文件。
- 2.5.3 JavaScript与QML交互：JavaScript 可以直接访问 QML 对象的属性和方法，也可以通过信号与 QML 对象交互。
- 2.5.3.1 访问QML对象属性：
- 2.5.3.2 调用QML对象方法：
- 2.5.3.3 信号与JavaScript交互：
- 2.5.4 JavaScript模块化：通过外部 JavaScript 文件，可以实现模块化开发，将逻辑分离到不同的文件中。
- 2.5.5 JavaScript作用域：1.函数内部定义的变量只能在函数内访问。2.外部 JavaScript 文件中的函数和变量可以被 QML 全局访问。
- 2.5.6 JavaScript性能优化：

## 03.Quick基础

- 3.1.1 Item：它是所有可视化元素的基类，但本身并不直接绘制任何内容。`Item` 提供了一个容器和布局的基础，用于组织和管理其他可视化元素。
- 3.1.2 Text：`Text` 类型允许你在用户界面中显示静态或动态文本，并提供了许多属性来控制文本的外观、样式和布局。
- 3.1.3 TextInput：`TextInput` 类型允许用户在应用程序中输入文本，并提供了许多属性和信号来管理和响应用户输入。


## 09.QML和C++

- 9.1.1 QML和C++概念：开发者将 QML 的灵活性和易用性与 C++ 的强大功能结合起来。qml专注UI，c++专注业务逻辑。
- 9.1.2 两者为何集成：通过集成，QML 可以专注于界面设计，而 C++ 负责处理复杂的逻辑和性能优化。
- 9.1.3 集成方式说明：
- 9.1.4 有哪些场景用：1.复杂数据处理与可视化；2.数据库与持久化存储；3.本地系统接口访问等等。
- 9.2.1 属性绑定：使用 `Q_PROPERTY` 将C++属性暴露给QML。
- 9.2.2 方法调用：使用`Q_INVOKABLE` 标记可被QML调用的方法。
- 9.2.3 信号-槽系统：C++信号触发QML函数执行。在C++中触发，在Qml中监听函数执行。具体用到`Connections`。
- 9.3.1 将C++注册到QML：本质是**在QML运行环境中暴露C++类的接口**，使这些C++对象能被QML识别、访问和操作。
- 9.3.2 注册上下文属性：将C++对象实例注册为全局可访问属性，`setContextProperty()`
- 9.3.3 注册QML类型：通过 `qmlRegisterType` 或 `qmlRegisterSingletonType`，可以将 C++ 类注册为 QML 类型。
- 9.3.4 生命周期管理：上下文属性这种是持久性服务，全局控制器；注册QML类型是临时组件，视图相关对象。
- 9.3.5 注册调试技术：
- 9.3.7 两种注册比较：暴露一个全局对象，使用 `setContextProperty`。创建可复用的组件或在 QML 中动态创建对象，使用 `qmlRegisterType`。
- 9.4.1 **信号槽设计**-基本通信模型：信号槽机制是支撑QML与C++无缝集成的核心通信系统，其设计允许跨越C++和QML边界进行双向通信。
- 9.4.2 信号槽核心特性：
- 9.4.3 C++到QML通信：1.c++定义温度变化信号槽方法；2.将c++注册到qml；3.在qml中触发c++方法，然后监听信号槽信息。
- 9.4.4 QML到C++通信：1.c++定义登录处理槽函数，登录结果信号；2.注册；2.在qml触发登陆处理函数，使用`Connections`监听登陆结果信号。
- 9.4.5 信号参数处理机制：string转化为QString，list转化为QVariantList，var转化为QVariant等等。
- 9.4.6 高级连接技术：
- 9.4.7 信号槽优化策略：
- 9.4.8 信号槽调试：
- 9.4.9 信号槽最佳实践：C++信号使用camelCase命名（如`dataReceived`）；QML处理器信号名首字母大写加`on`前缀（如`onDataReceived`）。
- 9.5.1 **在C++中操作QML**-获取QML对象：通过对象名查找，QObject *button = rootObject->findChild<QObject*>("submitButton");
- 9.5.2 操作QML属性与方法：1.属性操作，主要是设置属性，读取属性；2.方法调用，主要是调用无参方法，带参方法。
- 9.5.3 信号与事件处理：
- 9.6.1 **用户登录功能**-C++定义登录逻辑：在C++定义登录逻辑，使用`Q_INVOKABLE`。
- 9.6.2 注册并暴露登陆：在 `main.cpp` 中注册并暴露 `AuthManager`。
- 9.6.3 QML实现登录界面：直接调用 authManager.login(username, password)






