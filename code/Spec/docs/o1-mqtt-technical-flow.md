# O1 MQTT 技术流程文档

> 关联 Spec: `specs/v1.7.0/135221689-o1-shadow-mqtt.md`  
> O4 参考: `specs/v1.7.0/134755763-mqtt-integration.md`

## 1. 架构总览

```
┌─────────────────────────────────────────────────────┐
│                    MQTT Broker                       │
│              link-{device_domain_host}:443            │
│          TLS 1.2 / MQTT 3.1.1 / QoS 1               │
└───────┬──────────────┬──────────────┬────────────────┘
        │ down/{sn}    │ resp/{sn}    │ req
        ▼              ▼              ▲
┌──────────────────────────────────────────────────────┐
│              PaasMqtt (C++ Native)                    │
│  async_mqtt v9.0.2  |  SE 签名认证                    │
│  渐进重连 500ms→60s |  心跳 60s                        │
│  4 信号: PushMsg / Connected / Disconnected / Failed  │
└───┬──────────────────────────────────────────────────┘
    │ PushMsgSig (msgType=6)          │ ConnectedSig/DisconnectedSig
    ▼                                ▼
┌───────────────┐   ┌──────────────────────────────────┐
│ PaasShadowImpl │   │  LongLinkManager (Java)           │
│ (C++ 原生)     │   │  OnMessageListener(msgType=1,2)   │
│ msgType=6     │   │  OnStatusListener                  │
│ 属性推送处理    │   │                                  │
└───────┬───────┘   └────┬──────────┬───────────────────┘
        │                │ msgType=1 │ msgType=2
        ▼                ▼          ▼
┌──────────────┐  ┌──────────┐ ┌──────────┐
│ ShadowManager │  │ OTA 模块  │ │ CMD 模块  │
│ 属性变更分发   │  │ checkUpd  │ │ 指令处理   │
└──────┬───────┘  └──────────┘ └──────────┘
       │
       ▼
  ShadowModuleApi.Listener
  → 刷掌识别 / 其他业务模块
```

**6 个模块，1 个连接**：Shadow（C++）、OTA（Java）、CMD（Java）三个消费者共用同一个 `PaasMqtt` 实例，通过 `msgType` 路由分发。

## 2. 初始化时序

```
IoTServiceApp 启动
│
├─① LongLinkManager.init(baseDomain)          ← 必须最先（创建 PaasMqtt 实例）
│     ├─ System.loadLibrary("paas_mqtt")
│     ├─ PaasMqttLibrary.initialize(context)
│     ├─ PaasMqttConfig(link-{domain}, 443)
│     ├─ PaasMqttDevice.getDeviceSn()           → DeviceUtils.getSerial()
│     ├─ PaasMqttDevice.signSeCommonAuth()       → SE 签名
│     ├─ PaasMqtt.createInstance(config, device, listener)
│     └─ mMqtt.start()                           → TLS 握手 → CONNECT
│
├─② ShadowManager.start()
│     ├─ PaasManager.init()
│     │     ├─ PaasPlatform.init()
│     │     ├─ PaasReport.init()
│     │     └─ PaasNetwork.init()                → HTTP 长连接认证
│     │
│     └─ PaasShadow.init()                       → JNI → PaasShadowImpl()
│           ├─ 启动 boost::asio io_context 线程 (thr_)
│           ├─ PaasMqtt::Global()->PushMsgSig.connect(msgType=6)  // 属性推送
│           ├─ PaasMqtt::Global()->ConnectedSig.connect(→300s)    // 动态轮询
│           ├─ PaasMqtt::Global()->DisconnectedSig.connect(→60s)  // 动态轮询
│           ├─ co_spawn(ctx_, Schedule())                         // HTTP 轮询
│           └─ PaasShadowClient 创建                               // HTTP 客户端
│
├─③ OTA 模块 init
│     └─ LongLinkManager.addOnMessageListener(msgType=1)
│
└─④ CMD 模块 init
      └─ LongLinkManager.addOnMessageListener(msgType=2)
```

> ⚠️ 若 `LongLinkManager.init()` 未先完成，`PaasMqtt::Global()` 返回 nullptr。Shadow 仅打印 `LOG_E` 日志，不会崩溃——降级为纯 HTTP 轮询模式。

## 3. 消息数据流

### 3.1 MQTT → 属性变更（主链路）

```
[MQTT Broker]  down/{device_sn} publish
       │  MqttDownMessage { header { msgType=6 }, body }
       ▼
[PaasMqtt 内部]  解析 MqttDownMessage, 解码 encoding (ZSTD/PLAIN)
       │  广播 PushMsgSig(std::shared_ptr<PushMsg>)
       ▼
[PaasShadowImpl]  PushMsgSig 回调 (MQTT 线程)
       │  if msg->message_type != 6 → 跳过
       │  asio::co_spawn(ctx_, OnCloudSidePropertyMsgReceived(...), detached)
       │  ↑ 立即返回，不阻塞 MQTT 线程
       ▼
[OnCloudSidePropertyMsgReceived]  (io_context 协程)
       │  1. CloudSidePropertyNotifyMessage.ParseFromString(msg_data)
       │     ├─ 解析失败 → SendCloudSidePropertyAck(error=ParseFail) → return
       │  2. shadow_cache_.GetCloudSidePropertyFromCache(identifier, scope)
       │     ├─ 本地版本 > 推送版本 → SendCloudSidePropertyAck(error=VersionLow) → return
       │  3. shadow_cache_.SaveCloudSidePropertyToCache(property)
       │  4. NotifyCloudSidePropertyChanged(identifier, scope, value, version)
       │  5. SendCloudSidePropertyAck(success)
       ▼
[NotifyCloudSidePropertyChanged]
       │  signals_[identifier] → signal_ptr->(identifier, scope, value, version, property)
       │  (Boost.Signals2, C++ 层)
       ▼
[PaasShadowListener]  (JNI 桥接 C++ → Java)
       │  onCloudSidePropertyChanged(identifier, scopeType, scopeValue, version, propertyValue)
       ▼
[ShadowManager]  遍历所有注册的 ShadowModuleApi.Listener
       │  listener.onCloudSidePropertyChanged(identifier, scope, scopeValue, version, property)
       ▼
[业务模块]  刷掌识别 / 设备行为 / 其他业务
```

