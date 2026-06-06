---
title: README
date: 2026-04-13 21:10:07
permalink: /pages/436764/
categories:
  - book
  - Web开发和进阶
  - 专栏博客
tags:
  - 
author: 
  name: xugaoyi
  link: https://github.com/xugaoyi
---
# 专栏笔记总结大全


## JavaScript核心原理专栏博客（12篇）

- [01.JavaScript引擎与运行时原理](01.JavaScript引擎与运行时原理.md)：V8架构演进、JIT编译策略、AST与字节码、Ignition解释器与TurboFan优化编译器、隐藏类与内联缓存、对象存储模型、数值Smi优化
- [02.执行上下文与作用域链原理](02.执行上下文与作用域链原理.md)：三种执行上下文、执行上下文栈、词法环境与变量环境、作用域链形成机制、变量提升底层原理、TDZ暂时性死区、闭包的V8实现
- [03.原型链与继承机制原理](03.原型链与继承机制原理.md)：原型链完整模型、属性查找算法、new底层四步骤、五种继承方式演进对比、class语法糖本质、Mixin模式、原型污染防护
- [04.类型系统与类型转换原理](04.类型系统与类型转换原理.md)：8种数据类型、typeof实现原理与类型标签历史、ToPrimitive算法、ToNumber/ToString/ToBoolean规则、==算法逐步推导、NaN/-0/Infinity设计
- [05.异步编程与事件循环原理](05.异步编程与事件循环原理.md)：事件循环完整模型、微任务与宏任务优先级、Promise状态机实现、async/await=Generator+Promise、定时器4ms限制、rAF与渲染时机、Node.js事件循环差异
- [06.this绑定与函数调用原理](06.this绑定与函数调用原理.md)：四种绑定规则及优先级、隐式丢失陷阱、箭头函数词法this、手写call/apply/bind、[[Call]]/[[Construct]]内部方法、尾调用优化
- [07.内存管理与垃圾回收原理](07.内存管理与垃圾回收原理.md)：V8分代堆结构、新生代Scavenge算法、老生代Mark-Sweep/Compact、增量与并发标记、七种内存泄漏模式、WeakRef/FinalizationRegistry、对象池与TypedArray
- [08.模块系统演进与原理](08.模块系统演进与原理.md)：模块化演进史（全局→IIFE→CommonJS→AMD/CMD→ESM）、require实现原理、ESM三阶段加载、Live Binding vs值拷贝、循环依赖处理、Tree Shaking原理
- [09.Proxy元编程与响应式原理](09.Proxy元编程与响应式原理.md)：13种Proxy拦截器、Reflect对偶关系、defineProperty局限性、Vue2与Vue3响应式对比、手写响应式系统、数据验证与观察者模式
- [10.迭代协议与生成器原理](10.迭代协议与生成器原理.md)：迭代器与可迭代协议、Generator状态机实现、yield双向通信、协程原理、异步迭代器、惰性求值管道、async/await的Generator本质
- [11.正则引擎与字符串处理原理](11.正则引擎与字符串处理原理.md)：UTF-16与代理对、V8字符串内部表示、模板字面量与标签模板、NFA vs DFA引擎、回溯机制、灾难性回溯防护、Unicode正则、命名捕获组
- [12.DOM事件与浏览器渲染原理](12.DOM事件与浏览器渲染原理.md)：Chrome多进程架构、关键渲染路径、DOM/CSSOM构建、布局与绘制与合成、重排重绘优化、事件三阶段模型、事件委托、Observer API、Core Web Vitals
