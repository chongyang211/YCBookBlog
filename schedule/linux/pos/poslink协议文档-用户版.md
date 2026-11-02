# PosLink 刷掌模组通信协议 — 接入文档（用户版）

- **版本**：v2.3.0（协议版本 0x01 / 编码版本 0x01）
- **适用对象**：自行实现 PosLink 协议对接刷掌模组的上位机（POS/收银机）方案商与开发者
- **说明**：本文档由协议源码（`pos_link/pos_link_app.h`、`pos_link/pos_link_protocol.h`、`pos_app_commands.gen.h` ← `commands.yaml`）整理；使用官方 SDK（PalmManager Android SDK）接入的开发者无需关心帧格式，直接参考《PalmManager Android SDK 文档》即可

---

## 1. 概述

### 1.1 通信模型

- **传输**：上位机（POS）与刷掌模组之间的串口/USB 链路，二进制帧协议
- **角色**：上位机为协议客户端（发起业务请求），模组为服务端（处理请求 + 主动推送事件）
- **消息三类**：
  - **请求/响应**（上位机 → 模组）：一问一答，请求 5s 超时、重试 3 次
  - **推送**（模组 → 上位机）：模组主动通知（识别结果、状态变化、等待输入等），无需应答
  - **双向指令**（0xAD）：模组与上位机均可发起（远程开门、日志上传等）
- **会话模型**：一次刷掌业务由 `session_id`（模组生成）贯穿——启动会话 → 过程事件 → 结果上报 →（可选）上位机确认

### 1.2 术语

| 术语 | 说明 |
|------|------|
| 上位机 / Host | POS 收银设备，协议客户端 |
| 模组 / Module | 刷掌模组（掌纹识别 + 摄像头 + 网络），协议服务端 |
| session_id | 一次刷掌会话的唯一标识，模组生成（12 字符） |
| traceparent | W3C 链路追踪标识，随请求透传至云端 |

---

## 2. 帧格式

```
| 帧头标识 | 帧长 | 协议版本 | 编码版本 | 消息ID(10B) | 校验类型 | 保留 | 业务体 | CRC32 | 帧尾标识 |
|  1B 0x7E |  4B  |   1B 0x01 |   1B 0x01 |     10B     |  1B 0x01 |  4B  |  变长  |  4B   |  1B 0x7E |
```

### 2.1 帧头（22 字节，紧凑对齐）

| 字段 | 长度 | 值 | 说明 |
|------|------|-----|------|
| msg_tag | 1 | 0x7E | 帧起始标识 |
| msg_length | 4 | — | 业务体长度（不含帧头帧尾） |
| protocol_version | 1 | 0x01 | 协议版本 |
| encoding_version | 1 | 0x01 | 编码版本 |
| msg_id | 10 | — | 消息标识：msg_type(1) + msg_datetime(7, BCD 时间) + msg_no(2, 序号) |
| msg_checksum_type | 1 | 0x01 | 校验类型：0x01=CRC32 |
| reserved | 4 | 0xFF | 保留 |

### 2.2 业务体

首个字段固定为 `msg_cmd`（2 字节功能码），后接该功能的字段（定长/字符串混合）。

### 2.3 帧尾（5 字节）

| 字段 | 长度 | 说明 |
|------|------|------|
| msg_checksum | 4 | CRC32（多项式约定初值 0），校验范围=帧头+业务体 |
| msg_end_tag | 1 | 0x7E |

### 2.4 转义

业务体中出现 0x7E 时需转义处理（转义前缀 0x5E），接收侧还原后再校验。

### 2.5 消息类型与应答规则

| msg_type | 含义 | 应答规则 |
|----------|------|----------|
| 0x03 | 请求（Req） | 接收方回响应帧，**响应帧头携带 reply 三元组**：reply_msg_id（原请求 msg_id）+ reply_msg_cmd + reply_msg_code（应答状态码，0=成功） |
| 0xFB | 响应（Resp） | — |
| 0x01/0x02 | 0xAD 双向指令的控制请求/状态通知 | 见 0xAD |