### 3.2 轮询兜底（副链路）

MQTT 连接时：每 **300s** 拉取；断连时：每 **60s** 拉取。

```
Schedule() 协程循环 (每 30s 检查一次)
  │
  ├─[DeviceSide 拉取]  if now - lastPullDevice >= pull_interval_sec_.load()
  │     └─ PullDeviceSideProperties() → 成功则 lastPullDevice = now
  │
  ├─[CloudSide 拉取]   if now - lastPullCloud  >= pull_interval_sec_.load()
  │     └─ PullCloudSideProperties()  → 成功则 lastPullCloud  = now
  │
  └─[DeviceSide 上报]  if sendInterval 条件满足
        └─ SendDeviceSideProperties()
              ├─ 成功 → send_fail_count_ = 0
              └─ 失败 → send_fail_count_++
                    ├─ < 5 次 → 300s 后重试
                    └─ ≥ 5 次 → 600s 退避
```

### 3.3 动态轮询间隔切换

```
PaasMqtt ConnectedSig 触发
  → pull_interval_sec_.store(300)
  → LOG_I("mqtt connected, pull interval adjusted to 300s")

PaasMqtt DisconnectedSig 触发
  → pull_interval_sec_.store(60)
  → LOG_I("mqtt disconnected, pull interval restored to 60s")
```

## 4. 鉴权与连接

### 4.1 设备认证

```
ClientID  = Device::GetSN()
Username  = "IOTDEVICE-SHA256-SM2|{sn}|{timestamp}|{nonce}"
Password  = Base64(PaasSignPackage {
              version = "1.0",
              sign = SeKeyStore::SignDataHash(Username),
              type = "DeviceAuthSign"
            })
```

### 4.2 连接状态机

```
kInitial → kConnecting → kConnected ⇄ kDisconnected → kReconnecting → kConnected...
                ↓              ↓                          ↓
           ConnectFailed    [模块回调]             渐进式重连 (内部)
           (10 次后触发)
```

### 4.3 重连策略

| 重连次数 | 间隔 | 累计时间 |
|---------|------|---------|
| 1 | 500ms | 0.5s |
| 2 | 1s | 1.5s |
| 3 | 3s | 4.5s |
| 4 | 5s | 9.5s |
| 5 | 10s | 19.5s |
| 6 | 30s | 49.5s |
| 7+ | 60s (稳定) | — |

## 5. 多模块路由

| msgType | 模块 | 接入方式 | 消息体格式 |
|---------|------|---------|-----------|
| 1 | OTA | `LongLinkManager.addOnMessageListener` | `BatchQueryUpgradeDetailResp` |
| 2 | CMD | `LongLinkManager.addOnMessageListener` | `InstructionLinkMsg` |
| 6 | Shadow | `PaasMqtt::PushMsgSig` (C++) | `CloudSidePropertyNotifyMessage` |

**路由规则**：
- Java 桥接：`LongLinkManager.onPushMessage(msgType, msgId, body)` 遍历 `listenerList`，每个 listener 自行过滤 msgType
- C++ 原生：`PushMsgSig` 广播后，`PaasShadowImpl` lambda 过滤 `msgType == 6`

## 6. 错误处理

| 场景 | 级别 | 行为 | 恢复 |
|------|------|------|------|
| TLS 握手失败 | LOG_E | ConnectFailedSig 触发 | 自动重连 |
| CONNACK 拒绝 | LOG_E | ConnectFailedSig 触发 | 自动重连 |
| SE 签名失败 | LOG_E | 返回空 sign → 连接失败 | 自动重连 |
| `PaasMqtt::Global()` 为 nullptr | LOG_E | 跳过订阅，纯 HTTP 模式 | 等 LongLinkManager 初始化 |
| 消息体 PB 解析失败 | LOG_E | SendCloudSidePropertyAck(ParseFail) | 跳过该消息 |
| 推送版本 ≤ 本地版本 | LOG_I | SendCloudSidePropertyAck(VersionLow) | 不更新缓存 |
| MQTT 断连 | LOG_W | 自动重连 + HTTP 轮询 60s 兜底 | 重连成功恢复 Push |
| 单模块异常 | LOG_W | try-catch 包裹 → 不影响分发链 | 下次消息正常处理 |

## 7. 关键常量

| 常量 | 值 | 说明 |
|------|-----|------|
| `kPushMessageTypeOfCloudSideProperty` | 6 | Shadow 属性推送 msgType |
| `kPullIntervalSec` (断连) | 60s | HTTP 轮询兜底间隔 |
| `kPullIntervalSec` (已连接) | 300s | MQTT 连接后的拉取间隔 |
| `kDelayIntervalSec` | 30s | Schedule 循环检查间隔 |
| `kSendIntervalSec` | 300s | 设备侧属性批量上报间隔 |
| `kBackoffSendIntervalSec` | 600s | 上报失败退避间隔 |
| `kSendFailLimit` | 5 | 失败次数阈值 |
| MQTT KeepAlive | 60s | PINGREQ 心跳间隔 |
| `kInitDelaySec` | 60s | 首次轮询延迟 |
