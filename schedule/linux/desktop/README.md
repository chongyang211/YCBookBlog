# PosLink 桌面上位机文档系列

> 系列入口。范围：`poslink-desktop-demo/`（Electron 桌面上位机 Demo）+ `pos_link/c_api/`（C ABI 封装层）——刷掌模组在 Windows / macOS 收银机侧的上位机接入路径。模组侧 POS 协议门面见 `.idea/docs/pos/` 系列。

## 一、这套东西是什么

刷掌协议栈只有 C++ 一份实现（`pos_link/` 核心）。Android 走 Djinni/JNI AAR，桌面侧无法复用。本系列覆盖为此新建的两层：

```text
poslink-desktop-demo (Electron)          pos_link/c_api
┌──────────────────────────────┐        ┌──────────────────────────┐
│ Renderer (Next.js 静态导出)   │        │  纯 C11 动态库            │
│   ↕ preload contextBridge    │        │  libposlink.dylib /      │
│ Main (IPC + 日志 + 存储)      │  koffi │  pos_link_c_api.dll      │
│   ↕ utilityProcess RPC       │ ─────► │  封装 C++ 核心            │
│ Bridge Worker (唯一 FFI 接触点)│        │  (WxpayPosLink 核心零改动) │
└──────────────────────────────┘        └───────────┬──────────────┘
                                                    ↕ 串口 0x 帧
                                                 O41 刷掌模组
```

设计原则：协议栈单实现（升级零成本）、C ABI 作为 FFI 最大公约数、子进程隔离（FFI 崩溃不伤主进程，可一次性重生）。

## 二、分篇目录

| 篇 | 文件 | 状态 | 主题 |
|---|---|---|---|
| 01 | 《01.PosLink桌面上位机全流程分析.md》 | ✅ | 快速通读：连接→握手→刷掌识别的端到端调用链 |
| 02 | 《02.总览与架构.md》 | ✅ | 系列总览：三层架构、组件表、S1/S2 分工边界 |
| 03 | 《03.C_API契约与ABI设计.md》 | ✅ | 17 个 `pl_*` 导出、ABI 指纹断言、符号门禁、构建与产物 |
| 04 | 《04.线程模型与内存契约.md》 | ✅ | Asio io 线程、回调 StrPool、one-shot io_context、同步拷贝铁律 |
| 05 | 《05.原生桥接层.md》 | ✅ | `electron/sdk/`：koffi 声明、回调桥、worker/client、ABI 校验、native staging |
| 06 | 《06.IPC与进程模型.md》 | ✅ | main/preload/renderer 划分、IPC 通道全表、单实例锁、worker 重生 |
| 07 | 《07.业务流程与前端状态机.md》 | ✅ | 连接梯、识别流程、AppError 错误模型、ret_code 映射、日志导出 |
| 08 | 《08.协议字典与TS镜像.md》 | ✅ | `protocols/index.ts` 与 C 头文件逐节对应、镜像同步规则 |
| 09 | 《09.测试与质量门禁.md》 | ✅ | 四层门禁全景、PTY 双真实栈集成、无头 e2e、audit 扫描器、`release:check` 链、已知缺口 |
| 10 | 《10.风险与遗留事项.md》 | ⬜ | 平台覆盖现状、已知限制、手工同步风险点 |

## 三、快速导航

- 只想了解整体：读 01 + 02。
- 要接 C API（非 Node 宿主）：读 03 + 04。
- 改 Demo 前端业务：读 07 + 08。
- 排查发布/打包问题：读 05 + 09 + 10。

模组侧协议细节（0x7E 帧、0x 指令字典、识别链）不在本系列范围内，见 `.idea/docs/pos/` 系列。
