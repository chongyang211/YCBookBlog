# pos_link POS 上位机链路 SDK · 业务串讲

> **一句话定位**：POS（收银上位机）与模组之间的**通信 SDK**——`说什么`（`pos_link_app.h` 命令字典 0x01~0x15/0xA0~0xAC + Boost.Fusion 结构体）+ `怎么变字节`（`pos_link_app_wire.h` 小端序列化）+ `怎么送达`（帧层：0x7E 转义 + CRC32 + 5s×3 重试 + msg_id 去重）。**双端复用同一份实现**：模组侧跑 `PosLinkServer`（/dev/ttyS2@115200），Android 收银 App 以 `PosLinkClient` 经 AAR（`libpalm_manager.so`）接入。
> **场次**：60 分钟（28 文件 + generated，难点在帧层三件套与双端复用）　**听众**：同组开发 / 评审人 / 后续接手者
> **对照模版**：`06.技术模版/04.业务串讲的模版.md` 第 08 章　**代码基线**：`paymax_device/pos_link/`（2026-10-08 审阅；两条最高严重度结论已实测坐实）

---

## 1. 需求与业务背景  [8min · 先现象后价值]

### 1.1 需求与业务背景
- **来源**：刷掌设备的形态是「模组 + POS 收银机」——识别在模组、交互在 POS（Android App）。两者经串口（/dev/ttyS2）对话：POS 发 StartPalm/BindPalm/加验/HostAck，模组回 0xA3 结果、0xAB 加验请求、0xAC 绑定输入请求。
- **问题**（决定帧层设计的三条）：① 串口是**字节流无边界**——需要帧头/转义/长度/CRC；② 线路有噪声——错误帧要**重同步**而不是断链；③ 上位机 SDK 版本碎片化——**兼容降级**要有通用信号（131/未知 cmd）。
- **目标**：一份协议实现双端复用（消灭双端漂移）；帧层对任意分片/噪声鲁棒；请求-响应带超时重试与去重；文件传输四任务复用同一通道。

### 1.2 业务边界
- **做**：帧层（转义/CRC32/状态机解析/重同步）、请求-响应模型（超时重试/去重/帧层错误码）、App 命令字典（0x01~0x15 + 0xA0~0xAC + 0xAD BiComm）、client/server 双端、文件传输四任务（16KB 分片）、Android AAR 桥。
- **不做**：断点续传/进度回调/整文件 hash（能力缺口，6.2-P15）；多链接并发（单链接模型，新连入驱旧）；命令业务语义（模组侧 pos_request_handler / 上位机 App 层）。

### 1.3 关键业务规则
- **帧格式**（pos_link_protocol.h:82-101，22B 头 + 16B 回包头 + 5B 尾）：`msg_tag=0x7E`（生产）；`msg_type` 0x03=Req/0xFB=Resp；`msg_no` **大端**（唯一大端字段，protocol_test.cpp:62-64 钉死）；CRC32 覆盖 reply_header+data（**不含 header**）。
- **CRC 非标准**：`DoCRC_32` 多项式 0xEDB88320 反射、初值 0、**无终 XOR**——"123456789"→0x2DFD2D88（crc_test.cpp:13-17 钉死）——**与标准 CRC-32/ISO-HDLC 差一个终值 XOR**，双端契约靠测试向量锁死而非标准文档。
- **转义**（PPP 惯例）：`0x7E→7D 5E`、`0x7D→7D 5D`（后缀=值^0x20）；首尾 tag 裸写。
- **可靠性**：无 ack/nak 帧——5s 超时 × 3 重试（protocol.h:25-26）+ 接收端 msg_id 去重窗口 32 条。
- **命令三段**：上位机主动 0x01~0x15 / 模组主动 0xA0~0xAC（0xA3 结果/0xAB 加验/0xAC 绑定输入/0xA8 会话结束）/ 双向 0xAD BiComm；协议版本 v2.1（kPosLinkProtocolVersion=0x0201）。

### 1.4 消费方全景
- **双端复用**（docs/pos-link-framing-and-escaping.md:3 明言）：模组 palmapp（`pos.cmake:8` 链 WxpayPosLink，`pos_transferer.cpp:106-115` 起串口 server）+ Android AAR（palm_manager 顶层构建编入 `libpalm_manager.so`，djinni JNI 桥 client_impl/server_impl）。
- 模组侧业务对偶：`pos_request_handler`（下行处理）+ `pos_notifier`（上行通知）——刷掌串讲已见过 0xA3/0xAB/0xAC/0xA8 的消费现场。

---

