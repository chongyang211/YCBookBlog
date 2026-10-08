# facility/link 本地 P2P 链路 · 业务串讲

> **一句话定位**：**设备巡检专用**的本地 P2P 命令通道——产测工具经 PL2303 USB 串口接入，`P2pLink` 把 local_link SDK 包成「按需起停」的通道（巡检期间才初始化，正常运营零占用，与 pos_link 互斥）；`LocalLink` 只做一次性日志接线。**注意区分**：本模块是本地 USB 串口 P2P，**不是**网络库里的 iLink 长链（`DualLinkTdiRequest` 是未实现的 stub）——同名「link」两回事。
> **场次**：25-35 分钟（4 文件约 290 行，小而精，亮点在线程正例与 phylink 调查史）　**听众**：同组开发 / 评审人 / 后续接手者
> **对照模版**：`06.技术模版/04.业务串讲的模版.md` 第 08 章　**代码基线**：`palm_app_linux/src/facility/link/`（4 文件全量精读，2026-10-08 审阅）

---

## 1. 需求与业务背景  [6min · 先现象后价值]

### 1.1 需求与业务背景
- **来源**：产线/运维需要用**产测工具**（host PC）对设备做巡检（DeviceInspectController 场景）——通过 USB 串口（PL2303 适配器）建立 P2P 命令通道，payload 为每 FID 一条 JSON。
- **问题**：① 串口是**稀缺独占资源**——正常运营期间 pos_link（上位机 POS 通信）要用 USB，两者必须互斥，巡检结束必须把串口还给系统（重启前释放）；② SDK 是同步 C 函数集合，直接散调会横跨线程；③ SDK 日志要接入工程统一 logger。
- **目标**：巡检期间按需起停；SDK 调用全串行化；回调统一转主线程；SDK 日志进工程日志。

### 1.2 业务边界
- **做**：P2P 通道起停（Init/UnInit 幂等）、FID 注册/回调分发、异步写（completion-token）、双连接态查询（逻辑/物理）、phylink 启停、company-bus 常驻模式、SDK 日志接线。
- **不做**：产测业务协议（payload JSON 由 DeviceInspectController 定义）；msg_id 配对（**SDK 在 FID 通道内管 request/response pairing**，p2p_link.h:21-22 注释）；CH9329/UART 线路选择（SDK 内部行为，见 4.4 调查史）。

### 1.3 关键业务规则
- **互斥规则**（local_link.h:10-12 + p2p_link.h:19-21）：pos_link 与 p2p_link **互斥**，SDK 只在巡检期间初始化——**正常运营 local_link_init_p2p 从不调用**。
- **双连接态**：`IsConnected`（安全握手后的逻辑连接）vs `IsPhyConnected`（物理串口连接）——两层语义，调试时先看物理再看逻辑。
- **company-bus 分叉**（:39-44）：`PALM_BIZ_COMPANY_BUS` 构建下 phylink **常驻**（Init 即 enable、UnInit 不 disable，`is_need_enable_phylink_`）。
- **phylink 设备**：`kPhylinkDeviceName="TTYUSB_PL2303"`、type `"P2P"`（:11-12）。

### 1.4 调用方
- `DeviceInspectController`：Enter→`InitForDeviceInsepect(fids)`、stop→`UnInit()`；巡检命令走 `WriteAsync`/`AddReceiveDataCallback`。

---

## 2. 整体设计与模块划分  [4min · 先图后讲]

### 2.1 架构与分层
```
 DeviceInspectController（巡检业务）
        │ InitForDeviceInsepect / WriteAsync / AddReceiveDataCallback
 ┌──────┴───────────────────────────────────────┐
 │ P2pLink（单例，专用 "p2p_link" 线程）           │
 │  Init：EnsureInit(日志) → local_link_init_p2p  │
 │        → 注册 FID 回调 → enable phylink        │
 │  Send/UnInit/phylink 全部 post 到本线程串行     │
 │  OnDataReceived → MainThread CoSpawn 分发       │
 │  ConnectionStateChangeSig（MainThread 触发）    │
 ├──────────────────────────────────────────────┤
 │ LocalLink（单例，仅一次性日志接线）              │
 │  local_link_logger_set → 工程 logger           │
 ├──────────────────────────────────────────────┤
 │ local_link SDK（第三方，同步 C API）             │
 │  PL2303 / CH9329 / UART_P2P 多线路管理          │
 └──────────────────────────────────────────────┘
```
分层关键：**「业务协议归业务、通道机制归本模块」**——p2p_link 只管起停/串行化/线程转递，payload 语义零沾染；**SDK 的同步 C API 被线程化封装整体隔离**（本模块是 Keystore K1 的反面——那里给了线程没用，这里给了而且全用上了）。

