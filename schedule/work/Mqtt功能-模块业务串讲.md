# service/mqtt 长连接功能 · 业务串讲

> **一句话定位**：设备与后台的 **MQTT 长连接通道**——TLS + SE 国密签名认证接入 `link-{host}:443`，订阅 `down/{sn}`（云端实时推送）/ `resp/{sn}`（请求响应）；用「QoS1 + 去重窗口 + 重连补拉未 ack 消息」补偿 broker 不为离线设备保留推送的语义空缺；HTTP 短链（网络库/属性功能串讲）出问题时的兜底通道由本模块的连接态信号反向调节。
> **场次**：60 分钟（7 个文件，核心全在 `MqttClientPrivate` 约 600 行）　**听众**：同组开发 / 评审人 / 后续接手者
> **对照模版**：`06.技术模版/04.业务串讲的模版.md` 第 08 章　**代码基线**：`palm_app_linux/src/service/mqtt/`（2026-10-08 审阅，7 文件全量精读 + 上游调用方核对）

---

## 1. 需求与业务背景  [10min · 先现象后价值]

### 1.1 需求来源与问题
- **来源**：HTTP 短链是「设备问、云端答」——云端**主动推**（OTA 任务、平台指令、cloud-side 属性、支付/会员会话）没有长连接就做不到；短链轮询延迟与流量都不可接受。
- **问题**（三条决定了代码里的大量防御）：
  1. **TCP 半开（half-open）**：门店断网恢复后，旧连接「看似在线实则已死」——TCP 层重传超时要 **~16 分钟**才暴露，期间设备一直假在线；
  2. **broker 不为离线设备保留推送**：断线窗口内的下行全部丢失（属性功能串讲 4.5 已见过这个坑，靠 HTTP 轮询兜底最长 300s）；
  3. **重复投递**：QoS1 重传 + 重连补拉会产生同一条消息的多次到达。
- **目标**：秒级实时下行；断链 120s 内自愈；丢推补拉；重复去重；弱网下退避重连、长断网 give-up 等外部唤醒。

### 1.2 业务边界
- **做**：连接生命周期（退避/give-up/外部唤醒）、双向消息（下行推送分发 + 上行 Request 请求-响应）、SE 签名认证、ZSTD 解压、去重、补拉未 ack、连接态信号。
- **不做**：MQTT 连接的鉴权语义（SE 产 `DeviceAuthCode`，签名细节在 keystore/SE 芯片，见 iWiki 4022241442）；HTTP 轮询兜底（消费方 paas_shadow 自己做）；上行识别/查单通道的接入（`MESSAGE_TYPE_UP_RECOGNITION=10002` 等已定义**未接线**，识别仍走 HTTP，见 6.2-M2）。

### 1.3 关键业务规则
- **Topic 约定**：订阅 `down/{sn}` 与 `resp/{sn}`（QoS1，:364-367）；上行发布固定 `req`（QoS0，:177/:579）；broker 地址 `link-{host}:443`（:142-144，host 来自激活域名）。
- **消息协议**（linkcomm.proto:14-31）：下行 type 1~12（OTA=1、指令=2、补拉响应=3、支付意图=4、扫码=5、**cloud 属性=6**、支付/会员会话 7~12）；上行 10001~10003。外层统一 `MqttDownMessage{header{encoding,type,message_id,created_time}, body}`；`ENCODING_ZSTD` 时 body 需 zstd 解压。
- **认证格式**：username = `IOTDEVICE-SHA256-SM2|{sn}|{ts}|{nonce}`（:58-62）；password = Base64(PaasSignPackage{version="1", type="DeviceAuthSign", sign=DeviceAuthCode bytes})（:64-74）——与 HTTP 的 `Authorization` 头同一签名体系（SE SM2）不同封装入口。

### 1.4 改动范围（模块组成）
- **[抽象核心]** `mqtt_client.h/.cpp`：`MqttClient` 抽象基类 + PIMPL `MqttClientPrivate`（RunLoop/DoConnect/RecvLoop/去重/Request）。
- **[设备子类]** `device_mqtt_client.h`：40 行——只注入两个设备钩子（SN、SE SignCommonAuth）。
- **[生命周期]** `mqtt_service.h/.cpp`：持有 `mqtt_thread_` + `mqtt_client_`，桥接信号到 `interface::PaasMqtt`。
- **[全局门面]** `paas_mqtt.h/.cpp`：无 MqttClient 引用的模块（NetworkMonitorService 等）从这里订阅。
- **[协议]** `linkcomm.proto`：MessageType/Encoding/五个 body 消息（含 hex dump 验证注释）。