- 请求超时 5s，重试上限 3 次
- 收到不认识的 cmd_id：回 reply_msg_code=0x2（提示对端升级）
- 帧级错误码：0x1 帧标识错 / 0x2 未知指令 / 0x3 编码版本错 / 0x4 校验类型错 / 0x5 CRC 错 / 0x6 数据错 / 0x7 重复消息

---

## 3. 功能码总表

### 3.1 会话控制（上位机 → 模组）

| 功能码 | 名称 | 方向 | 说明 |
|--------|------|------|------|
| 0x04 | StartPalm | → | 启动刷掌会话（biz_type 区分识别/注册意图） |
| 0x0E | StartPalmEx | → | 启动刷掌会话（per-call 覆盖离线偏好/扫码模式，替代 0x04 编码） |
| 0x05 | StopPalm | → | 停止当前刷掌会话 |
| 0x09 | BindPalm | → | 提交用户绑定信息（用户信息 + session_id） |
| 0x14 | HostResultAck | → | 上位机结果确认（HOST_ACK 模式下确认刷掌结果） |

### 3.2 查询与配置（上位机 → 模组）

| 功能码 | 名称 | 说明 |
|--------|------|------|
| 0x01 | QueryInfo | 查询模组信息 |
| 0x02 | QueryState | 查询刷掌状态（state + error_code） |
| 0x03 | Control | 控制指令 |
| 0x06 | HostStateChange | 上报上位机自身状态变化 |
| 0x07 | CameraLight | 摄像头灯模式 |
| 0x08 | QueryActivationStatus | 查询激活状态 |
| 0x0A / 0x0B | Query/SetPalmBehavior | 查询/设置刷掌行为配置（双掌、结果模式等） |
| 0x0C | Handshake | 协议握手（交换协议版本与指令清单，建链首报文） |
| 0x0F | QueryLicense | 查询授权状态（auth_state/appid/expire_time） |
| 0x10 | LicenseAuthorize | 触发授权 |
| 0x12 | EnterActivationFlow | 进入激活流程 |
| 0x13 | LicenseExitRefresh | 退出刷新 license |
| 0x15 | NetworkDiagnosis | 网络诊断 |
| 0x0D | VerifyPalm | 提交加验数据（session_id + base64 JSON） |

### 3.3 模组 → 上位机 推送

| 功能码 | 名称 | 说明 |
|--------|------|------|
| 0xA1 | ModuleStateChange | 模组设备状态变化 |
| 0xA3 | RecognizeResult | 刷掌/加验/录掌/绑定结果上报（中途成绩单亦走此通道） |
| 0xA4 | PalmStatus | 掌状态 |
| 0xA5 | ActivationStatusChange | 激活状态变化（含 error_code/msg） |
| 0xA6 | UpgradeStatusChange | 升级状态变化（type/status/progress/error） |
| 0xA7 | SpeedTestNotify | 网速测试结果（带宽/RTT/弱网等级） |
| 0xA8 | PalmEventNotify | 掌事件（0x01 优选图就绪 / 0x02 切注册 / 0x03 会话启动 / 0x04 会话结束） |
| 0xA0 | QueryHostInfoPush | 主机信息推送 |
| 0xA2 | PlayTtsPush | TTS 播报推送（status_code/tip_msg/tts_msg） |
| 0xAB | PalmVerifyNotify | 加验通知（need_verify / 100038 重试） |
| 0xAC | PalmBindInputNotify | 录掌等待通知（转绑定输入，携 timeout_seconds 渲染倒计时） |
| 0xB0 | LicenseStateNotify | 授权状态推送 |
| 0xB2 | LicenseAuthResult | 授权结果推送 |
| 0xB3 | AdviceUpgradeNotify | 升级建议确认（task_id/version/changelog/timeout，上位机回 user_choice） |
| 0xB5 | AutoActivationConfirm | 自动激活确认（device_sn/customer_domain/timeout） |

### 3.4 双向指令

