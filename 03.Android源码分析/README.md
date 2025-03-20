# 专栏笔记总结大全


## 03.App核心概念分析

**01.App进程启动流程**

- 如何理解Android的根进程：android系统的根进程，主要作用：可以作用Zygote进程fork出SystemServer进程和各种应用进程。
- Zygote进程流程：Zygote进程充当了应用进程的模板，避免了每个应用都需要重新加载和初始化系统类和资源的开销。
- SystemServer进程在Android系统中扮演着重要的角色：负责启动和管理系统的核心服务（比如AMS，PMS，WMS等），初始化系统资源（比如binder驱动），处理系统广播（开机广播，网络状态广播）以及管理系统权限（应用程序权限分配）。
- ActivityThread主要做了什么：应用程序与系统之间的桥梁，确保应用程序的正常运行和用户交互的顺畅体验。比如启动Application，启动主Activity页面，启动Looper轮训，上下文环境管理等等。

**02.ActivityThread**

- 启动Looper线程流程：ActivityThread创建了一个Looper对象，并启动了消息循环。Looper负责管理消息队列和消息的分发，使得应用程序能够接收和处理来自系统和应用的消息。这样，应用程序可以通过Handler机制来实现异步操作、UI更新和线程间通信等功能。


## 06.Android应用窗口

**03.窗口Window设计**

- Window设计思想是什么：用于管理应用程序界面的抽象概念。Window代表了一个可视化的区域，用于承载应用程序的UI内容，并提供与用户交互的能力。