---

## 2. 整体设计与模块划分  [10min · 先图后讲]

### 2.1 架构与分层
```
 BootStrapper（开机 :307 Start）
 NetworkMonitorService（链路恢复 :67 Reconnect）
 业务模块：paas_shadow(type6) / upgrade_schd(type1) / iot_service(type2)
        │ RegisterDownMessageHandler(type, handler)   ← 各注册自己的 type
 ┌──────┴─────────────────────────────────────────────┐
 │ MqttService（单例，持 mqtt_thread_ + mqtt_client_）  │
 │   信号桥：MqttClient sigs → interface::PaasMqtt      │
 │ ┌─────────────────────────────────────────────────┐ │
 │ │ MqttClient（抽象，PIMPL 隔离 async_mqtt）          │ │
 │ │   ├ MqttClientPrivate：RunLoop / DoConnect /     │ │
 │ │ │   RecvLoop / Dispatch / Dedup / Request        │ │
 │ │ └ protected virtuals：GetDeviceSn / SignSeCommon │ │
 │ │      ↑ DeviceMqttClient 覆写（SN + SE 签名）       │ │
 │ └─────────────────────────────────────────────────┘ │
 │   broker: link-{host}:443  TLS1.2  MQTT v3.1.1      │
 │   sub: down/{sn}(QoS1) + resp/{sn}(QoS1)；pub: req  │
 └─────────────────────────────────────────────────────┘
   依赖：async_mqtt（第三方，被 PIMPL 隔离）、boost::asio::ssl、
        SeKeyStore（SE SM2）、Network（GetDeviceDomainHost）、zstd
```
分层关键（三层组装 + PIMPL）：
- **PIMPL 把 async_mqtt 挡在公开头文件之外**（mqtt_client.h:22-24 注释）——业务模块 include 不引入第三方库；
- **设备相关位（SN、SE 签名）通过 protected virtuals 注入**——核心可在无 SE 芯片环境下复用/测试；
- **信号桥解依赖**：网络监控层要感知 MQTT 连接态但不应依赖具体 client → `interface::PaasMqtt` 全局单例转发（mqtt_service.cpp:53-61）。

### 2.2 模块划分与职责
| 模块 | 职责 | 切分理由 |
|---|---|---|
| `MqttClientPrivate` | 连接状态机 + 收发 + 去重 + Request 匹配 | 全部核心单点（约 600 行） |
| `MqttClient` | 公开接口 + PIMPL 委托 + **Start 前缓冲**（host/handler 两级 pending，:77-81/:717-733） | 接口与实现隔离 |
| `DeviceMqttClient` | SN 与 SE 签名两个钩子 | 设备差异最小化（40 行） |
| `MqttService` | 线程/客户端生命周期 + 信号桥 + 自己的一级 handler 缓冲 | 拥有权单点 |
| `interface::PaasMqtt` | 全局信号门面 | 反向依赖解除 |

### 2.3 核心数据流（cloud 属性推送为例）
```
运营平台改配置 → broker publish down/{sn} (type6, QoS1)
 → RecvLoop 收 PUBLISH → HandlePublish(:448)
    ① QoS1 立即 PUBACK（broker 停止重传）
    ② 解析 MqttDownMessage → (ZSTD 解压) → dedup 滑动窗口
    ③ DispatchDownMessage → paas_shadow 注册的 type6 handler
       （handler 内部再 co_spawn 到 property_thread_，值捕获防 UAF）
 → paas_shadow: OnCloudSidePropertyMsgReceived → 版本门 → 生效 + ack
（若设备当时离线：broker 不保留 → 重连成功后 SyncUnAck 补拉 →
  type3 响应 HandleSyncUnAckResponse → 内层逐条重走 ②③）
```

---

## 3. 逐模块实现讲解  [25min · 业务意图先行]

### 3.1 连接生命周期：RunLoop 状态机　mqtt_client.cpp:199-277
- **业务意图**：一个永不退出的协程循环，承载连接/断开/重试/暂停全部状态。
- **顺序**：时间门（:205-209，TLS+签名需要正确系统时间）→ paused 等待（:212-215，`mqtt_enabled=false` 时暂停不杀循环，Reconnect 可恢复）→ `DoConnect` → 成功：`ConnectedSig` + `SyncUnAck` 补拉 + `KeepAliveWatchdog` + `RecvLoop`（:224-246）→ 失败：退避重试。
- **要点**：`pending_disconnect_msg_` 由 RecvLoop 退出时填写、RunLoop 消费（:241-243）——**DisconnectedSig 带真实原因**（"disconnected"还是具体错误），排障时区分断链根因全靠它。

