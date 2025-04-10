# 专栏笔记总结大全


## 01.Android系统的框架

**01.Android组件设计**

- 组件的设计思想：程序由组件组成；组件与进程剥离；组件皆程序入口
- 进程的设计思想：
- AMS是什么东西：AMS（Activity Manager Service）是Android操作系统中的一个核心组件，负责管理应用程序的生命周期、任务栈、进程和应用间的交互等。
- AMS核心设计思想：通过统一管理应用生命周期、任务栈、进程和应用间交互等，使得应用程序能够在Android系统中协调运行。
- 为何要设计Intent？Intent提供了一种统一的机制，使得Android应用程序中的各个组件可以相互协作，共享功能和数据。
- 如何理解内存紧张回收进程设计：设计进程优先级管理、低内存级别、进程重启和资源释放等策略，确保系统能够在有限的内存资源下进行有效的管理和分配。

**06.Android用户界面**



## 03.App核心概念分析

**01.App进程启动流程**

- 如何理解Android的根进程：android系统的根进程，主要作用：可以作用Zygote进程fork出SystemServer进程和各种应用进程。
- Zygote进程流程：Zygote进程充当了应用进程的模板，避免了每个应用都需要重新加载和初始化系统类和资源的开销。
- SystemServer进程在Android系统中扮演着重要的角色：负责启动和管理系统的核心服务（比如AMS，PMS，WMS等），初始化系统资源（比如binder驱动），处理系统广播（开机广播，网络状态广播）以及管理系统权限（应用程序权限分配）。
- ActivityThread主要做了什么：应用程序与系统之间的桥梁，确保应用程序的正常运行和用户交互的顺畅体验。比如启动Application，启动主Activity页面，启动Looper轮训，上下文环境管理等等。

**02.ActivityThread**

- 启动Looper线程流程：ActivityThread创建了一个Looper对象，并启动了消息循环。Looper负责管理消息队列和消息的分发，使得应用程序能够接收和处理来自系统和应用的消息。这样，应用程序可以通过Handler机制来实现异步操作、UI更新和线程间通信等功能。
- 启动Application流程：
- 启动第一个Activity流程：

**03.Context上下文**

- Context设计目的是什么：应用程序可以获取系统资源、启动组件、获取应用程序级别的信息、访问系统服务以及进行安全性和权限管理。这使得应用程序能够与Android系统进行有效的交互和操作。
- 如何通俗化理解Context：可以理解Context为当前对象在程序中所处的一个环境，一个与系统交互的过程，Context在加载资源、启动Activity、获取系统服务、创建View等操作都要参与。
- Context分类设计的意图是什么：这些不同类型的Context提供了不同级别和范围的功能和访问权限，以满足应用程序在不同场景下的需求。
- Context类的代码设计：`Context`下有两个子类，`ContextWrapper`是上下文功能的封装类，而`ContextImpl`则是上下文功能的实现类。
- 为什么不能用Application应用级上下文启动Activity：这是因为非Activity类型的Context并没有所谓的任务栈，所以待启动的Activity就找不到栈了。



## 06.Android应用窗口

**03.窗口Window设计**

- Window设计思想是什么：用于管理应用程序界面的抽象概念。Window代表了一个可视化的区域，用于承载应用程序的UI内容，并提供与用户交互的能力。


## 11.四大组件原理分析

**01.Activity设计和原理**

- Activity架构设计的思想是什么：Activity并不负责视图控制，它只是控制生命周期和处理事件。真正控制视图的是Window，而Window通过定义不同的View组件实现不同的界面展示。
- 包管理服务是做什么的：系统在启动的时候会启动PackageManagerService（包管理服务），PackageManagerService会对应用的AndroidManifest.xml进行解析，从而得到应用里所有的组件信息。
- Activity布局创建流程：在onCreate()方法中，通过setContentView()设置加载View，然后将布局添加到根容器DecorView中。
- Activity布局渲染流程：执行Activity的onResume方法，然后调用Activity#makeVisible()，将mDecor给显示到界面上，然后设置VISIBLE让布局可见。

**02.Activity使用技巧**

- Activity可以new吗？组件并不能采用普通的Java对象创建方式，new一下就能创建实例了，而是要有它们各自的上下文环境。Context是维持Android程序中各组件能够正常工作的一个核心功能类。
- Activity正常生命周期：创建（Create）-> 启动（Start）-> 可见（Resume）-> 暂停（Pause）-> 停止（Stop）-> 销毁（Destroy）。
- Activity三种运行状态设计：前台运行（Foreground）、暂停状态（Paused）和停止状态（Stopped）。
- 横竖屏切换的过程中发生什么：会发生Activity被销毁并重建的过程。两个回调：**onSaveInstanceState和onRestoreInstanceState**。
- onSaveInstanceState()与onPause()的区别：onSaveInstanceState()适用于对临时性状态的保存，而onPause()适用于对数据的持久化保存。
- Activity为何要设计不同的启动模式：可以灵活地控制Activity的启动和管理方式，以满足不同的需求，如界面的单一实例、任务栈的管理、界面的复用等。
- Activity业务设计的思想是什么：通过合理的组织和分离关注点；将界面逻辑和业务逻辑分离；可以使得Activity的代码更加清晰、灵活和易于维护。
