# facility/network 网络模块 · 业务串讲

> **一句话定位**：刷掌 Linux 模组的**统一网络出口**——把「域名/会话管理、国密签名、请求收发、错误分类、trace 串联、失败上报」收在一处，上层业务（激活 / 识别 / 注册 / 升级 / 心跳）只面对 `IoTResponse {error_code, server_code, error_message, response_body, trace_id}` 这一个结构。
> **场次**：60 分钟（代码量中等、难点集中在签名与错误分类）　**听众**：同组开发 / 评审人 / 后续接手者
> **对照模版**：`06.技术模版/04.业务串讲的模版.md` 第 08 章　**代码基线**：`palm_app_linux/src/facility/network/` 当前实现（2026-08-31 审阅）
> **模块文件**：`network.h/.cpp`（主干）、`http_request.h/.cpp`（传输层）、`network_info.h/.cpp`（网络状态）、`network_stat.h/.cpp`（上报）、`server_errcode.h`（服务端码）、`palm_sign_package.proto`（签名包）+ 两个 gtest

---

## 1. 需求与业务背景  [10min · 先现象后价值]

### 1.1 需求来源与问题
- **来源**：刷掌设备端所有 IoT 后台通信（激活 / 识别 / 注册绑定 / 升级 / 心跳 / 上报）都需要「签名 + HTTPS + 错误处理 + 可观测」，早期散落在各业务链路里，重复且不一致。
- **问题**：① 各链路各写一套 cpr 调用，超时/重试/CA/DNS 策略不统一；② 签名算法混用（国密 SM2/SM3、会话密钥、SE 安全模块），实现分散；③ 错误码语义混乱——传输失败、HTTP 状态失败、服务端业务失败三种语义混为一谈，上层无法判断「值不值得重试」；④ 失败无 trace 凭据，客服无法定位；⑤ 弱网/无网场景下失败上报刷量。
- **目标**：收敛为单一网络出口；统一四类签名；把失败语义三分（传输 / HTTP / 业务）并给出可重试判定；请求带 W3C traceparent 且 trace 可随响应带回；失败上报做本地网络问题过滤。

### 1.2 业务边界
- **做**：IoT 短链 HTTPS 请求（GET/POST/PUT）、四类签名、错误分类与 error_msg 组装、trace 生成与回带、请求级上报、网络状态/离线模式、内嵌 CA 与连接复用。
- **不做**：长链 iLink（`DualLinkTdiRequest` 是 stub，network.cpp:702-707 直接返回 `{-1,{}}`）、请求体重压缩（接口有 `use_compression` 参数但实现忽略）、TDI 上报（network_stat.cpp:17 注释「按需求暂不启用」）、重试策略本身（只提供 `IsNetworkError` 判定，重试由上层决定）。

### 1.3 关键业务规则
- **三种失败语义**（决定重试）：
  - **传输失败**（cpr error）：超时 20101 / 网络 20102 / **TLS 证书 20105** / 其他 20104；
  - **HTTP 状态失败**：4xx → 20104（保守按网络类），5xx → 20407；
  - **服务端业务失败**：HTTP 200 但 body `{code != 0, message}` → 20407 + `server_code = code`。
- **可重试判定**：`IoTResponse::IsNetworkError()`（network.cpp:81-85）只收录 20101 / 20102 / 20104，**不含 20105**（证书问题重试无意义）。
- **签名四态**：`SIMPLE`（固定密钥 HMAC-SM3）/ `ACTIVATION`（会话密钥 HMAC-SM3）/ `DEVICE`（SE-SM2，`DeviceAuthSign`）/ `RECOGNIZE`（SE `SignPayAuth`，`DeviceRecogSign`）。
- **trace 规则**：W3C `traceparent` = `00-{32hex trace}-{16hex span}-01`；调用方可 override 串联会话，否则网络层每次现起新 trace。


【TODO：签名是什么，为什么要签名？】



### 1.4 改动范围
- **[主干]** `network.h/.cpp`：`Network` 单例、`IotDeviceRequestImpl` 请求主干、四类签名、`ClassifyCprError` / `ParseIoTHttpResponse` / `BuildErrorNotification`。
- **[传输层]** `http_request.h/.cpp`：cpr 封装、curl share 复用、内嵌加密压缩 CA、DNS 缓存 1h。
- **[辅助]** `network_info.*`（状态/离线）、`network_stat.*`（上报过滤）、`server_errcode.h`（服务端码）、`palm_sign_package.proto`（签名包）。
- **[近期演进]** TLS 证书错误从 20104 拆出独立 20105（story 137332527）；`BuildErrorNotification` 由模板函数改为 `ServerCodeMapper` 函数指针；trace 尾缀拼接点移至 `ParseIoTHttpResponse`。