### 3.2 DoConnect：每次连接全新 SSL context　:279-408
- **业务意图**：连得上也签得过——TLS 10s 握手超时、SE 签名、CONNACK 校验、双 topic 订阅校验。
- **要点**：
  - **每次尝试重建 `ssl_ctx_`**（:284-287 注释）：失败握手会把 context 留在 "protocol is shutdown" 状态，复用则后续全部 `ssl_write_internal` 失败——**这是踩过的坑写成注释的**；
  - CA 三级回退：bundle 文件 → 目录 → 默认路径（:292-307）；
  - **suback 逐项校验**（:374-401）：down/resp 任一被拒（0x80）即整次连接判失败——订阅不全是假在线；
  - `set_pingresp_recv_timeout(90s)`（:315，断链检测第一层，见 4.3）。

### 3.3 消息处理链：HandlePublish → Dispatch　:448-533
- **业务意图**：任何情况下先 ack、再处理——**broker 停止重传比本地处理成败更重要**。
- **顺序**：QoS1 立即 PUBACK（:451-457，即使消息是重复的或解析失败）→ 解析 protobuf（失败只记日志）→ ZSTD 解压（:471-477）→ type3 特判走 `HandleSyncUnAckResponse`（内层消息逐条重走分发，:482-485）→ `DispatchDownMessage`（去重 → 拷贝 handler 列表 → 逐个 co_spawn，**try-catch 包裹**——detached 协程未捕获异常会 `std::terminate` 整个进程，:517-528）→ `resp/{sn}` 前缀额外匹配 pending Request（:490-497）。
- **要点**：同一 type 可注册多个 handler（vector），handler 抛异常不影响其他 handler。

### 3.4 Request 上行请求-响应　:151-196
- **机制**：随机 message_id → publish 到 `req`（QoS0）→ `promise/future` 挂起等 `resp/{sn}` 匹配 → 超时清理。见 6.2-M2（当前无调用方）。

### 3.5 Reconnect / Stop / Shutdown 三态　:108-140
- **Stop=暂停**（`mqtt_enabled` 云端开关）：`paused_=true` + 关闭在途 endpoint；RunLoop 存活，Reconnect 可恢复。
- **Reconnect=强制立即重连**（:117-123 注释是本模块最精彩的一段）：网络恢复时旧 TCP 几乎必死（half-open），但 `connected_` 还没翻转——RecvLoop 正阻塞在 `async_recv` 上等一个永远不会来的包。`CloseEpLocked()` 让 `async_recv` 立刻报错 → RecvLoop 退出 → `connected_=false` → RunLoop 重新连接。**用「打断阻塞等待」代替「等它自己发现」**。
- **Shutdown=永久停止**（析构）：唤醒所有等待让协程自然退出（:134-140）。
- `ep_` 的所有读写经 `EpLocked/SetEpLocked/CloseEpLocked` 加锁（:623-643）——mqtt 线程与 property/network 线程会并发调 Stop/Reconnect。

### 3.6 组装层：MqttService 与信号桥　mqtt_service.cpp
- **要点**：`Start()` 创建 `mqtt` 线程 + `DeviceMqttClient`，host 取自 `Network::GetDeviceDomainHost()`（raw，拼 `link-` 前缀）；三族信号桥接到 `interface::PaasMqtt`（:53-61）；自己的 handler 二级缓冲（:63-67）；**析构顺序：先 Stop/重置 client 再停线程**（:22-37 注释——防回调访问悬空 client）。

---

## 4. 核心代码实现细节与原理（重点）  [10min+ · 讲透原理]

### 4.1 挑选的核心代码
4 处：**① 连接生命周期状态机**（退避阶梯/阈值/give-up）；**② 断链检测双保险**（TCP half-open 的两种检测）；**③ 消息可靠性链**（QoS1+dedup+补拉）；**④ SE 签名认证链**。