## 2. 整体设计与模块划分  [8min · 先图后讲]

### 2.1 架构与分层
```
 「说什么」pos_link_app.h：命令字典 + Req/Resp 结构体 + Fusion 反射
 「怎么变字节」pos_link_app_wire.h：WireReader/Writer/Sizer（小端/前缀长度）
 「怎么送达」pos_link.h(门面, type erasure) → pos_link_impl.h(请求-响应
    状态机 + Reader/Writer 循环) → pos_link_protocol/parsing/writing(帧层)
 client（POS App：Connect/ConnectSerial + AsyncRequest）
 server（模组：BindSerial /dev/ttyS2 单链接，新入驱旧）
 四文件任务（download/receive/send/upload，16KB 分片复用 req/resp 通道）
 ─────────────────────────────────────────────
 双端：Android AAR libpalm_manager.so（Client 为主）
      模组 palmapp（PosLinkServer + pos_request_handler/pos_notifier）
```
分层关键：**三层正交**——命令语义/字节编码/传输送达各一层，可独立演进；**双端同库**是本 SDK 最重要的架构决策——协议 bug 修一处双端同时生效，杜绝「两端各写一份协议」的经典漂移。

### 2.2 核心数据流（一次 StartPalm 的旅程）
```
POS App → AAR(jni) → PosLinkClient.AsyncRequest(PosAppStartPalmReq)
 → Wire 序列化 → 组帧（22B 头 + data + CRC32 + 转义 0x7E）
 → /dev/ttyS2 → 模组 Parser（FindStart→Header→Data→Trailer 状态机，
    任意分片安全，非法转义弃帧重同步）
 → 去重窗口（msg_id）→ pos_request_handler（License 门禁 → RequestEnter）
 → 模组刷掌（刷掌串讲的完整链）→ PosNotifier 0xA3
 → POS App 回调（reply_msg_id 匹配 req_call；5s×3 重试兜底）
```

---

## 3. 逐模块实现讲解  [18min · 业务意图先行]

### 3.1 帧层三件套　pos_link_protocol/parsing/writing
- **Parser 状态机**（pos_parsing.cpp:22-229）：`Init→FindStart→Header→(ReplyHeader)→Data→Trailer`；任意分片安全；**4900ms 无数据复位**（:24-29）；非法转义立即弃帧重同步（:74-80）；header 内再逐字节重同步（:84-109）——**噪声鲁棒性靠两层重同步**。
- **pending_escape_ 中间态**（:231-310）：转义字节对跨 read 分片/跨字段——TAPD bug 1070139249164015474 回归组专门钉死（payload 以 `}}` 结尾、chunk 边界扫掠）。
- **转义与 CRC** 见 1.3；首尾 tag 裸写（MergeAndEscapeData pos_writing.cpp:57-72）。

### 3.2 请求-响应模型　pos_link_impl.h:143-205
- `AsyncRequest` → 入队 `req_calls_` + `AsyncEvent`；等待循环（:157-188）：**timer 到期=真超时**（retry<3 重发），`operation_aborted`=响应到达——**反直觉语义**（steady_timer 模拟事件信号的副作用，注释自认）。
- 响应匹配按 `reply_msg_id`（:378-388）；缓存帧层 `reply_msg_code` 供上层识别「未知 cmd」→ PROTOCOL_x_TOO_OLD 兼容降级。
- 去重窗口 32 条 LRU（:357-371）。

### 3.3 client/server　pos_link_client/server.h/.cpp
- Client：`Connect(ip, 8071)` TCP / `ConnectSerial(path, baud, flow_control)` 8N1；`Start()` 阻塞跑 io_context。
- Server：单链接模型——**新 TCP 连入驱逐旧链接**（server.cpp:138-166）；串口模式唯一链接；生产角色在模组侧（/dev/ttyS2@115200）。
- `*_cmd.cpp`：仅 `POS_LINK_LOCAL_DEVELOP` 宏下的本地联调 demo，**非 SDK 产物**。

### 3.4 文件传输四任务　（16KB 分片）
| 任务 | 模式 | 完整性 |
|---|---|---|
| download | 主动拉（每块 AsyncRequest） | 帧 CRC + chunk_no 顺序（非+1→broken_pipe） |
| receive | 被动收（逐块 Req→回 0/0xFE） | 同上 |
| send | 被动发（收 Req 后 AsyncSendResponse） | 帧 CRC |
| upload | 主动传；`AsReq=true` 走 AsyncRequest **可享帧层重试**，false 走 AsyncSendResponse | 帧 CRC |
- 共性缺口：**无断点续传、无进度回调、无整文件 hash**——completion 只给 `(ec, total_bytes)`；chunk 级完整性全靠帧 CRC + 序号连续。