【TODO： cpr 封装 是什么意思，curl share 复用 这个又是什么意思？ 】

---

## 2. 整体设计与模块划分  [10min · 先图后讲]

### 2.1 架构与分层
```
   业务层  激活 / 识别 / 注册绑定 / 升级 / 心跳 / 上报
                │  只认 IoTResponse{error_code, server_code, error_message, body, trace_id}
 ┌──────────────┴───────────────────────────────────────────┐
 │ facility/network                                          │
 │  ┌────────────────────────────────────────────────────┐  │
 │  │ Network（单例）                                     │  │
 │  │  域名/会话管理 → 签名 → IotDeviceRequestImpl        │  │
 │  │        │                    │            │          │  │
 │  │  Generate*Auth      GenerateTraceparent  SCOPE_EXIT │  │
 │  │  (SM3-HMAC/SE-SM2)  (trace_mutex_)       →NetworkStat│ │
 │  └────────┬───────────────────────────────────────────┘  │
 │           │ HttpRequest                                   │
 │  ┌────────┴───────────────────────────────────────────┐  │
 │  │ HttpRequest: cpr::Session + curl share + 内嵌CA     │  │
 │  └────────┬───────────────────────────────────────────┘  │
 │           │ 响应                                          │
 │  ParseIoTHttpResponse → ClassifyCprError → BuildErrorNotif│
 │  NetworkInfo（离线/连接态）   NetworkStat（上报过滤）      │
 └──────────────────────────────────────────────────────────┘
```
分层关键：**业务层不碰 cpr、不碰签名、不碰错误码映射**，只面对一个 `IoTResponse`。网络层负责「把一次 HTTP 通信翻译成一个结构化的失败语义」。

### 2.2 模块划分与职责
| 模块 | 职责 | 切分理由 |
|---|---|---|
| `Network` | 单例入口：域名/会话、四类签名、请求主干、trace | 唯一网络出口，收敛策略 |
| `HttpRequest` | cpr 薄封装：同步/异步、share 复用、内嵌 CA、DNS 缓存 | 传输细节与业务语义解耦，可单测/替换 |
| `ClassifyCprError` | cpr 错误 + HTTP 状态 → 统一错误码 | 与 Download 路径共用（注释 network.h:38-39） |
| `ParseIoTHttpResponse` | 响应体解析：业务码提取、trace 尾缀、文案组装 | 解析逻辑单点，各 HTTP 方法共用 |
| `BuildErrorNotification` | 对外通知组包：映射 + 兜底文案 + 超长截断 | 纯函数，各链路注入自己的 mapper |
| `NetworkInfo` | 网络类型/连接态/离线模式（signals2 通知） | 状态与请求解耦，`NetworkMonitorService` 为友元更新 |
| `NetworkStat` | 请求上报 + 本地网络问题过滤 + 最近可用时间 | 可观测，避免弱网刷量 |

### 2.3 核心数据流（一次激活 POST 为例）
```
ActivationService（持有 activation_trace_id_）
 → ActivationPostRequest(type, path, pb_body, trace_id)        network.cpp:689
 → IotDeviceRequestImpl("POST", ..., override_trace_id=trace_id) network.cpp:459
      ① CHECK_IOT_DEVICE_HOST_SET      480          → 未设置直接 ERR_PALM_IOT_HOST_NOT_SET
      ② switch(sign_type) 生成签名      491-504      → 失败 ERR_PALM_IOT_SIGN_FAILED
      ③ trace: override ? 复用 : GenerateTraceparent(true)  514-530
      ④ 组 URL + Header(Authorization/Content-Type/traceparent) 532-543
      ⑤ PostSync 发送                   566-581
      ⑥ ParseIoTHttpResponse(status, error, body, traceparent) 594-595
            error_obj  → ClassifyCprError → 20101/20102/20105/20104 + message+尾缀
            body.code≠0→ 20407 + server_code=code + message+尾缀
            200        → SUCCESS
            4xx / 5xx / 其他 → 20104 / 20407 / UNKNOWN + 尾缀
      ⑦ SCOPE_EXIT → NetworkStat::ReportDeviceRequest  472-475
 → IoTResponse{error_code, server_code, error_message, response_body, trace_id}
```