| 功能码 | 名称 | 说明 |
|--------|------|------|
| 0xAD | BiComm | 通用双向控制：msg_type 0x01=控制请求 / 0x02=状态通知；phase 枚举（0x01 已接受/0x02 处理中/0x10 完成/0x11 失败/0x12 取消 + 业务扩展段如日志上传 0x20~0x22）；发起方生成 task_id |

---

## 4. 关键流程时序

### 4.1 建链与就绪

```
上位机                                模组
  |---- 0x0C Handshake ----------------->|   协议握手（版本/指令清单交换）
  |---- 0x01 QueryInfo ----------------->|   查询模组信息
  |<--- 0xA1 ModuleStateChange ----------|   模组状态推送（激活/授权就绪后 → 可刷掌）
```

### 4.2 识别会话

```
  |---- 0x04/0x0E StartPalm ------------>|   启动（响应带 session_id + palm_mode）
  |<--- 0xA8 PalmEventNotify(0x03) ------|   会话启动
  |    （用户刷掌……）
  |<--- 0xA3 RecognizeResult ------------|   识别结果（成功带 result_data；未注册 306 触发切注册）
  |---- 0x14 HostResultAck ------------->|   （HOST_ACK 模式）上位机确认结果
  |<--- 0xA8 PalmEventNotify(0x04) ------|   会话结束
```

### 4.3 未注册 → 扫身份码注册（hybrid/CPM）

```
  |<--- 0xA3 RecognizeResult(306) -------|   识别未注册（中途成绩单，切注册）
  |<--- 0xA8 PalmEventNotify(0x02) ------|   切注册通知
  |    （用户再次刷掌，注册探测）
  |<--- 0xAB PalmVerifyNotify? ----------|   （若命中加验：need_verify/100038 重试）
  |<--- 0xAC PalmBindInputNotify --------|   探测=Unregistered → 转等身份码（timeout 倒计时）
  |    （用户扫身份码）
  |---- 0x09 BindPalm ------------------>|   提交用户信息（register-palm）
  |<--- 0xA3 RecognizeResult ------------|   录掌绑定收敛结果
  |<--- 0xA8 PalmEventNotify(0x04) ------|   会话结束
```

### 4.4 HOST 绑定（WaitBindInput 分支）

同 4.3，但 0xAC 后等待 **POS 下发 0x09 BindPalm**（携带 user_info）；40s 未收到 → 结果报 124 等待超时。可重试失败会再次 0xAC（带当轮 error_code，倒计时不重置）。

### 4.5 加验

```
  |---- 0x0D VerifyPalm ---------------->|   提交加验数据（session_id + base64 JSON）
  |<--- 0x0D Resp(ret_code) ------------>|   0=受理 / 125 无会话 / 126 状态错 / 127 解析失败
  |<--- 0xA3 RecognizeResult ------------|   加验结果（100038=信息有误需重输，最多 3 轮）
```

---

## 5. 核心报文字段

> 完整字段以 `pos_app_commands.gen.h` / `pos_link_app.h` 为准，此处列核心业务报文。

### 5.1 StartPalm (0x04) Req/Resp

| 字段 | 类型 | 说明 |
|------|------|------|
| biz_type | int16 | 业务类型（识别/注册意图枚举） |
| → ret_code | int16 | 0=成功 |
| → session_id | string | 会话标识 |
| → palm_mode | int8 | 实际工作模式（识别/注册_HOST/注册_CPM/混合） |

### 5.2 StartPalmEx (0x0E) Req

| 字段 | 类型 | 说明 |
|------|------|------|
| offline_preferred | uint8 | 0=跟随配置 1=强制在线 2=强制离线 |
| scan_mode | uint8 | 0=跟随配置 1=关 2=仅扫码 3=扫码+刷掌 |

### 5.3 BindPalm (0x09) Req

| 字段 | 类型 | 说明 |
|------|------|------|
| session_id | string | 会话标识（须与当前等待绑定会话一致） |
| user_info | string | 用户信息（JSON，含 user_id 等） |

