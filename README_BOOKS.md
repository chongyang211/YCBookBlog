# 混合模式（Hybrid）技术方案

## 1. 背景与目标

混合模式 = **识别 + 注册** 二合一：用户刷掌后，设备自动判断是已注册还是未注册：

- **已注册** → 直接识别成功，回传 `user_id + otc`；
- **未注册** → 提示二次刷掌（注册阈值）→ 拉起扫码 → `registerPalm` 完成录入。

按触发方式分两套实现，事件命名空间和会话生命周期不同：

| 模式 | Coordinator | Route | 触发方式 | 结果后行为 |
| --- | --- | --- | --- | --- |
| 掌唤混合 | `HybridCoordinator` | `/hybrid` | 进入页面后**常驻刷掌** | 由前端结果页倒计时结束 → `restartHybrid` 启下一轮 |
| 机唤混合 | `HostHybridCoordinator` | `/host-hybrid` | 仅由上位机 `wakeup(6001)` 触发 | 不自动 restart，等待下一次 wakeup |

## 2. 整体流程

```
                ┌───────────────┐
                │   activate    │
                └───────┬───────┘
                        ▼
        ┌──────── CAPTURING_PALM (round-1) ────────┐
        │  recognize(RECOGNITION) → palm_state?    │
        └─────┬───────────────────────────┬────────┘
              │ Registered                │ Unregistered
              ▼                           ▼
    notifyRecognizeSuccess        RECAPTURING_PALM (round-2)
    (user_id + otc)               recognize(ENROLLMENT) → session_key
                                          │
                                          ▼
                                    SCANNING_QR (palmpact://...)
                                          │
                                          ▼
                                    ENROLLING (registerPalm)
                                          │
                                          ▼
                                  notifyEnrollSuccess
```

任意阶段失败 → `notifyFailure(code, msg)` → stage 复位为 `IDLE` + 广播失败事件 + 上位机回包（机唤）。

## 3. Native 状态机

两个 Coordinator 共享同一组 `Stage`：

| Stage | 含义 | 触发的 SDK 调用 |
| --- | --- | --- |
| `IDLE` | 空闲 | — |
| `CAPTURING_PALM` | 第一次采集（识别阈值） | `palmClientManager.recognize()` |
| `RECOGNIZING_PALM` | 第一次采集后调 `recognize(RECOGNITION)` | API |
| `RECAPTURING_PALM` | 第二次采集（注册阈值） | `palmClientManager.register()` |
| `RECOGNIZING_PALM_FOR_ENROLL` | 第二次采集后调 `recognize(ENROLLMENT)` | API |
| `SCANNING_QR` | 扫码 | `palmClientManager.scanQr()` |
| `ENROLLING` | 调 `registerPalm` | API |

**已注册边界**：第一次未注册→第二次又判已注册（极少数并发场景），fallback 走 `notifyRecognizeSuccess`。

**已注册判定**：`code == 0 && palm_state == "Unregistered"`，或 `mapRecognizeErr(code) == CODE_NOT_ENROLLED(100025)`。

## 4. JSBridge 协议

### 4.1 前端 → Native（call）

| 方法 | 时机 | 备注 |
| --- | --- | --- |
| `activateHybrid` / `activateHostHybrid` | 页面 mounted | 机唤返回 `linkManager.isConnected()` |
| `deactivateHybrid` / `deactivateHostHybrid` | 页面 unmounted | 切模式 / 路由 |
| `cancelHybrid` / `cancelHostHybrid` | 用户主动取消 | 走 `CODE_OPERATION_CANCELLED` |
| `timeoutHybrid` / `timeoutHostHybrid` | 前端各阶段倒计时归零 | Native 按当前 `stage` 选择 timeout code |
| `restartHybrid` | **仅掌唤**：结果页结束 | 触发下一轮 `startCapture` |

### 4.2 Native → 前端（broadcast）

事件命名空间：掌唤 `hybrid:*`，机唤 `hostHybrid:*`。

| 事件 | 触发 stage | payload |
| --- | --- | --- |
| `linkStateChanged` *(仅机唤)* | USB 连接变化 | `{ connected, reason? }` |
| `commandReceived` *(仅机唤)* | 收到 6001 | `{}` |
| `submitting` | 进入 `RECOGNIZING_PALM` / `RECOGNIZING_PALM_FOR_ENROLL` | `{}` |
| `recapture` | 进入 `RECAPTURING_PALM` | `{}` |
| `qrScanning` | 进入 `SCANNING_QR` | `{}` |
| `qrSubmitting` | 进入 `ENROLLING` | `{}` |
| `recognizeSuccess` | 已注册路径成功 | `RecognizePalmResp` |
| `enrollSuccess` | 未注册路径成功 | `RegisterPalmResp` |
| `failure` | 任意失败 | `{ code, message }` |

### 4.3 上位机协议（机唤特有）