---

## 3. 逐模块实现讲解  [25min · 业务意图先行]

### 3.1 `Network` 单例与域名/会话管理　network.h:32-130
- **业务意图**：设备域名由云端下发、激活会话（session_id/session_key）随激活流程变化，需要一处统一存放与清理。
- **代码落点**：`SetIotDeviceHost`（113-121）、`GetIotDeviceHost`/`GetDeviceDomainHost`（123-132）、`SetActivationSession`/`ClearActivationSession`（134-147）。
- **要点**：`SetIotDeviceHost` **存 raw host**（剥离 `https://`/`http://`），因此既能拼 `https://{host}` 给 HTTP，也能拼 `link-{host}` 给 MQTT/NTP（注释 114-115）。这是一个容易忽略的设计——**存 raw 是为了两种端点复用**。

### 3.2 `IotDeviceRequestImpl` 请求主干　network.cpp:459-611
- **业务意图**：GET/POST/PUT 共用一条主干，保证签名、trace、header、上报、错误处理完全一致。
- **代码落点**：`SCOPE_EXIT` 上报（472-475）、host 校验宏（480，定义 94-102）、签名 switch（491-504）、trace（514-530）、header 组包（532-543）、发送（566-581）、解析（594-595）。
- **要点**：用 `SCOPE_EXIT` 包住上报，**任何中途 return（host 未设/签名失败/方法不支持）都会上报**，不会静默丢失。这是可观测性的关键手法。

### 3.3 `HttpRequest` 传输层　http_request.cpp
- **业务意图**：设备端弱网 + 频繁请求，TLS 握手与 DNS 解析成本高，且私有化部署需要自带 CA。
- **代码落点**：`StaticInitializer` 一次性初始化（49-63）、`LoadEmbedCaCert`（22-46）、`SendHttpRequestSyncImpl`（105-111）。
- **要点**：见 4.5 深挖（连接复用 / 内嵌 CA / DNS 缓存）。

### 3.4 响应解析与错误分类　network.cpp:164-273
- **业务意图**：把「传输 / HTTP / 业务」三种失败翻译成统一错误码，让上层能判断重试。
- **代码落点**：`IsCprNetworkError`（164-170）、`ClassifyCprError`（172-207）、`ParseIoTHttpResponse`（209-273）、`BuildErrorNotification`（45-63）、`IsNetworkError`（81-85）。
- **要点**：见 4.2 深挖（三分法 + 20105 拆分 + 砍头保尾 + trace 尾缀）。

### 3.5 `NetworkInfo` 网络状态与离线模式　network_info.h/.cpp
- **业务意图**：离线模式下请求必然失败，需要让上报/重试知道「别白费劲」。
- **代码落点**：`SetOfflineMode`（28-33，`offline_mode_.exchange` 变化时才发信号）、`InOfflineMode`（26）。
- **要点**：`UpdateNetworkType`/`UpdateConnectState` 是 `protected`，只有友元 `NetworkMonitorService` 能改（network_info.h:48-51）——**状态写入收口，避免各链路乱改**。

### 3.6 `NetworkStat` 上报与过滤　network_stat.cpp
- **业务意图**：失败要上报，但无网/弱网/离线导致的失败不能刷量。
- **代码落点**：`IsLocalNetworkIssue`（23-33）、`ReportDeviceRequest`（35-60）。
- **要点**：过滤三层——错误码是 20101/20102 → 跳过；离线模式 → 跳过；`!IsConnected()` → 跳过（28-32）。还要**排除上报类请求自身**（47）避免循环上报。成功时更新 `last_net_available_time_`（16/38）供他处判断「网络最后一次可用是什么时候」。

---

## 4. 核心代码实现细节与原理（重点）  [10min+ · 讲透原理]

### 4.1 挑选的核心代码
4 处：**① 错误分类与 error_msg 组装**（三分法 / 20105 拆分 / 砍头保尾 / trace 尾缀）；**② trace 链路**（生成、override 回带、为何不回读共享成员）；**③ 签名体系**（国密 + SE + 防重放）；**④ 连接复用与内嵌 CA**（性能原理）。