### 5.4 RecognizeResult (0xA3) Push

| 字段 | 类型 | 说明 |
|------|------|------|
| ret_code | int32 | 结果码（0 成功；306 未注册；114 质量不达标；117 加验信息有误；124/161/141 超时类等） |
| session_id | string | 会话标识 |
| result_data | string | JSON 明细：recognize_type/palm_state/user_id/user_name/otc/session_key/traceparent 等 |

### 5.5 PalmEventNotify (0xA8) Push

| 字段 | 类型 | 说明 |
|------|------|------|
| event_type | int8 | 0x01 优选图就绪 / 0x02 切注册 / 0x03 会话启动 / 0x04 会话结束 |
| session_id | string | 会话标识 |

### 5.6 PalmBindInputNotify (0xAC) Push

| 字段 | 类型 | 说明 |
|------|------|------|
| ret_code | uint32 | 0=需要录掌输入（保留扩展） |
| notify_params | string | base64(JSON)：session_id + timeout_seconds |

### 5.7 HostResultAck (0x14) Req/Resp

| 字段 | 类型 | 说明 |
|------|------|------|
| session_id | string | 会话标识 |
| error_code | uint32 | 0=确认成功收下结果；非 0=上位机拒绝 |
| reason | string | 拒绝原因 |
| → ret_code | uint32 | ACK 应答码 |

### 5.8 VerifyPalm (0x0D) Req

| 字段 | 类型 | 说明 |
|------|------|------|
| session_id | string | 会话标识 |
| verify_data | string | base64(JSON)：手机尾号或 error_code（拒绝加验时） |

---

## 6. 常用枚举

| 枚举 | 定义处 | 关键值 |
|------|--------|--------|
| BizType（0x04） | SDK `BizType.java` | 识别/注册意图 |
| PalmMode | SDK `PalmMode.java` | 1=识别 2=注册_HOST 3=注册_CPM 4=混合 |
| ScanMode（0x0E） | SDK `ScanMode.java` | 0=跟随配置 1=关 2=仅扫码 3=扫码+刷掌 |
| OfflinePreferred（0x0E） | SDK `OfflinePreferred.java` | 0=跟随配置 1=关 2=开 |
| ActivationStatus（0x08/0xA5） | SDK `ActivationStatus.java` | 0x01 未激活 0x02 激活中 0x03 已激活 0x04 异常 |
| AuthState（0x0F/0xB0/0xB2） | 协议注释 | 0 未知 1 检查中 2 未授权 3 授权中 4 已授权 5 过期 6 错误 |
| PalmEventType（0xA8） | 模组 `pos_request_handler.h` | 0x01 优选图就绪 0x02 切注册 0x03 会话启动 0x04 会话结束 |
| ErrorCode | SDK `ErrorCode.java` | 应用层统一错误码（如 118=掌已录入、124=绑定等待超时、306=用户未注册、42=激活异常） |

---

## 7. 注意事项

1. **结果以 0xA3 为准**：识别/录掌/绑定/加验的最终结果均经 0xA3 上报，`session_id` 关联会话；0xA8 仅是过程事件
2. **0xAC 是行动指令**：收到后须在 `timeout_seconds` 内提交 0x09 BindPalm（HOST）或引导扫码（CPM）；超时模组以 124 收敛
3. **可重试失败**：绑定提交失败若可重试，模组再次 0xAC（带当轮 error_code、倒计时不重置）——上位机应提示用户修正后重试，不要停止会话
4. **未知指令兜底**：升级模组后旧上位机可能收到新推送，按 reply 机制忽略即可；旧模组收到新指令会回 0x2，提示升级
5. **握手指令清单**：0x0C 握手可交换双方支持的指令清单，用于能力协商与兼容性判断

---

*文档生成：2026-11-02，依据源码整理（协议版本 0x01）；正式交付时建议归入 `palm_manager/docs/external/` 并随协议变更同步维护。字段完整定义以 `pos_app_commands.gen.h`（由 `commands.yaml` 生成）与 `pos_link_app.h` 为准。*
