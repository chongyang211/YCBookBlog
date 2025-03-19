# 专栏笔记总结大全


## 03.App核心概念分析

**01.App进程启动流程**

- 如何理解Android的根进程：android系统的根进程，主要作用：可以作用Zygote进程fork出SystemServer进程和各种应用进程。
- Zygote进程流程：
- SystemServer进程在Android系统中扮演着重要的角色：负责启动和管理系统的核心服务（比如AMS，PMS，WMS等），初始化系统资源（比如binder驱动），处理系统广播（开机广播，网络状态广播）以及管理系统权限（应用程序权限分配）。

