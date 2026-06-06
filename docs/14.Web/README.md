---
title: README
date: 2025-11-26 15:24:30
permalink: /pages/e04cda/
categories:
  - book
  - Web开发和进阶
tags:
  - 
author: 
  name: 杨充
  link: https://github.com/xugaoyi
---



我需要实现离线演示demo。我的入口文件是：DemoMainSession

模式0：回归真实模式，这个直接是 PaymentMainSession 中真实的操作；
其中模式1到模式8，都不会访问网络。相当于代替 requestPalmRecognize，网络请求为mock方式，通过mock组装返回的 recognizeRes 数据。其中 handleQRRegister 也通过mock组装返回result数据。

模式1：未录掌-录掌失败-服务异常
模式2：未录掌-录掌失败-非服务异常
模式3：未录掌-录掌成功-OTC请求异常
模式4：未录掌-录掌成功-OTC返回成功
模式5：已录掌-OTC请求异常
模式6：已录掌-OTC请求成功
模式7：中止支付-中止失败-当前正处于支付中/录掌中/识别中
模式8：断网模拟

针对模式0到模式8，帮我抽象策略接口，接口中必须有 mockRequestPalmRecognize，mockHandleQRRegister。策略接口中，包含策略id，策略名称，策略描述等等，策略接口满足基本要求就行，不要设计太复杂。

模式0，其实就是真实的实现，相当于在 mockRequestPalmRecognize，mockHandleQRRegister 具体实现中，实现现在真实业务逻辑操作。
模式1到模式8，都是假实现，具体策略子类可以先空实现。帮我实现这个架构的设计和代码完善。所有代码写在 main-sessions/demo 这个目录下。