### 4.2 专深①：错误分类与 error_msg 组装

**三分法**（`ClassifyCprError` network.cpp:172-207）：
```cpp
if (error) {                                     // 传输层失败
  if (OPERATION_TIMEDOUT)        return 20101;   // 175-177 超时
  if (IsCprNetworkError(code))   return 20102;   // 179-182 连接/DNS/收发
  if (SSL_CONNECT/LOCAL_CERT/REMOTE_CERT) return 20105;  // 183-190 TLS 证书
  return 20104;                                  // 191-193 其他兜底
}
if (4xx) return 20104;  if (5xx) return 20407;   // 196-203 HTTP 状态
return 20104;                                    // 204-206 3xx 等兜底
```
**为什么 20105 要独立拆出来**（原理）：证书/握手失败（含**设备时间不正确**导致的 `not-yet-valid`/`expired`）在私有化部署里高发，且**可行动**——检查证书或 NTP 即可修复。所以它不该混在 20104 里被当成「网络抖动」反复重试；同时 `IsNetworkError`（81-85）**刻意不收录 20105**，让上层不会无意义重试。注释 65-80 把收录规则与「若未来出现 20104 死循环则收敛为 20101+20102」的退路都写清了。

**服务端业务码提取**（`ParseIoTHttpResponse` 231-252）：HTTP 200 也未必成功，body 里 `{code, message}` 才是业务结果。这里用 `try/catch` 包住 JSON 解析（248-251）——**body 不是 JSON 时静默落到后面的 status 分支**，不会崩。

**砍头保尾截断**（`BuildErrorNotification` 56-61）：
```cpp
if (msg.size() > kDefaultErrorMsgMaxLength) {      // 512
  const size_t ellipsis = 3;
  const size_t tail_budget = kDefaultErrorMsgMaxLength > ellipsis ? kDefaultErrorMsgMaxLength - ellipsis : 0;
  msg = "..." + msg.substr(msg.size() - tail_budget);   // 砍头保尾
}
```
**为什么砍头不砍尾**（原理）：0xA5 `error_msg` 是变长 string（uint16 长度前缀），无协议定长约束，异常长文案会溢出语义并撑坏 UI。截断时必须**保留尾部**——尾部是具体失败原因和 `[trace_id:xxx]` 尾缀，是客服唯一的定位凭据；丢了尾缀，前面再有文案也没法查日志。

**trace 尾缀拼接点**（220-221）：
```cpp
const std::string trace_suffix =
    trace_id.empty() ? std::string() : fmt::format(" [trace_id:{}]", trace_id);
```
注意：**拼接发生在网络层的 `ParseIoTHttpResponse`，不是上层的 `BuildErrorNotification`**（后者现已无 trace_id 参数，见 6.2 审阅项）。

### 4.3 专深②：trace 链路（生成 → override → 回带）

```cpp
// network.cpp:275-282  生成（锁内）
std::string Network::GenerateTraceparent(bool start_new_trace) {
  std::lock_guard<std::mutex> lock(trace_mutex_);
  if (start_new_trace || current_trace_id_.empty()) {
    current_trace_id_ = utils::StringUtil::GenerateRandomHexString(32);
  }
  std::string span_id = utils::StringUtil::GenerateRandomHexString(16);
  return fmt::format("00-{}-{}-01", current_trace_id_, span_id);
}

// network.cpp:514-530  override 优先
if (override_trace_id.empty()) {
  traceparent = GenerateTraceparent(true);
  req_trace_id = traceparent.substr(3, 32);   // 按固定位移取 trace 段
} else {
  req_trace_id = override_trace_id;
  traceparent = fmt::format("00-{}-{}-01", req_trace_id, GenerateRandomHexString(16));
}
```
**为什么这么写（原理）**：
- **锁内生成**（276）：`current_trace_id_` 是 `Network` 单例的共享成员，心跳/指令等并发请求都会走这里，必须加锁。
- **`substr(3, 32)` 而非回读共享成员**（524-525，注释明说「避免锁外回读共享成员」）：W3C traceparent 格式固定 `00-{32hex}-{16hex}-01`，trace 段位移恒定。直接从已生成的 header 里切，**既避开锁的范围，也避开被并发请求改写槽位的竞态**——拿到的一定是本次请求的 trace。
- **override 通道**（526-530）：刷掌（story 135784881）与激活会话（story 137332527）由调用方自持 trace 并透传，实现「一次激活尝试一个号」的跨步骤串联；无 override 时网络层每次现起新 trace。
- **trace 随响应带回**（215-218 + 594-595）：`resp.trace_id = trace_id`，供上层失败时拼尾缀/取回会话 trace。这是「所有权随请求流动」的保证——号跟响应一起回来，而不是事后去公共槽位现看。