### 4.2 专深①：连接生命周期状态机　:199-277
```cpp
constexpr int32_t kReconnectIntervalsMs[] = {500, 1000, 3000, 5000, 10000, 30000, 60000};
constexpr int32_t kConnectFailThreshold = 10;   // ~30s 后才发 ConnectFailedSig
constexpr int32_t kReconnectGiveUpCount = 120;  // ~1h 后放弃
```
**为什么这么写（原理）**：
- **退避阶梯而非固定间隔**：弱网下高频重试会加剧拥塞且浪费电；500ms 起步保证快恢复，60s 封顶保证不放弃。`retry_cnt` 成功即清零（:225）——**每一轮断线是独立周期**。
- **ConnectFailedSig 每「断开→重连成功」周期只发一次**（`connect_failed_fired_` + `exchange`，:253-256/:260-262）：订阅方（如 paas_shadow 调整轮询间隔）要的是「长连接长时间不可用」这个**状态**，不是每次失败的事件——发 120 次只会淹没订阅方。成功连接时 reset（:226）进入下一周期。
- **give-up 之后 `WaitForReconnect()`**（:257-267/:616-621）：~1h 后停止出站尝试（省电/省流量），1s tick 轮询 `reconnect_requested_`——**只等外部唤醒**（NetworkMonitorService 在链路态跳变时调 Reconnect）。这就是属性功能串讲 4.5 里说的「≥2h 断网且链路态无跳变时加速不生效」的另一半：链路态跳变是唯一唤醒源，跳变不发生就一直 give-up 等待。

### 4.3 专深②：断链检测双保险　:315/:596-614
```
第一层（库内建）：set_pingresp_recv_timeout(90s)（:315）
  CONNECT 后库按 60s 周期发 PINGREQ；90s 收不到 PINGRESP → 库主动关闭底层连接
第二层（应用层）：KeepAliveWatchdog（:596-614）
  30s 一查 last_recv_time_；120s 无任何包 → CloseEpLocked 强制关闭
```
**为什么两层（原理）**：
- **TCP half-open 的暴露要 ~16 分钟**（重传超时）——设备假在线期间推送全丢且无感知。第一层把检测缩到 ~90s；但注释（:592-595）点明**内建超时可被 TCP 重传队列阻塞**——半开连接上 PINGRESP 的「等不到」与「还在路上」难以区分，所以再加第二层应用级 watchdog（120s 无任何包就强断）。
- `last_recv_time_` 在 RecvLoop **每个包**都刷新（:423-425）——任何流量（含 puback/pingresp）都算活着，比只看 PINGRESP 更鲁棒。
- watchdog 随连接 spawn、`connected_` 翻转后自退（:599-602）——快速重连循环中短暂并存多个 watchdog 无害（各自查同一个时间戳）。

### 4.4 专深③：消息可靠性链　:448-558
```
QoS1 立即 PUBACK → dedup 滑动窗口(1000) → 多 handler 分发
重连成功 → SyncUnAck(limit=20) → type3 响应 → 内层逐条重走分发
```
**为什么这么写（原理）**：
- **先 ack 后处理**（:451-457）：broker 的重传是 QoS1 语义；**只要没 ack，broker 会一直重发**。本地处理失败（解析错/解压空）也先 ack——丢一条好过 broker 无限重传挤占通道。代价是「确实坏了的消息没有重试机会」，取舍上认为重传同一条坏消息没有意义。
- **dedup 窗口的必要性**：QoS1 的重传 + 补拉（SyncUnAck 返回的正是「未 ack」的消息）会同一条消息到达两次——`IsDuplicate` 按 `(type, message_id)` 滑动窗口（1000 条）去重（:550-558）。**补拉消息与实时推送用同一个 dedup**：重连瞬间 broker 重传的、和补拉响应里的，是同一条（同 message_id），窗口保证业务只处理一次。
- **type3 包装本身不去重/不广播**（:480-482 注释）：外层是信封，内层才是消息——展开后内层走完整链路（含内层去重）。
- **SyncUnAck 是对「broker 不保留离线推送」的补偿**：重连后主动问「我掉线期间/我没 ack 的有哪些」（limit 20，fire-and-forget，:560-588），响应走普通下行通道回来。**这是 MQTT broker 层面做不到、必须应用层补的语义**——与 paas_shadow 的 HTTP 轮询兜底（300s）形成两级恢复：秒级补拉 + 分钟级轮询。