### 3.5 Android AAR 桥　palm_manager/android/pos_link
- djinni JNI：client_impl/server_impl/request_helper/bicomm_manager/command_dispatch + 生成物 send_methods/register_commands gen.inc；输出 `libpalm_manager.so`（version script 隐藏符号）。

---

## 4. 核心代码实现细节与原理（重点）  [10min · 讲透原理]

### 4.1 深挖①：帧层的不变量（docs 1-25 三条）
1. 线上**永不出现**裸 0x7E（除首尾 tag）/ 0x7D（除转义前缀）；
2. CRC 覆盖 reply_header+data——**接收端可先验 CRC 再决定是否信 header 之外的一切**；
3. 解析器对任意分片/噪声收敛到下一好帧（两层重同步 + 4900ms 复位）。
- **为什么转义而非定长**：串口字节流里 data 可含任意字节（含 JSON、二进制掌纹数据）——转义让 tag 唯一化，帧边界可在流中任意点恢复。

### 4.2 深挖②：双端复用同一份实现
- 模组（server）与 Android（client）链接**同一个静态库 WxpayPosLink**——协议的 source of truth 只有一份。对照教训：若双端各写协议（业界常态），每次改命令双端联调；本仓库把协议 bug 的修复面收敛为一次提交双端生效。
- 代价：SDK 必须**传输无关**（TCP/串口二选一的 type erasure）且**平台无关**（模组 Linux / Android NDK）。

### 4.3 深挖③：兼容降级设计（2026-06-08 protocol-command-compat-design）
- 帧层 `kPosMsgProtocolVersionErr=0x2` 复用为「未知 cmd_id 兜底」+ 131=ADVICE 不支持——**老 SDK 遇新命令**统一回可识别的降级信号而非断链；上位机据此弹「SDK 过旧」提示（PROTOCOL_x_TOO_OLD）。
- 注意 0x2 语义漂移（原义帧头版本不兼容）——排障时同一码两种含义（6.2-P4）。

### 4.4 深挖④：无 ack/nak 的可靠性取舍
- 用「超时重试 + 去重窗口」替代 ack/nak 帧：**少一种帧类型 = 少一类状态机**；代价是重发风暴下窗口外陈旧帧理论上可被二次接受（32 条 LRU 无时间维度，6.2-P18）。对串口点对点低并发场景，取舍成立。

---

## 5. 边界、异常与测试  [5min · 别一笔带过]

### 5.1 边界条件
| 场景 | 行为 |
|---|---|
| 任意分片到达 | 状态机跨 read 安全（pending_escape_） |
| 噪声/坏帧 | 弃帧重同步至下一好帧 |
| 0x7E/0x7D 出现在 data | 转义（首尾 tag 裸写） |
| 未知 cmd（老 SDK） | 131/0x2 降级信号 |
| 重复 msg_id | 32 条去重窗口 |
| 串口缓存脏数据 | StartWithCleanCache（5KB/100ms） |
| 未知 msg_type 帧 | `DataLength()` 返回 0，行为未定义清楚（6.2-P14） |

### 5.2 测试覆盖（帧层是全工程测试最佳之一）
- **44 个用例**：crc_test（4，已知向量钉死非标准 CRC）、protocol_test（11，BCD 时间钉 UTC、msg_no 大端钉死）、parsing_test（19，含 TAPD bug 回归组 + 分片喂入 + 重同步）、writing_test（9）。
- **零覆盖**：client/server 传输层、四文件任务、去重/重试逻辑——macOS 构建干脆排除 client/server.cpp（CMakeLists:19-25）；`#define private public` 白盒手法（parsing_test:11-17）。
- **协议演进史**直接引用 pos_link_app.h:11-21/:39-60 注释块。

---

## 6. 踩坑与接手注意事项（含审阅发现）  [审阅 + 雷区]