### 4.4 专深③：签名体系（国密 + SE + 防重放）
```
待签内容 = method + "\n" + path + "\n" + timestamp + "\n" + nonce + "\n" + body + "\n"   (294-307)
   → SHA256 → hex                                                                    (309-312)
   → 签名：SIMPLE/ACTIVATION 用 SM3-HMAC（固定密钥 / session_key_）(344-360, 363-384)
          DEVICE/RECOGNIZE 用 SE 安全模块 SM2（SignDataHash / SignPayAuth）(387-456)
   → base64 → 装进 PalmSignPackage{version, sign, type} protobuf → 再 base64       (407-416)
   → Authorization: <scheme> device_sn="..",nonce_str="..",signature="..",timestamp=".."[,session_id=".."]
                                                                                    (327-341)
```
**为什么这么写（原理）**：
- **method + path + body 进签名**：防止请求被篡改/重放到别的接口；**timestamp + nonce 进签名**防重放（nonce 32 位随机，318）。
- **国密 SM2/SM3**：合规要求；`DEVICE`/`RECOGNIZE` 走 **SE 安全模块**（`SeKeyStore`）而非软件密钥——**私钥不出安全芯片**，这是设备认证的可信根。
- **为什么套一层 protobuf 再 base64**（407-416）：`PalmSignPackage` 带 `version` 与 `type`，让服务端能区分 `DeviceAuthSign` / `DeviceRecogSign` 并保证后续可扩展（proto 注释：字段只增不减）。
- **SE 未初始化直接失败**（394-397, 429-432）：返回空 → 上层 `ERR_PALM_IOT_SIGN_FAILED`（506-512），**绝不退化成不签名请求**。

### 4.5 专深④：连接复用与内嵌 CA（性能原理）
```cpp
// http_request.cpp:49-73  进程内一次性初始化
curl_share_setopt(curl_sh_, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
curl_share_setopt(curl_sh_, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);
curl_share_setopt(curl_sh_, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT);
curl_share_setopt(curl_sh_, CURLSHOPT_LOCKFUNC, CurlShareLockFunc);
cpr::async::startup(..., std::max(CPR_DEFAULT_THREAD_POOL_MAX_THREAD_NUM, 32u));
curl_easy_setopt(..., CURLOPT_CAINFO_BLOB, curl_cacert_blob_);      // 内嵌 CA
curl_easy_setopt(..., CURLOPT_DNS_CACHE_TIMEOUT, 3600L);            // DNS 缓存 1h
```
**为什么这么写（原理）**：
- **share DNS / SSL_SESSION / CONNECT**：设备端请求密集（心跳 + 指令 + 识别），跨请求复用 DNS 结果与 TLS 会话，省掉重复解析与握手；共享对象必须配 `LOCKFUNC`，配合静态 `curl_mutexes_`（15-20, 115）保证线程安全。
- **内嵌 CA 且加密压缩**（22-46）：私有化部署的证书不在系统 CA 里，必须自带；但直接打进二进制等于明文暴露，所以 **AES-GCM 加密 + zstd 压缩**，运行时解密成 `curl_blob` 交给 curl（`CURL_BLOB_NOCOPY` 表示内存常驻、不拷贝）。同时**先禁用系统默认 capath/CAINFO**（67-68）再设 blob，避免两套 CA 混用。
- **DNS 缓存 60s → 1h**（73）：设备端 DNS 稳定，减少解析次数；代价是 DNS 变更后最长 1h 才生效（见 5.2）。
- **线程池提到 32**（62）：默认池太小会让异步请求排队；注释强调 **MUST run first**（必须在其他初始化之前）。

### 4.6 性能与复杂度
- 签名：SHA256 + SM3-HMAC/SM2，O(body)；SE 签名是硬件调用，耗时在毫秒级、不可并行。
- `GenerateTraceparent`：加锁，临界区仅两次随机串生成，O(1)。
- 请求主干：单次 HTTP，无重试（重试由上层按 `IsNetworkError` 决定）。
- 连接复用：跨请求均摊 TLS 握手/DNS 成本，弱网收益最大。