### 2.2 模块职责
| 组件 | 职责 |
|---|---|
| `LocalLink` | 一次性 SDK 日志接线（idempotent，仅 friend P2pLink 可调） |
| `P2pLink` | 通道生命周期 + FID 分发 + 线程化 + phylink 启停 |
| `Thread("p2p_link")` | SDK 调用串行化（Init/Send/UnInit/phylink 全走此线程） |

---

## 3. 逐模块实现讲解  [10min · 业务意图先行]

### 3.1 `LocalLink`：最小封装　local_link.cpp:18-34
- 只做 `local_link_logger_set`（SDK 0-trace..4-error 与工程 LogLevel 1:1 对齐，:21-22）；`EnsureInit` 幂等（atomic flag）；**protected + friend P2pLink**——外部不能触发 SDK 日志接线（收口）。

### 3.2 `P2pLink::Init/UnInit`：按需生命周期　p2p_link.cpp:21-67/:127-138
- **Init**（:21-61）：p2p 线程内——首次则 `EnsureInit → local_link_init_p2p → 连接态回调注册`（逻辑态转 MainThread :27-35，物理态仅日志 :36-38）+ company-bus 分支（:39-44）；**无论首不首次都注册本次 fids**（:50-59，幂等重入语义）。
- **UnInit**（:127-138）：调用线程清 callback_map_（:129）+ p2p 线程内复位 has_init_、条件 disable phylink（**非 company-bus 才释放串口**，:132-136）——重启前把串口还给系统的语义。

### 3.3 双通道读写
- **写**（DoWriteAsync/WriteAsync，p2p_link.h:56-74）：completion-token——post 到 p2p 线程执行 SDK send，**返回码带回调用方 executor**（与 ThreadPool::AsyncExec、Keystore Async 三兄弟同款）。
- **读**（OnDataReceived，:140-153）：SDK 回调线程 → **MainThread CoSpawn** 再查 `callback_map_` 分发——回调统一在主线程执行（业务侧免锁心智负担）。

### 3.4 `SetPhylinkEnable` 与调查史　:100-125（见 4.4 深挖）

---

## 4. 核心代码实现细节与原理（重点）  [8min · 讲透原理]

### 4.1 深挖①：按需生命周期与互斥
- **为什么巡检期间才初始化**：USB 串口在正常运营归 pos_link（上位机通信）用——若 local_link SDK 常驻初始化，会占线/抢资源。**「通道的生命周期 = 业务会话的生命周期」**（Enter↔stop），不是进程级常驻——与 MqttService（常驻）形成两极对照。
- UnInit 的释放语义：`local_link_disable_phylink` 让串口在**重启前**归还系统——否则驱动状态残留影响下次枚举。

### 4.2 深挖②：线程模型（本模块是全工程的正例）
- **SDK 同步 C API 三处线程化**：调用（Init/phylink/UnInit）→ p2p 线程 post；写（Send）→ DoWriteAsync 桥；读（回调）→ MainThread CoSpawn。
- **与 Keystore K1 对照**：Keystore 给了 se 线程但同步接口散落 4 类线程（零调用方死资源）；本模块**所有 SDK 触点都收敛到一条线程**——同一工程两种命运，接手模板照抄这里。
- FID pairing 交给 SDK（h:21-22 注释）：**通道内请求/响应配对是 SDK 职责**，模块不引入 msg_id——少一层状态 = 少一类 bug。

### 4.3 深挖③：双连接态的调试价值
- `IsConnected`（含安全握手结果）≠ `IsPhyConnected`（串口在不在）——**巡检连不上先分清哪层断**：物理断（换线/重插）vs 逻辑断（握手失败看 `ConnectionStateChangeSig` 的 msg）。
- 逻辑态回调**转主线程**再发信号（:29-34）——订阅者（UI/业务）统一线程语境。

### 4.4 深挖④：phylink 调查史（:105-116，本模块最重要的一段注释）
```
SDK 内部：CH9329_P2P（/dev/ttyCH29，优先级 99）预载在 UART_P2P
（/dev/tty_uart_p2p → ttyS2，优先级 103）之上。
Host 未在 open 时检测到 → UART connect:false → SDK 把待机活动线交给 CH9329 并关闭 ttyS2。
Plan A（local_link_disable_phylink("CH9329","P2P") + 放弃 PL2303 enable，让 UART 成为唯一 p2p 线）
  → 实现并验证过（disable 后 SDK 重开 UART）
  → 又回滚：P20 连不上追因到 UART connect 检测对 P20 接线永不置位
    （P10 立即置位；connect:false 时重开的 UART 无视对端字节）。
结论：Plan A 搁置，等 connect 检测根因解决后再启。
```
- **为什么值得整段讲**：这是**保留调查轨迹的决策记录**（不是垃圾注释）——记录了现象（P20 vs P10 差异）、根因路径（connect detection）、做过什么（Plan A 实现→验证→回滚）、重启条件。**接手者的行动项**：P20 UART connect 检测根因未解决，Plan A 处于搁置——**清理代码时千万别当 dead code 删**（6.2-L4）。

