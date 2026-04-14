# 专栏笔记总结大全


## Java核心原理专栏博客（12篇）

- [01.JVM内存模型与对象生命周期](01.JVM内存模型与对象生命周期.md)：JVM运行时数据区、对象创建与内存布局、对象生命周期与可达性分析、堆内存分代设计、逃逸分析与栈上分配
- [02.类加载机制与双亲委派模型](02.类加载机制与双亲委派模型.md)：类的生命周期五阶段、三层类加载器体系、双亲委派源码原理、打破双亲委派的SPI/OSGi/热部署
- [03.垃圾回收算法与GC调优原理](03.垃圾回收算法与GC调优原理.md)：标记-清除/复制/标记-整理算法、Serial到ZGC收集器演进、GC日志分析与调优策略、三色标记与SATB
- [04.HashMap底层原理与哈希设计](04.HashMap底层原理与哈希设计.md)：hash扰动函数设计、put/resize源码剖析、容量2的幂/负载因子0.75/树化阈值8的数学原理、并发安全问题
- [05.String不可变性与常量池原理](05.String不可变性与常量池原理.md)：String底层char到byte演进、不可变性三重保护、常量池位置变迁与intern原理、字符串拼接编译优化
- [06.Java泛型擦除与类型系统原理](06.Java泛型擦除与类型系统原理.md)：类型擦除机制、桥接方法、泛型边界与限制、PECS原则、运行时获取泛型信息
- [07.反射机制与动态代理底层原理](07.反射机制与动态代理底层原理.md)：反射调用链与Inflation优化、JDK动态代理Proxy类生成、CGLIB继承代理、Spring AOP选择策略
- [08.并发编程之synchronized与锁升级](08.并发编程之synchronized与锁升级.md)：对象头Mark Word、偏向锁/轻量级锁/重量级锁原理、锁消除/锁粗化/自适应自旋、虚拟线程Pinning问题
- [09.volatile与Java内存模型JMM](09.volatile与Java内存模型JMM.md)：CPU缓存架构与MESI协议、JMM抽象模型、happens-before规则、volatile内存屏障实现、DCL单例分析
- [10.线程池核心原理与源码设计](10.线程池核心原理与源码设计.md)：七大参数、ctl状态控制、execute提交流程、Worker线程复用秘密、ForkJoinPool工作窃取、参数设置策略
- [11.IO模型演进：BIO到NIO到AIO](11.IO模型演进之BIO到NIO到AIO.md)：五种IO模型对比、NIO三大组件、Selector多路复用、select/poll/epoll底层对比、零拷贝原理、Reactor模式与Netty
- [12.Java异常体系与JVM异常处理机制](12.Java异常体系与JVM异常处理机制.md)：异常表字节码结构、栈展开机制、finally代码复制原理、异常性能代价、try-with-resources