---

## 5. 边界、异常与测试  [5min · 别一笔带过]

### 5.1 异常与降级
- **host 未设置**：`CHECK_IOT_DEVICE_HOST_SET` 宏（94-102）直接返回 `ERR_PALM_IOT_HOST_NOT_SET`，不发请求。
- **签名失败**（SE 未初始化 / 签名返回空）：`ERR_PALM_IOT_SIGN_FAILED`（506-512），**不退化成无签名请求**。
- **不支持的 HTTP method**：`ERR_PALM_IOT_UNKNOWN_ERROR`（576-580）。
- **body 非 JSON**：`try/catch` 吞掉（248-251），落到 status 分支，不崩。
- **离线/无网**：请求照发（网络层不拦），但**不上报**（`NetworkStat` 过滤，network_stat.cpp:42-45）。

### 5.2 边界条件
| 场景 | 行为 / 风险 |
|---|---|
| 域名带/不带 scheme | `SetIotDeviceHost` 统一剥离，两种都能吃（113-121） |
| 激活会话清理 | 激活成功调 `ClearActivationSession`，否则 session_key 残留（143-147） |
| error_msg 超 512 | 砍头保尾，尾缀保留（56-61） |
| TSN/证书错误 | 独立成 20105，且 `IsNetworkError` 为 false（不重试） |
| **query 参数** | ⚠️ `fmt::format("{}={}", k, v)` 手工拼接，**未做 URL 编码**（654-665），value 含 `&`/空格/中文会串参（见 6.2） |
| DNS 变更 | 缓存 1h，最长 1h 才生效（73） |
| 上报类请求失败 | 排除自身 msg_type，避免循环上报（network_stat.cpp:47） |
| TDI 长链 | `DualLinkTdiRequest` 是 stub，返回 `{-1,{}}`（702-707） |

### 5.3 测试覆盖
- **已有**：`network_stat_test.cpp`（6 个用例，覆盖 API 存在性、空 URL/空 msg_type、连续上报不 crash）、`network_info_test.cpp`。
- **已覆盖（设计上）**：错误分类各分支（超时/网络/SSL/4xx/5xx/业务码）在 `ClassifyCprError`/`ParseIoTHttpResponse` 有明确分支与日志。
- **未覆盖（open）**：① 签名正确性（SM2/SM3）无单测，依赖联调；② `ClassifyCprError` / `ParseIoTHttpResponse` / `BuildErrorNotification` / `GenerateTraceparent` **均无单测**，纯函数却只能靠真机/真实网络验证；③ trace 尾缀与 override 串联只靠人工抓日志回归；④ 内嵌 CA 解密失败路径（返回 nullptr → 回退系统 CA）无用例。

---

## 6. 踩坑与接手注意事项（含审阅发现）  [审阅 + 雷区]

### 6.1 开发中坑
- **TLS 拆分要同步改两处**：新增 20105 后，`ClassifyCprError` 出码（183-190）与 `IsNetworkError` 收录表（81-85）必须同步——**只改出码不改收录，会让证书错误被无限重试**。
- **上报的静默过滤**：本地网络问题直接 `return` 不上报（network_stat.cpp:42-45），排查时「后台看不到失败」往往是**被过滤了**，不是没发生。
- **`SCOPE_EXIT` 会让早退路径也上报**：host 未设/签名失败同样会进上报统计，看数据时要区分 error_code。

### 6.2 接手者雷区地图（⚠️ 本次审阅发现的偏差）
审阅 `facility/network` 当前代码，发现 **7 处值得注意的偏差/风险**，接手与评审请以代码为准：

