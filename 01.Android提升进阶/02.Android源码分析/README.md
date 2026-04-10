# 专栏笔记总结大全


## 01.Android系统的框架

- 组件的设计思想：程序由组件组成；组件与进程剥离；组件皆程序入口
- 进程的设计思想：
- AMS是什么东西：AMS（Activity Manager Service）是Android操作系统中的一个核心组件，负责管理应用程序的生命周期、任务栈、进程和应用间的交互等。
- AMS核心设计思想：通过统一管理应用生命周期、任务栈、进程和应用间交互等，使得应用程序能够在Android系统中协调运行。
- 为何要设计Intent？Intent提供了一种统一的机制，使得Android应用程序中的各个组件可以相互协作，共享功能和数据。
- 如何理解内存紧张回收进程设计：设计进程优先级管理、低内存级别、进程重启和资源释放等策略，确保系统能够在有限的内存资源下进行有效的管理和分配。




## 03.App核心概念分析

- **1.如何理解Android的根进程**：android系统的根进程，主要作用：可以作用Zygote进程fork出SystemServer进程和各种应用进程。
- Zygote进程流程：Zygote进程充当了应用进程的模板，避免了每个应用都需要重新加载和初始化系统类和资源的开销。
- SystemServer进程在Android系统中扮演着重要的角色：负责启动和管理系统的核心服务（比如AMS，PMS，WMS等），初始化系统资源（比如binder驱动），处理系统广播（开机广播，网络状态广播）以及管理系统权限（应用程序权限分配）。
- ActivityThread主要做了什么：应用程序与系统之间的桥梁，确保应用程序的正常运行和用户交互的顺畅体验。比如启动Application，启动主Activity页面，启动Looper轮训，上下文环境管理等等。
- **2.启动Looper线程流程**：ActivityThread创建了一个Looper对象，并启动了消息循环。Looper负责管理消息队列和消息的分发，使得应用程序能够接收和处理来自系统和应用的消息。这样，应用程序可以通过Handler机制来实现异步操作、UI更新和线程间通信等功能。
- 启动Application流程：
- 启动第一个Activity流程：
- **3.Context设计目的是什么**：应用程序可以获取系统资源、启动组件、获取应用程序级别的信息、访问系统服务以及进行安全性和权限管理。这使得应用程序能够与Android系统进行有效的交互和操作。
- 如何通俗化理解Context：可以理解Context为当前对象在程序中所处的一个环境，一个与系统交互的过程，Context在加载资源、启动Activity、获取系统服务、创建View等操作都要参与。
- Context分类设计的意图是什么：这些不同类型的Context提供了不同级别和范围的功能和访问权限，以满足应用程序在不同场景下的需求。
- Context类的代码设计：`Context`下有两个子类，`ContextWrapper`是上下文功能的封装类，而`ContextImpl`则是上下文功能的实现类。
- 为什么不能用Application应用级上下文启动Activity：这是因为非Activity类型的Context并没有所谓的任务栈，所以待启动的Activity就找不到栈了。
- **5.Intent设计思想是什么**：
- 为什么要设计Bundle：把数据封装至bundle对象中，这个是用来装数据的。而Intent负责四大组件之间的通信交互！



## 06.Android应用窗口

**03.窗口Window设计**

- Window设计思想是什么：用于管理应用程序界面的抽象概念。Window代表了一个可视化的区域，用于承载应用程序的UI内容，并提供与用户交互的能力。


## 11.四大组件原理分析

- Activity架构设计的思想是什么：Activity并不负责视图控制，它只是控制生命周期和处理事件。真正控制视图的是Window，而Window通过定义不同的View组件实现不同的界面展示。
- 包管理服务是做什么的：系统在启动的时候会启动PackageManagerService（包管理服务），PackageManagerService会对应用的AndroidManifest.xml进行解析，从而得到应用里所有的组件信息。
- Activity布局创建流程：在onCreate()方法中，通过setContentView()设置加载View，然后将布局添加到根容器DecorView中。
- Activity布局渲染流程：执行Activity的onResume方法，然后调用Activity#makeVisible()，将mDecor给显示到界面上，然后设置VISIBLE让布局可见。
- Activity可以new吗？组件并不能采用普通的Java对象创建方式，new一下就能创建实例了，而是要有它们各自的上下文环境。Context是维持Android程序中各组件能够正常工作的一个核心功能类。
- Activity正常生命周期：创建（Create）-> 启动（Start）-> 可见（Resume）-> 暂停（Pause）-> 停止（Stop）-> 销毁（Destroy）。
- Activity三种运行状态设计：前台运行（Foreground）、暂停状态（Paused）和停止状态（Stopped）。
- 横竖屏切换的过程中发生什么：会发生Activity被销毁并重建的过程。两个回调：**onSaveInstanceState和onRestoreInstanceState**。
- onSaveInstanceState()与onPause()的区别：onSaveInstanceState()适用于对临时性状态的保存，而onPause()适用于对数据的持久化保存。
- Activity为何要设计不同的启动模式：可以灵活地控制Activity的启动和管理方式，以满足不同的需求，如界面的单一实例、任务栈的管理、界面的复用等。
- Activity业务设计的思想是什么：通过合理的组织和分离关注点；将界面逻辑和业务逻辑分离；可以使得Activity的代码更加清晰、灵活和易于维护。

## 13.View绘制流程设计

- performMeasure核心思路是什么：这是View测量入口，通过递归地测量子 View，整个 View 树能够正确地计算出每个 View 的大小和位置，从而实现正确的布局和显示效果。
- 测量中MeasureSpec设计意图：大小（Size）和模式（Mode），用于指定 View 在父容器中的大小限制和测量模式。让开发者能够更好地控制 View 的大小和布局，同时保证 View 在不同情况下都能正确地测量和显示。
- MeasureSpec值设计是什么：MeasureSpec用一个32位int值来表示布局要求描述。前2位代表了测量模式，后30位则表示了测量的大小，对于模式和大小值的获取，只需要通过位运算即可。
- 单个View测量流程是什么：经过measure() -> onMeasure() -> setMeasuredDimension()函数的调用，最终View自身测量流程执行完毕。



## 20.IPC通信机制设计

- IPC设计意图是什么：不同应用程序或组件之间进行通信和共享数据。选择适当的IPC机制来实现不同组件或应用程序之间的通信。
- IPC机制有哪些种类：Intent用于不同组件通信，Binder用于跨进程通信，ContentProvider数据共享通信，Broadcast不同组件广播通信
- 常见进程间通信有哪些：1.内存共享，2.管道通信，3.消息队列，4.Socket通信。包含Android中的binder通信设计
- 为什么要设计Binder通信：1.性能这块相比传统的Socket更高效，2.安全性高支持通信双方进行身份验证
- 传统的IPC通信机制为何不安全：首先传统IPC的接收方无法获得对方进程可靠的UID和PID（用户ID进程ID），从而无法鉴别对方身份
- 为何说Binder机制通信安全：Android中PID和UID是由内核填写的，所以调用者进程不能通过伪造身份标识来获取超出系统允许的权限。
- Binder使用场景有哪些：调用activity启动，
- 总结一下Binder是什么：对于服务端进程来说Binder指的是Binder本地对象，对于客户端进程来说Binder指的是Binder代理对象。对于传输过程来说Binder是可以进行跨进程传递的对象。
- 举例说Binder通信：




