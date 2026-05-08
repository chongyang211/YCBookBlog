# 专栏笔记总结大全


## C++核心原理专栏博客（16篇）

- [01.内存模型与布局](01.内存模型与布局.md)：进程地址空间三层布局、对象内存排列与对齐、虚函数表指针vptr、多重继承内存布局、C++11内存模型与多线程可见性
- [02.引用与指针原理](02.引用与指针原理.md)：引用的汇编实现、引用vs指针本质区别、右值引用底层、引用折叠规则、const引用绑定与延长生命周期
- [03.类与对象机制](03.类与对象机制.md)：类的编译器翻译规则、this指针传递机制、成员函数与非成员函数、访问控制底层、友元与运算符重载
- [04.对象生命周期](04.对象生命周期.md)：构造函数调用顺序、成员初始化列表底层、委托构造、析构顺序与虚析构、对象的存储期与生命周期
- [05.继承与多态](05.继承与多态.md)：单继承与多继承内存模型、虚函数表结构与查找、菱形继承与虚继承、RTTI底层、编译期多态vs运行时多态
- [06.模板与泛型](06.模板与泛型.md)：模板实例化机制、特化与偏特化、SFINAE原理、可变参数模板、编译期计算与类型操作
- [07.STL容器原理](07.STL容器原理.md)：vector扩容策略、deque分段连续、list双向链表、map/set红黑树、unordered_map哈希表、Allocator设计
- [08.STL算法与迭代器](08.STL算法与迭代器.md)：五种迭代器类别、iterator_traits萃取、算法与容器分离设计、sort混合排序、函数对象与谓词
- [09.智能指针与RAII](09.智能指针与RAII.md)：RAII哲学、unique_ptr独占所有权、shared_ptr引用计数、weak_ptr打破循环引用、自定义删除器、make_shared优化
- [10.右值与移动语义](10.右值与移动语义.md)：左值/右值/亡值分类、移动构造与移动赋值、std::move本质、完美转发与std::forward、RVO/NRVO优化
- [11.Lambda与函数式](11.Lambda与函数式.md)：Lambda编译器生成匿名类、捕获列表底层实现、泛型Lambda、std::function类型擦除、函数式编程模式
- [12.异常与错误处理](12.异常与错误处理.md)：从C错误码到C++异常、异常表与零成本异常模型、栈展开机制、noexcept优化、std::expected/std::optional现代方案
- [13.并发与内存序](13.并发与内存序.md)：CPU缓存架构、MESI协议、内存屏障、六种memory_order、原子操作底层、mutex/condition_variable实现
- [14.类型系统与推导](14.类型系统与推导.md)：auto推导规则、decltype推导规则、模板参数推导、CTAD类模板参数推导、类型转换四件套、type_traits
- [15.编译期计算原理](15.编译期计算原理.md)：constexpr函数演进、consteval与constinit、编译期容器、static_assert、if constexpr条件编译
- [16.编译链接与演进](16.编译链接与演进.md)：预处理/编译/链接全流程、ODR规则、模板实例化模型、C++20模块、ABI兼容性、C++版本特性总结


## C++工程实践专栏（8篇）

### 原理篇

- [Cpp编译和链接原理](../04.开发技巧/Cpp编译和链接原理.md)：编译链接问题排查、链接错误分析、Makefile管理编译、头文件依赖处理、ELF结构与符号解析、GOT/PLT/PIE、COMDAT 与模板链接
- [崩溃流程和捕获原理](../04.开发技巧/崩溃流程和捕获原理.md)：崩溃九大原因、硬件异常到信号完整流程、信号处理与sigaction、GDB核心转储分析、崩溃捕获框架设计、DWARF 与 .eh_frame、异步信号安全、进程终止与僵尸
- [调试技巧和原理分析](../04.开发技巧/调试技巧和原理分析.md)：GDB核心技巧、断点/监视点/条件断点、AddressSanitizer/Valgrind/ThreadSanitizer、ptrace底层原理、断点实现机制、TSan 与 happens-before、perf 火焰图采样原理

### 实战篇

- [01.从一次BusError排查Cpp崩溃问题](../04.开发技巧/01.从一次BusError排查Cpp崩溃问题.md)：真实 SIGBUS 排查案例、硬件层真相、si_code 解读、macOS vs Linux 差异、ASan shadow memory 解析
- [GDB与LLDB调试实战手册](../04.开发技巧/GDB与LLDB调试实战手册.md)：命令速查、LLDB 对照、脚本化调试、远程与核心调试、优化代码调试、调试器底层原理
- [Sanitizer全家桶实战](../04.开发技巧/Sanitizer全家桶实战.md)：ASan/UBSan/TSan/MSan/LSan 系统化使用、shadow memory + redzone + quarantine 原理、vector clock、组合使用与 CI 集成、误报抑制
- [内存泄漏排查实战](../04.开发技巧/内存泄漏排查实战.md)：狭义 vs 广义泄漏、LSan mark-sweep、tcmalloc/jemalloc heap profile、ptmalloc2 不归还之谜、智能指针时代新泄漏模式、线上排查方法论
- [性能优化与perf火焰图](../04.开发技巧/性能优化与perf火焰图.md)：perf stat/record/script 三件套、火焰图（On-CPU/Off-CPU/Differential）、PMU/NMI/perf_event_open/LBR/PEBS 原理、四类热点模式与解法