### 6.2 接手者雷区地图（⚠️ 本次审阅发现 18 项，P1/P8 已实测坐实）
| # | 现象 | 代码位置 | 风险 / 建议 |
|---|---|---|---|
| **P1** 🔴 | **双份陈旧 gen.h（双 source of truth 残留）**：`pos_link/generated/` 与 `palm_manager/pos_link/generated/` 两份互不一致（缺 0x14/0x15/0xA7；旧版 14 字段 RecognizeResult；AuthState 枚举错位 5=NearExpire vs 5=Expired）；**全仓无任何代码 include**（grep 坐实）——codegen 流程已被手写 pos_link_app.h 取代但产物未清理 | 两处 generated/ | 删除双 gen.h + CONTRIBUTING 指引更新；**别照 gen.h 改代码**（它比手写版旧） |
| **P8** 🔴 | `kPosLinkDefaultPort=8071` 在 client.h:11 与 server.h:17 **同 namespace 重复定义**（实测坐实）——两头同时 include 即 ODR/重定义冲突，目前靠没人同时 include 掩盖 | client.h:11 vs server.h:17 | 挪到公共头 |
| **P6** | `msg_length` 无下溢保护：`0 < msg_length < 27` 的线上帧 → size_t 下溢 → `reserve(huge)` 在 asio 回调抛异常，**Reader 循环无 try/catch** → 链接死亡 | pos_parsing.cpp:44-51/:151 | header 校验加下界（如 ≥27）；Reader 包 try/catch |
| **P3** | 默认 PosLinkConfig（0xF0/0xFF/无转义）与生产（0x7E/0x7E/转义）不一致——**忘 Init 即用错帧格式**；测试默认配置曾掩盖真实 bug（parsing_test:480-482 自认） | pos_link_config.h:8-10 | 默认值改成生产配置，或 Init 前断言 |
| **P7** | 字节序混用：msg_no 大端、msg_length/reply 字段 memcpy 原生序——协议文档未声明，移植大端机静默错乱 | protocol.cpp:30-31 | 文档显式声明；统一走显式序列化 |
| **P15** | 文件传输能力缺口：无断点续传/进度回调/整文件 hash；EOF 标记 `chunk_no=max()` 与计数语义潜在碰撞 | 四 task 头 | 大文件场景先评估；加 chunk 类型宽度断言 |
| **P4** | 0x2 错误码语义漂移（帧版本不兼容 → 复用为未知 cmd） | protocol.h:39-42 | 注释已自认；排障注意双义 |
| **P5** | 超时 ec 反直觉（aborted=响应到达，success=真超时） | pos_link_impl.h:159-189 | 注释已自认；别"修复"它 |
| **P9/P10/P17** | ConfigStore 单例无锁（多线程 TODO）；/dev/ttyS2 与 16KB 分片硬编码；Connect 同步阻塞（卡 UI 风险 App 自负） | 多处 | 各自 TODO 已挂 |
| **P13** | 死条件/死代码：download_file.h:115 `size()>=0` 恒真；空 if；type erasure 残留 shared_ptr | 多处 | 清理批 |
| **P12** | TODO(LXC) 三处 + 拼写 TOOD | receive_file.h:92 等 | 顺手修 |
| **P18** | 去重窗口按条数不按时间（32 LRU）——重发风暴下窗口外陈旧帧可二次接受 | pos_link_impl.h:357-371 | 窗口加时间维度或协议层论证 32 足够 |

**给接手者的硬提醒**：
- 🔴 **改命令先改 pos_link_app.h（手写版）**——gen.h 是旧残骸（P1），别被 generated 名字骗了。
- 🔴 新命令字登记三处：pos_link_app.h 常量 + Fusion 结构体 +（Android 侧）commands.yaml 若重启 codegen 则先清理旧产物。
- 🟡 CRC 是非标准变体（无终 XOR）——**别用标准库 CRC32 替换**，双端契约靠测试向量 0x2DFD2D88 锁死。
- 🟡 parsing_test 的 TAPD 回归组（:484-607）是帧层改动的护栏，改转义/解析前先跑它。

---

## 7. 参考文档
- 模块路径：`paymax_device/pos_link/`（门面/impl/protocol/parsing/writing/client/server/task×4/config/request + generated）
- 协议规范：`docs/pos-link-framing-and-escaping.md`（帧结构表 + 三不变量 + 生产配置速查）+ `pos_link_app.h` 注释（命令层 + 演进史）+ `palm_manager/docs/specs/2026-06-08-protocol-command-compat-design.md`（兼容降级）
- 双端消费方：模组 `pos_transferer.cpp`（server）/`pos_request_handler`/`pos_notifier`（刷掌串讲 3.4 的对偶）；Android `palm_manager/android/pos_link`（djinni 桥）
- codegen 残骸：`palm_manager/scripts/generate_commands.py` + commands.yaml（流程已废，P1）
- 相关串讲：《刷掌功能》（0xA3/0xAB/0xAC/0xA8 消费现场）、《指令功能》（captureLog 提到的 pos 链）、《启动应用》（监督塔之上的业务层）