### 4.5 专深④：SE 签名认证链　:58-74/:334-343
```
username = "IOTDEVICE-SHA256-SM2|{sn}|{ts}|{nonce}"           (:58-62)
     ↓ 作为待签数据
SE SignCommonAuth(username, anti_replay=true)  → DeviceAuthCode bytes   (device_mqtt_client.h:31-36)
     ↓
password = Base64(PaasSignPackage{version="1", type="DeviceAuthSign", sign=DeviceAuthCode})  (:64-74)
     ↓ CONNECT packet (keepalive 60s, clientid=sn)   (:341-343)
```
**为什么这么写（原理）**：
- 与 HTTP 签名（网络库串讲 4.4）**同源不同入口**：同一 SE 芯片、同一 SM2 体系；HTTP 走 `Authorization` 头（`GenerateDeviceAuth` 用 SignDataHash），MQTT 走 username/password（`SignCommonAuth` 产 `DeviceAuthCode`）——username 里的 `ts/nonce` 就是防重放材料（:338 `anti_replay=true`）。
- password 是 **protobuf 再 Base64** 的两层封装（`PaasSignPackage`）——与 HTTP 的 `GenerateDeviceAuth` 手法一致，服务端同一套解析。
- clientid 用 SN（:342）——broker 端按 SN 路由 `down/{sn}` topic。

### 4.6 性能与复杂度
- 单线程串行（mqtt_thread_）：RecvLoop + handler spawn 都在同一 executor；`ep_` 跨线程访问加锁。
- 去重 O(n)（n≤1000，低 qps 可接受）；Request 的 promise 匹配 O(1)。
- keepalive 60s 周期 PINGREQ；watchdog 30s 轮询；give-up 后零出站流量。

---

## 5. 边界、异常与测试  [5min · 别一笔带过]

### 5.1 异常与降级
- 时间未同步：整轮跳过 5s 重查（TLS 证书校验与签名时间戳都需要正确时间，:203-209）。
- host 未设置：DoConnect 直接失败（:280-283）——MQTT 依赖激活后 `SetDeviceDomainHost`。
- TLS 握手 10s 超时定时器只在**真超时**时关连接（`operation_aborted` 忽略，避免误杀刚成功的连接，:320-326）。
- handler 抛异常：catch 记日志，绝不 std::terminate（:522-526）。
- 解析/解压失败：先已 ack，只记日志丢弃。

### 5.2 边界条件
| 场景 | 行为 |
|---|---|
| 断网（TCP half-open） | 双层检测 90s/120s 内自断（4.3） |
| 网络恢复（链路跳变） | NetworkMonitorService 调 Reconnect → 打断阻塞 recv 立即重连 |
| give-up 后断网又恢复 | 只能靠链路态跳变唤醒；无跳变则退化为 HTTP 轮询兜底 |
| 同一消息到达两次 | dedup 窗口拦下（含补拉与实时重叠） |
| suback 部分被拒 | 整次连接判失败重连 |
| mqtt_enabled=false（云开关） | Stop=暂停不杀循环，开关打开即恢复 |
| ZSTD 解压空 | 已 ack，丢弃记日志 |
| 重启 | dedup 窗口清零（内存态），broker QoS1 重传可能被重复处理（低风险，见 6.2-M4） |

### 5.3 测试覆盖
- **未覆盖（open 风险）**：本模块**零单测**——RunLoop 状态机（退避/give-up）、双 watchdog、dedup、Request 匹配全部靠真机/联调。与 paas_shadow 同病（状态机无回归用例），是本模块最大短板。PIMPL + protected virtuals 的设计本意就是「核心可在无 SE 环境测试」，**但测试至今没写**——架构给了可测性，工程没兑现。
- 已有验证手段：`linkcomm.proto` 的 hex dump 注释（:79-83，cloud 属性字段布局是对真实抓包验证的）——协议正确性靠双端对齐文档 + 联调。

---

## 6. 踩坑与接手注意事项（含审阅发现）  [审阅 + 雷区]

### 6.1 开发中坑
- **SSL context 复用 = 全废**：失败握手留下的 "protocol is shutdown" 状态会让后续所有连接失败——**不要为了省对象把 ssl_ctx_ 提出去复用**（:284-287 注释是踩坑后的遗产）。
- **PUBACK 优先于处理**：改动处理顺序前想清楚——不 ack 则 broker 无限重传；这是用「放弃坏消息重试」换「通道不被重传挤占」。
- **proto 双端同源**：`OtaLinkMsg` 注释（linkcomm.proto:125-127）明示后台推送体与 batch-query 响应必须**同源同版本发布**——改一端字段要双端同步，否则 OTA 推送解析静默失败。