| # | 现象 | 代码位置 | 风险 / 建议 |
|---|---|---|---|
| **N1** | **网络失败分支也拼 trace 尾缀**，与设计原则冲突 | `ParseIoTHttpResponse` 227：`error_obj.message + trace_suffix` | ADR 明确「压根没连上后台（服务端没收到请求，查也查不到）」才不带号；但当前 `error_obj` 分支（连接失败/超时/SSL）**无条件拼了尾缀**，客服拿号去查会落空。需与 O4 规范「仅后台返回错误场景拼接」对齐确认 |
| **N2** | `IoTResponse.trace_id` 实际是**完整 traceparent**，与注释不符 | 595 传 `traceparent`（`00-{32hex}-{16hex}-01`），而注释 514-518 说「裸 trace_id（32 hex）随响应带回」 | 上层若按「32 hex」解析 `trace_id` 会拿到带前后缀的整串。**要么改代码传 `req_trace_id`，要么改注释**，二选一必须统一 |
| **N3** | `req_trace_id` 在非 override 分支是**死变量** | 519 声明、525 赋值，但 595 用的是 `traceparent`；仅 override 分支（527-528）真正用到 | 死存储，编译无警告但误导读者以为响应带的是它。建议直接用或删 |
| **N4** | `use_compression` 参数**完全未实现** | network.h:146/158/170/181/213 声明，network.cpp:616-618 等实现忽略 | 调用方传 `true` 也不会压缩，属**接口谎言**。要么实现要么删参数 |
| **N5** | **固定签名密钥硬编码在源码** | network.cpp:91 `kSimpleSignKey` | 明文密钥入二进制，可逆向。SIMPLE 仅用于非敏感接口（激活 SIMPLE），仍建议评估是否改为下发/编译期注入 |
| **N6** | query 参数**未做 URL 编码** | network.cpp:654-665 手工 `fmt::format("{}={}")` 拼接 | value 含 `&`、`=`、空格、非 ASCII 时会串参或被服务端拒。建议改用 cpr `Parameters` 或显式编码 |
| **N7** | `DualLinkTdiRequest` 是 stub，返回 `{-1,{}}` | network.cpp:702-707（声明处 network.h:93 已标 todo） | 若上层误调用会拿到 `-1` 却无日志级别告警，建议加 `LOG_E` 或断言 |

**另外两处需要同步修正的历史表述**（与上一篇 O4 串讲相关）：
- `BuildErrorNotification` **已不再是模板函数**：现为 `std::pair<int,std::string> BuildErrorNotification(int, const std::string&, int, ServerCodeMapper)`（network.h:28-30、network.cpp:45-63），**且不再接收/拼接 trace_id**。上一篇 O4 串讲里引用的「模板版 + trace 尾缀」已过期，需更新。
- `IsNetworkError` **确实存在**，位于 `Network::IoTResponse`（network.h:79-81、network.cpp:81-85），且**确实不收录 20105**（符合设计）。上一篇 O4 串讲的 R3 项表述不准确，应修正为「spec 所指函数位于 network 模块而非 `unified_errcode_iot.h`」。

**给接手者的硬提醒**：
- 🔴 改错误码出码（`ClassifyCprError`）时，**必须同步检查 `IsNetworkError` 收录表**，否则重试语义错乱。
- 🔴 改 trace 相关代码前，先确认 N2/N3 的口径（响应到底带 32hex 还是完整 traceparent）并统一注释与实现。
- 🟡 截断逻辑是**砍头保尾**，改动时别把尾缀砍掉——那是客服唯一凭据。
- 🟡 `HttpRequest` 的初始化是 `StaticInitializer` 一次性执行，**线程池 startup 必须最先跑**（注释 61），调整初始化顺序会出诡异问题。
- 🟡 新增请求接口请复用 `IotDeviceRequestImpl`，别另起炉灶直接调 `HttpRequest`，否则会丢签名/trace/上报三件套。

---

## 7. 参考文档
- 模块路径：`palm_app_linux/src/facility/network/`（`network.h/.cpp`、`http_request.h/.cpp`、`network_info.h/.cpp`、`network_stat.h/.cpp`、`server_errcode.h`、`palm_sign_package.proto`）
- 测试：`network_info_test.cpp`、`network_stat_test.cpp`
- 服务端码字典：`server_errcode.h`（当前仅 `kActSvrCodeDeviceNotRegistered = 268566642`；识别/注册的 `kRecCode*` 仍在 `biz/comm/palm_flow/recognize_retcode.h`）
- 错误码常量：`comm/constant/unified_errcode_iot.h`、`comm/constant/module_error_code.h`
- 相关 story：137332527（O4 错误码治理 / TLS 拆分 20105 / trace 尾缀）、135784881（刷掌 trace 透传）
- trace 链路 ADR：`palm_app_linux/docs/specs/2026-08-28-trace-id-start-new-trace-analysis.md`
- O4 治理串讲（上一篇，部分表述待按 6.2 修正）：`06.技术模版/137332527-o4-error-code-governance-业务串讲.md`