| FuncId | 作用 | 关键 code |
| --- | --- | --- |
| `FUNC_WAKEUP=6001` | wakeup / 结果回包 | `CODE_ALREADY_AWAKE`, success/失败码 |
| `FUNC_TERMINATE` | host 主动取消 | `CODE_NOT_AWAKE`, `CODE_ABORT_UNAVAILABLE` |
| `FUNC_STAGE` | 阶段进展 | `STARTED`, `READY_TO_SCAN`, `HYBRID_SCANNED`, `HYBRID_ENROLLMENT` |

`isRequestInFlight()`（`RECOGNIZING_*` / `ENROLLING`）期间 `FUNC_TERMINATE` 拒绝中断，返回 `CODE_ABORT_UNAVAILABLE`。

## 5. 前端 Phase 状态机

### 5.1 掌唤 `HybridPage.vue`

```
waiting → submitting →
   ├── result_success_recognize
   └── recapturing → submitting → qrScanning → qrSubmitting → result_success_enroll
任意失败 → result_failure → reset → waiting (call restartHybrid)
```

倒计时：`RECAPTURE_TIMEOUT_SEC=10s`，`SCAN_QR_TIMEOUT_SEC=60s`，结果页 `RESULT_DISPLAY_SEC`。

### 5.2 机唤 `HostHybridPage.vue`

```
waiting_connection ⇄ waiting_command (linkStateChanged)
waiting_command —commandReceived→ scanning_palm → recognizing_palm →
   ├── result_success_recognize
   └── recapturing_palm → recognizing_palm_for_enroll →
         scanning_qr → qr_submitting → result_success_enroll
任意失败 → result_failure → reset → waiting_(connection|command)
```

倒计时：`SCAN_PALM_TIMEOUT_SEC=10s`，`SCAN_QR_TIMEOUT_SEC=30s`，结果页 `RESULT_DISPLAY_SEC`。

`useInterruptible`（仅 `waiting_*` 阶段允许被其他模式抢占）。

## 6. 超时与异常

| 场景 | 处理 |
| --- | --- |
| 第一次刷掌超时 | 前端倒计时 → `timeout*` → Native `CODE_TIMEOUT_CAPTURE` |
| 第二次刷掌超时 | 同上 → `CODE_TIMEOUT_RECAPTURE`（机唤特有码 205） |
| 扫码超时 | 同上 → `CODE_QR_SCAN_TIMEOUT` |
| 用户取消 | `cancel*` → `CODE_OPERATION_CANCELLED` → 前端 `isCancelCode` 直接 `reset` 不显示失败页 |
| Host 主动 terminate | Native 直接广播 failure + 回包，跳过 `notifyFailure` 的 stage guard |
| USB 断连（仅机唤） | 进行中（`stage != IDLE`）→ `notifyFailure(CODE_INTERNAL_ERROR)`；广播 `linkStateChanged` |
| API 失败 | `mapRecognizeErr` / `mapRegisterErr` / `toHostCode` 映射统一错误码 |

## 7. 关键实现要点

1. **stage 单点切换**：`notifyFailure` / `notifySuccess` 一律先把 `stage = IDLE` 再广播，避免重复触发。
2. **`stage == IDLE` guard**：`notifyFailure` 首行直接 return，防止已结束会话被二次失败覆盖。
3. **`pendingSessionKey`**：`recognize(ENROLLMENT)` 拿到后保存，`registerPalm` 时取出，结束时清空。
4. **掌唤的 `cancelSession`**：`deactivate` / `host terminate` 调用，统一 `palmClientManager.stop()` + 复位。
5. **机唤 `notifyFailure` 末尾 `palmClientManager.stop()`**：防止 SDK 残留回调，掌唤不需要（结果页倒计时后还要 restart）。
6. **二次采集走 `register()` 而非 `recognize()`**：这是 SDK 阈值控制的关键差异。
7. **前端 `isResult` / `isScanning` guard**：所有事件 handler 进入前判断当前 phase 合法性，避免迟到事件污染状态。
8. **结果后行为差异**：掌唤 `reset()` 内调用 `restartHybrid`；机唤不调用，等待 host 下次 wakeup。

## 8. 与单一模式的复用

| 复用点 | 说明 |
| --- | --- |
| `DeviceApiService.recognizePalm` | 通过 `RecognizeMode` (`RECOGNITION` / `ENROLLMENT`) 区分阈值 |
| `DeviceApiService.registerPalm` | 与纯录入模式共用 |
| `PalmClientManager.recognize/register/scanQr` | SDK 层 API 一致 |
| 前端组件 `PalmGuide` / `PalmScan` / `QrScan` / `ResultSuccess` / `ResultFailure` | 全部复用 |
| `HostProtocol` 错误码、Stage 枚举 | 全模式共用 |

## 9. 文件索引

- `Android/.../mode/HybridCoordinator.java` — 掌唤
- `Android/.../mode/HostHybridCoordinator.java` — 机唤
- `web/src/features/hybrid/HybridPage.vue`
- `web/src/features/host-hybrid/HostHybridPage.vue`