### 6.2 接手者雷区地图（⚠️ 本次审阅发现）
| # | 现象 | 代码位置 | 风险 / 建议 |
|---|---|---|---|
| **M1** | **构造参数 `server/port` 被静默忽略** | mqtt_client.cpp:681-686（`(void)server; (void)port;`，实际地址走 `SetDeviceDomainHost("link-"+host)`） | 接口谎言：传错 server 不报错，地址永远来自激活域名。建议删参数或接通 |
| **M2** | **上行 Request 通道无调用方** | 全仓库未见 `MqttClient::Request` 外部调用；proto 10002（RECOGNITION）/10003（QUERY_ORDER）定义未接线 | 通道就绪但未启用（识别仍走 HTTP）。**接线前先审 `Request` 的实现**：`promise/future.wait_for` 是同步阻塞（:186-192），在协程里阻塞 mqtt executor 直到超时——若在 mqtt 线程上下文调用会饿死收发；接入时建议改 `steady_timer + co_await` 或确保调用方在别的线程 |
| **M3** | 上行 `req` topic 用 QoS0，下行订阅 QoS1 | :177/:579 vs :364-367 | 不对称取舍：上行丢包靠调用方超时重试兜底。接手别「顺手统一」为 QoS1——上行 QoS1 会引入 PUBACK 往返与 packet id 管理 |
| **M4** | dedup 仅内存、线性扫描 | :550-558（O(n)，n≤1000） | 重启后窗口清零，broker QoS1 重传可能重复处理一次（多数 handler 幂等则无害）；窗口规模别随意放大（平方级扫描） |
| **M5** | **MQTT 生命周期上报全 TODO** | :229/:245/:254/:259（`report_actions.h` 的 5 个 MQTT action） | 与《业务上报》串讲 R9 同一问题：连接成功率/断链原因无观测，排障全靠日志。接线时这些埋点位置已留好 |
| **M6** | proto 同名异构陷阱两处 | `CloudSideProperty`（linkcomm.proto:84-90）与 `palm::entity::CloudSideProperty` **字段布局不同**（省略 time/ack，hex dump 验证注释 :79-83）；`SyncUnAckMessageResponse.Header` 用 `create_time`（**无 d**，:70 NOTE） | 两个「CloudSideProperty」同名不同构、`created_time/create_time` 一字之差——改字段时对照 proto 注释，别从 entity 侧类推 |
| **M7** | watchdog 并存窗口 | :238（每次连接 spawn） | 快速重连循环中短暂多 watchdog 并存，各自查同一时间戳，无害；但**别把 watchdog 改成有状态逻辑**，否则并存期会互相干扰 |

**给接手者的硬提醒**：
- 🔴 改 `HandlePublish` 的 ack/处理顺序前，重读 4.4——顺序错 = broker 无限重传或坏消息无限重试，两头都是事故。
- 🔴 新增下行消息类型：**type 号先在 linkcomm.proto 登记**再写 handler（type 是 wire 契约，散定义会导致两个模块抢同一个 type）。
- 🟡 注册 handler 必须在 `MqttService::Start()` 之前（两级缓冲兜得住，但语义上先注册后启动是约定）。
- 🟡 `DisconnectedSig` 的 msg 字段带真实断链原因（`pending_disconnect_msg_`），排障优先看它而不是猜。

---

## 7. 参考文档
- 模块路径：`palm_app_linux/src/service/mqtt/`（mqtt_client/device_mqtt_client/mqtt_service/paas_mqtt + linkcomm.proto）
- 上游调用方：`bootstrapper.cpp:307`（Start）、`network_monitor_service.cpp:67`（Reconnect）、`paas_shadow_impl.cpp:218`（type6）、`upgrade_schd.cpp:276`（type1）、`iot_service.cpp:116`（type2 指令）
- 依赖：async_mqtt（PIMPL 隔离）、`SeKeyStore::SignCommonAuth`（iWiki 4022241442 §3）、`Network::GetDeviceDomainHost`（见《网络库-模块业务串讲》）、zstd
- 相关串讲：《属性功能-模块业务串讲》4.5（MQTT/HTTP 双通道切换与重连加速的消费者侧）、《业务上报-模块业务串讲》R9（MQTT 上报 TODO）
- Story：135478800（Instruction proto 对齐后台）；OtaLinkMsg 双端同源约束（linkcomm.proto:125-127）