---

## 5. 边界、异常与测试  [4min · 别一笔带过]

### 5.1 边界条件
| 场景 | 行为 |
|---|---|
| 重复 InitForDeviceInsepect | 幂等（has_init_ 守卫 + 重复注册 fids :48-49） |
| company-bus 下 UnInit | 不释放 phylink（常驻，:132 条件） |
| SetPhylinkEnable 重复调用 | has_enable_phylink_ 守卫静默跳过（:101-104） |
| 未注册 FID 的数据到达 | `callback not found` LOG_E（:148） |
| 巡检中拔线 | 物理态回调仅日志；逻辑态走信号 |
| P20 接线 | UART connect 检测不置位（4.4 已知未解） |

### 5.2 测试覆盖
- **零单测**（模块小、依赖真实串口，可理解）；可单测的部分（幂等守卫、company-bus 分支逻辑）也没写。回归靠产线工具联调。

---

## 6. 踩坑与接手注意事项（含审阅发现）  [审阅 + 雷区]

### 6.2 接手者雷区地图（⚠️ 本次审阅发现）
| # | 现象 | 代码位置 | 风险 / 建议 |
|---|---|---|---|
| **L1** | **公开 API 拼写错误**：`InitForDeviceInsepect`（Insepect→Inspect）——已是对外契约，改名要带调用方一起改 | p2p_link.h:37/:63 | 新代码别再扩散此拼写；改名走一次性重构 |
| **L2** | `callback_map_` 无锁跨线程：Add/Remove 在**调用线程**写（:78/:85），OnDataReceived 在 **MainThread** 读（:144）——std::map 并发读写形式上是数据竞争；当前调用方（DeviceInspectController）恰在 MainThread 才安全，**线程契约未声明** | p2p_link.cpp:76-86/:140-153 | 与 Keystore K1 同族：要么文档声明「Add/Remove 须在 MainThread」，要么挪进 p2p 线程 |
| **L3** | `RevieveCallback` 类型名 typo（Revieve→Receive） | p2p_link.h:25 | 内部类型，清理顺带 |
| **L4** | **Plan A 调查注释（:105-116）是决策记录不是死代码**——清理"无用代码"时误删 = 丢失 P20/P10 差异与重启条件的唯一记录 | :105-116 | 保留；或迁至 docs/ 并在原处留指引 |
| **L5** | `Send`/`OnDataReceived` 的 LOG_I 打完整 payload（:70/:143）——巡检 JSON 量大时日志噪音；若 payload 引入敏感字段则成泄露面 | :69-73/:143 | 降为 LOG_D 或截断打印 |
| **L6** | UnInit 双线程分工无同步点：clear 在调用线程、has_init_ 复位在 p2p 线程（post FIFO 保证顺序，**依赖同线程队列有序**这一隐式契约） | :127-138 | 语义自洽但脆弱——全部收进 p2p 线程更稳 |

**给接手者的硬提醒**：
- 🟡 **P20 UART connect 检测根因（4.4）是本模块唯一悬而未决的问题**——动 phylink/线路选择前先读那段注释，别重蹈 Plan A 覆辙。
- 🟡 新增 FID：业务侧只 `AddReceiveDataCallback(fid, cb)` + `InitForDeviceInsepect({fid,...})`——payload 协议归业务定义，本模块不加字段。

---

## 7. 参考文档
- 模块路径：`palm_app_linux/src/facility/link/`（local_link.h/.cpp + p2p_link.h/.cpp）
- 依赖：`local_link_api.h`（第三方 SDK，同步 C API）、`facility/thread`（专用 p2p_link 线程 + MainThread）
- 调用方：`DeviceInspectController`（巡检业务，Enter/stop 生命周期之源）
- 对照阅读：《Keystore功能-模块业务串讲》K1（线程化封装的反例，本模块是正例）、《网络库-模块业务串讲》（iLink stub——与本模块「同名不同物」）、《线程功能》（completion-token 三兄弟：ThreadPool::AsyncExec / Keystore Async / 本模块 DoWriteAsync）
