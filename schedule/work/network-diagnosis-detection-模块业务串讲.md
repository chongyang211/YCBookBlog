# network/diagnosis/detection 四层网络检测 · 业务串讲

> **一句话定位**：设备「连不上后台」时，把故障定位到「物理层 → 本地层 → 连通层 → 服务层」四层之一，并给出一码一场景的诊断码（10011~10047）与逐项明细（items）——上游故障未排除时测下游无意义，所以四层串行 fail-fast。
> **场次**：60 分钟（策略模式 + 大量 Linux 系统文件判读，难点集中在判定链设计）　**听众**：同组开发 / 评审人 / 后续接手者
> **对照模版**：`06.技术模版/04.业务串讲的模版.md` 第 08 章　**代码基线**：`palm_app_linux/src/facility/network/diagnosis/detection/`（Story 137345399，2026-09-08 审阅）
> **模块文件**：`network_detector_manager.h/.cpp`（编排器）+ `base_network_strategy.h/.cpp`（基类/结果结构）+ 4 个策略（physical / local / connectivity / service）

---

## 1. 需求与业务背景  [10min · 先现象后价值]

### 1.1 需求来源与问题
- **来源**：Story 137345399（O4 网络诊断 T1.2/T1.4），1:1 移植自 O2 `iotservice_linux` 的策略实现，叠加 O4 增强（items 逐项明细、一码一场景细分码、异常分层赋码）。
- **问题**：现场「设备断网」工单，客服只能看到「连不上」，分不清是**没插网线 / 网段配错 / 网关挂了 / DNS 挂了 / 后台挂了**——五类问题的处理方完全不同（用户自查 vs 运维改配置 vs 后台值班）。旧四层判定还有两类实战误报：网关禁 ping 误报「网关不可达」、双网口 metric 反了要到最后服务层才失败。
- **目标**：一次检测输出（1）**首败层级** + 精确诊断码；（2）**逐项明细**（每层每个检查动作的实测值，如 `eth0 carrier = 0 (no link — check cable / switch port)`）直达 Debug 页；（3）**trace_id** 透出，运维免导设备日志即可到后台检索。

### 1.2 业务边界
- **做**：四层串行检测、一码一场景诊断码（10011~10047）、items 逐项明细跨层累积、异常分层兜底码、awaitable/callback 双异步形态、超时构造注入。
- **不做**：公网 ping（M3 定案：私有化客户环境公网可能被防火墙禁，公网不通 ≠ 网络不可用）；重试（检测即诊断，不自带恢复）；测速（在兄弟模块 `speedtest/`，由上游 `network_diagnosis` 门面统一编排）；自动触发管理（上游 `NetworkDiagnosis` 持有状态，FR-2.3）。
- **上游**：`diagnosis/network_diagnosis.cpp:33-48` 薄委托本模块（协程/callback 两种形态）；`speedtest/weak_level.h:105-107` 用 fail-fast **首败结果**判定是否归入断网态（物理/本地层失败属设备侧配置问题，不归断网）。

### 1.3 关键业务规则
- **四层顺序**（Manager 编排，network_detector_manager.h:26-29）：
  | 层 | 回答的问题 | 诊断码段 |
  |---|---|---|
  | Physical 物理层 | 网卡/接口/载波/IP 在不在 | 10011~10015 |
  | Local 本地层 | 网关/路由/网段配置对不对 | 10021~10025 |
  | Connectivity 连通层 | 网关 ping 得通吗（唯一判据） | 10031~10032 |
  | Service 服务层 | 后台健康吗，挂了是链路还是业务 | 10041~10047 |
- **fail-fast**：任一层失败立即返回该层结果，不再往下——「上游故障未排除时测下游无意义」（网线都没插，测 DNS 是浪费 5 秒）。
- **一码一场景**：每个失败形态独立成码；策略抛异常按**所在层**赋 `*_EXC` 码（10015/10023/10032/10044），不再统一复用服务层的 10044（network_detector_manager.cpp:64-76）。
- **items 口径**：`layer` 用小写短名（physical/local/connectivity/service），`DetectionResult.level` 的 JSON 形态用大写（Physical/Local/...）——**两口径有意区分**（上报/wire/推送用短名，JSON 结构字段用大写），测试有断言勿改。

### 1.4 改动范围（相对 O2 原版）
- **[O4 增强]** `DetectionCheckItem` 逐项明细 + Manager 跨层累积打标（base_network_strategy.h:38-43、manager.cpp:45-58）；异常分层 `*_EXC` 码（原统一 10044）；本地层新增 10024（网关-网段一致性）/ 10025（metric 出口无 IP）两码（M3/M4 实战反推）。
- **[有意偏离 O2]** 策略集从「Manager 成员持有」改为「每次检测局部构造」——两个理由：`service_timeout` 运行时注入（成员持有就得加 setter/锁）、局部无共享即免锁线程安全（manager.cpp:32-33）。
- **[O2 保真]** ping 走 `/bin/ping` 子进程 + popen；`/license` 健康检查端点；callback 在网络线程回调的语义。

---

## 2. 整体设计与模块划分  [10min · 先图后讲]

### 2.1 架构与分层
```
 0x15 通道  network_diagnosis（门面：检测+测速+自动触发状态机）
                 │ 薄委托（T1.4）
 ┌───────────────┴────────────────────────────────────────────┐
 │ NetworkDetectorManager（单例壳，无状态成员）                  │
 │   for strategy in [Physical, Local, Connectivity, Service]: │
 │     result = strategy->Execute()                            │
 │     items 打层级标签并入 accumulated_items                   │
 │     fail-fast：!success → return result                     │
 │     异常 → 按层赋 *_EXC 码                                  │
 │   全过 → {level=service, code=0, "Network OK"}              │
 ├────────────────────────────────────────────────────────────┤
 │ BaseNetworkStrategy（抽象：GetName / Execute / ExecuteAsync）│
 │   ├ PhysicalLayerStrategy   getifaddrs + /sys/class/net/*   │
 │   ├ LocalNetworkStrategy    /proc/net/route + getifaddrs    │
 │   ├ ConnectivityStrategy    popen ping + /proc/net/arp      │
 │   └ ServiceStrategy         /license 健康检查 + HTTP 探测    │
 └────────────────────────────────────────────────────────────┘
```
分层关键：**Manager 只管编排与汇总，不碰任何系统细节；每个 Strategy 只负责自己那层的判定与明细产出**——新增一层（如代理层）= 新增一个 Strategy + Manager 数组加一行 + 一个码段。

### 2.2 模块划分与职责
| 模块 | 职责 | 切分理由 |
|---|---|---|
| `NetworkDetectorManager` | 串行 fail-fast 编排、items 打标累积、异常分层赋码、三形态入口 | 编排与判定解耦；单例壳无状态 → 免锁 |
| `BaseNetworkStrategy` | 抽象接口 + `DetectionResult`/`DetectionCheckItem` 结构 + JSON 序列化 | 结果结构统一，Debug 页只有一个渲染口径 |
| `PhysicalLayerStrategy` | 接口枚举/UP/载波/IPv4 四步收窄 | 全部纯系统文件读，毫秒级 |
| `LocalNetworkStrategy` | 网关/网段一致/直连路由/metric 出口四项判定 | 纯 `/proc/net/route` + getifaddrs，零网络请求 |
| `ConnectivityStrategy` | 网关 ping（唯一判据）+ ARP 佐证 | 唯一需要子进程的层 |
| `ServiceStrategy` | host 配置 → `/license` 健康检查 → HTTP 探测细分 | 唯一产生真实网络流量的层 |

### 2.3 核心数据流（一次断网检测）
```
0x15 触发 → network_diagnosis 门面
 → co_await Manager::PerformDetectionAwaitable()          manager.cpp:101
 → NetworkThread 上执行 PerformFullDetection()
 → ① Physical：eth0(ethernet) operstate=up carrier=1 ipv4=192.168.9.84/24 → 过
 → ② Local：gateway=192.168.9.1 在 eth0 网段内、直连路由在、metric 出口有 IP → 过
 → ③ Connectivity：ping 网关超时；ARP 表 resolved (L2 reachable) → 失败 10031
 → items=[①4项+②4项（已执行，带层级标签），③2项] fail-fast 返回
 → 门面组装 DetectionReport → 0x15 resp details.items → Debug 页逐行渲染
 （未执行的 Service 层天然无 items，UI 渲染为 SKIP）
```

---

## 3. 逐模块实现讲解  [25min · 业务意图先行]

### 3.1 `BaseNetworkStrategy` 与结果结构　base_network_strategy.h:21-90
- **业务意图**：所有层产出同一种结果形状，Debug 页只有一套渲染逻辑。
- **要点**：`DetectionResult{success, level, strategy_name, error_code(10xxx), error_message, details(旧通道 map), items(新通道明细)}`（51-62）。`items` 空时不输出 JSON 字段（base_network_strategy.cpp:54-62）——**保持旧形态**，老 UI 不受影响；`details` 是兼容旧 UI 的旧通道，新 UI items 非空时不再 dump details（local_network_strategy.cpp:69-70 注释点明了这个取舍）。

### 3.2 `PhysicalLayerStrategy`：四步收窄　physical_layer_strategy.cpp:158-311
- **业务意图**：回答「本机有没有一块能用的网卡」，四步逐步收窄候选集：
  1. **枚举接口**（63-99）：getifaddrs，排除 `lo`（回环永远 UP 无代表性）/ `tun*`（VPN）/ `docker*` / `br-*`（容器网桥）（48-58）。全无 → 10011。
  2. **UP 状态**（197-215）：读 `/sys/class/net/{iface}/operstate`；**"unknown" 视作 UP**（与 iproute2 语义一致，lo/虚拟接口惯例不上报 operstate，避免误判，108-114 注释）。全 DOWN → 10012（值展示 `down (admin disabled)`——区别于网线问题，启用即恢复）。
  3. **载波**（231-246）：读 `carrier` 文件，`1`=有链路、`0`=没插线；**接口未 UP 时读它会 EINVAL**，展示为 `n/a (interface not up)` 与「没插线」区分（116-125）。全无载波 → 10013。
  4. **IPv4**（262-287）：getifaddrs 取首个非 0 地址，**CIDR 形式**（`192.168.9.84/24`，掩码 `__builtin_popcount` 换前缀长度，145-148）——排查网段/掩码错配。全无 IP → 10014。
- **细节**：接口类型标注 `phy80211` 存在 = wireless、`usb*`/`can*` 按前缀（174-179），items 里 `eth0(ethernet)` 让用户先确认检测范围。

### 3.3 `LocalNetworkStrategy`：四项配置判定　local_network_strategy.cpp:27-186
- **业务意图**：本机网络**配置**对不对（不产生任何网络流量，纯文件读毫秒级）：
  1. **默认网关**（38-81）：`/proc/net/route` 中 `Destination=00000000` 的行。无 → 10021。多路由时输出全部条目+metric（M3 落点 #3：以太网 metric 更高 → 走以太网反而断网）。
  2. **网关-网段一致性**（83-115）：网关必须落在某接口直连网段内（`gw & mask == network`），否则内核根本送不到网关。不在 → 10024。**此前该场景会到连通层误报 10031「网关不回应」，实为配错网段**——归类分离后排查方向精准（注释 83-85）。
  3. **本地直连路由**（117-139）：`Gateway=00000000` 且 dest 非 0 的行，过滤回环和网关主机路由 /32（321-408）。无 → 10022。
  4. **metric 最优路由出口有效性**（141-171）：内核实际选路 = metric 最小的默认路由，其出口接口**必须有 IP**。没有 → 10025。M3 实测形态：wlan0 metric 更优但没拿到 IP → 流量黑洞；原四层全过（走 eth0 路径），到服务层才失败——**根因在此闭环**（注释 141-144）。
- **细节**：判定覆盖沿用 O2 语义——**后判定项覆盖前判定项**（如网关先失败又被路由失败覆盖成 10022，注释 133-134 有意保留）。

### 3.4 `ConnectivityStrategy`：网关 ping + ARP 佐证　connectivity_strategy.cpp:25-70
- **业务意图**：局域网基础链路通不通。**唯一判据 = ping 网关**（禁公网 ping，M3 定案）。
- **判定**：`popen("ping -c 1 -W 3 <网关>")`，**以退出码为准**（不受 ping 版本输出差异影响，148）；失败 → 10031。
- **ARP 佐证**（不参与判定，196-222）：ping 失败后读 `/proc/net/arp`，`ATF_COM(0x2)` 位 = 已解析到 MAC。**「ping 失败 + ARP resolved」= 网关禁 ping（ICMP 被过滤，业务多半正常）；「ping 失败 + 无表项」= 真二层不通（查交换机/VLAN/网线）**——这是 10031 高频误报的判读依据（connectivity_strategy.h:29-34）。
- **RTT 解析**（130-147）：iputils（`rtt min/avg/max/mdev`）与 busybox（`round-trip min/avg/max`）双格式容错，解析失败不影响判定。

### 3.5 `ServiceStrategy`：三步判定 + 环境留证　service_strategy.cpp:26-177
- **业务意图**：后台活着吗；挂了的话，是链路问题还是业务问题。
- **第 0 步 环境事实输出**（不参与判定，纯线索）：DNS 列表（`/etc/resolv.conf` nameserver 行，最多 5 个防撑爆，207-234）与**系统时间**（47-54）——TLS 证书校验失败（10046）与签名时间戳偏差（10042）的高频根因都是设备时间错，直接展示便于肉眼核对。
- **第 1 步 host 配置**（56-70）：未配置 → 10041（先配 host 再谈连通）。
- **第 2 步 业务健康检查**（72-111）：`ActivationGetRequest("/license")`（O2 保真端点，超时构造注入默认 5s）；`error_code==0 && server_code==0` 即成功。**trace_id 成败均透出**（90-94）——运维凭它直接到后台检索本次请求，无需导设备日志。
- **第 3 步 基础 HTTP 探测**（113-168，仅第 2 步失败后执行）：直接 GET host（不带业务路径，**3s 短超时**——判定「链路是否通」用更短超时，与 5s 健康检查区分）。按 cpr 错误类别一码一场景：能拿到任何 HTTP 响应（4xx/5xx 也算链路通）→ 10042；DNS → 10045；TLS → 10046；超时 → 10047；其余传输不可达 → 10043。

### 3.6 `NetworkDetectorManager`：编排器　network_detector_manager.cpp:30-122
- **业务意图**：把四个策略串成一次检测，统一处理 items 汇总与异常兜底。
- **要点**：策略集**局部构造**（34-38，偏离 O2 的理由见 1.4）；items 打层级标签累积（45-58）；fail-fast（59-62）；异常按层查表赋码（64-86，`kLayerExcCodes[]`/`kLayerLevels[]` 两个平行数组）；三种入口形态（见 4.3）。

---

## 4. 核心代码实现细节与原理（重点）  [10min+ · 讲透原理]

### 4.1 挑选的核心代码
4 处：**① fail-fast + items 跨层累积**（编排语义）；**② 一码一场景的错误码设计**（10024/10025/ARP 判读——从实战误报反推）；**③ 三种异步形态与生命周期守卫**（协程桥 + alive_flag_）；**④ ping 的安全与权限设计**（防注入 + setuid）。

### 4.2 专深①：fail-fast 编排 + items 跨层累积
```cpp
// manager.cpp:45-62（节选）
std::vector<DetectionCheckItem> accumulated_items;
for (size_t idx = 0; idx < strategies.size(); ++idx) {
  DetectionResult result = strategy->Execute();
  const std::string layer = DetectionLevelToString(result.level);
  for (const auto& item : result.items) {
    DetectionCheckItem tagged = item;      // 拷贝后打标
    tagged.layer = layer;
    accumulated_items.push_back(std::move(tagged));
  }
  result.items = accumulated_items;        // 跨层累积塞回
  if (!result.success) return result;      // fail-fast
}
```
**为什么这么写（原理）**：
- **fail-fast 的语义依据**：四层是**因果链**（物理通 → 配置对 → 链路达 → 服务活）。上游故障未排除时测下游，结果不可解释（网线没插时 DNS 必然失败，那个失败是噪音）。fail-fast 保证「首败即根因候选」，`weak_level` 据此归断网态。
- **为什么 items 要跨层累积而不是各层自报**：失败返回时用户需要看到**已执行层的全量明细**（物理 4 项 + 本地 4 项都过，才在连通层挂）——这证明「问题不在前面」，是定位信息的一半。未执行层**天然无 items**（没有代码路径产生它），UI 渲染为 SKIP——不需要显式 SKIP 标记，**用数据缺失表达状态**。
- **为什么 Strategy 内 record 时 layer 留空、由 Manager 打标**（physical_layer_strategy.cpp:163-168）：层级归属是**编排事实**（谁在什么位置执行），不是策略自身知识——策略只产出 name/value/pass 三元组，职责更纯粹，也避免策略写错自己的层名。

### 4.3 专深②：一码一场景——从实战误报反推的判定
错误码全景（10xxx 段，JSON 数字化透传）：
```
物理 10011 无接口 | 10012 全DOWN | 10013 无载波 | 10014 无IP | 10015 异常
本地 10021 无网关 | 10022 无本地路由 | 10024 网关不在网段 | 10025 出口无IP | 10023 异常
连通 10031 网关不可达 | 10032 异常
服务 10041 host未配 | 10042 API失败(链路通) | 10043 传输不可达 | 10044 异常 | 10045 DNS | 10046 TLS | 10047 超时
```
**为什么这么设计（原理）**：
- **每码对应一个可行动的排查动作**：10013 → 查网线/交换机端口；10014 → 查 DHCP；10024 → 修正网关/网段；10025 → 调 metric 优先级；10045 → 对照 DNS 明细查配置；10046 → 查证书/**设备时间**。码是给人指路的，不是给机器分类的。
- **10024/10025 是从 M3/M4 实战反推出来的**：原四层判定里，「网关配错网段」会漏到连通层报 10031（网关不回应）——排查方向整个错了；「metric 出口无 IP」四层全过、到服务层才失败——中间三层全是绿灯的假象。**新码的本质是把「配置错误」从「链路不通」的语义里拆出来**。
- **10042 与 10043-10047 的拆分**：健康检查失败后先做一次 3s 的裸 HTTP 探测——**通 = 链路 OK 业务挂（10042，找服务端值班）**，不通再按 cpr 错误细分成 10045/10046/10047/10043（找网络/运维）。一次探测把「找谁」的问题回答掉。
- **异常分层赋码**（manager.cpp:64-86）：策略抛异常不再统一复用服务层的 10044，而是按 `kLayerExcCodes[]` 查表——否则物理层异常会被当成服务层异常，误导排查方向。

### 4.4 专深③：三种异步形态与生命周期守卫
```cpp
// 形态一：同步（阻塞秒级）                     manager.cpp:30
DetectionResult PerformFullDetection(service_timeout);
// 形态二：协程（co_spawn 标准桥，零忙等）       manager.cpp:101-112
co_await boost::asio::co_spawn(Threads::NetworkThread()->Executor(),
    [service_timeout]() -> boost::asio::awaitable<DetectionResult> {
      co_return ...PerformFullDetection(service_timeout);
    }, boost::asio::use_awaitable);
// 形态三：callback（非协程上下文可安全调用）    manager.cpp:114-122
Threads::NetworkThread()->Post([cb, service_timeout]() { cb(...PerformFullDetection(...)); });
```
**为什么这么写（原理）**：
- **阻塞检测必须离开调用者线程**：最坏叠加约 11s（ping 3s + `/license` 5s + 探测 3s）。协程形态把阻塞体投递到 `NetworkThread` 的 executor，调用协程挂起等待（`use_awaitable`），**零忙等**、异常沿 asio 语义重新抛出——这是 asio 的「标准桥」写法，不是自造轮子。
- **callback 形态给非协程上下文**：没有 `this_thread` executor 的老代码也能安全调用；语义保真 O2（callback 在网络线程回调）。
- **ExecuteAsync 的生命周期守卫**（physical_layer_strategy.cpp:313-340，四策略同构）：
```cpp
std::weak_ptr<bool> weak_flag = alive_flag_;          // 成员 shared_ptr<bool>
Threads::NetworkThread()->Post([this, weak_flag, cb]() {
  if (weak_flag.expired()) { /* 策略已析构，回调错误结果，绝不碰 this */ }
  ... this->Execute() ...
});
```
  **为什么是 `shared_ptr<bool>` 而不是裸 `this`**：Post 出去的 lambda 生命周期不受控，Strategy 可能先析构——lambda 里若直接 `this->Execute()` 就是 UAF。`alive_flag_` 随对象析构释放，`weak_ptr.expired()` 即「对象已死」信号，过期分支只组错误结果、不触碰任何成员。**注意**：`expired()` 检查与 `Execute()` 之间理论上仍有窗口（检查通过后对象恰好被析构）——当前安全靠 Manager 的**局部构造 + 同步调用**保证（对象生命周期覆盖整个调用），独立使用 ExecuteAsync 需自行保证生命周期覆盖回调（见 6.2-D5）。

### 4.5 专深④：ping 的安全与权限设计
```cpp
// connectivity_strategy.cpp:105-115
in_addr addr;
if (inet_pton(AF_INET, host.c_str(), &addr) <= 0) return false;  // ① 先校验
const std::string cmd = "ping -c 1 -W 3 " + host + " 2>/dev/null";
FILE* fp = ::popen(cmd.c_str(), "r");                            // ② 再拼接
```
**为什么这么写（原理）**：
- **命令注入的防法是「校验输入」而不是「转义」**：host 来自 `/proc/net/route` 解析，若解析有缺陷拼出 `8.8.8.8; rm -rf /`，`inet_pton` 校验后**纯 IPv4 才放行**——校验过的 IP 无 shell 元字符，popen 拼接无注入空间。这是「先验证、后拼接」的安全模式（比事后转义更可靠）。
- **权限靠系统而不是自己**：`/bin/ping` 由系统 setuid / file capabilities 提权，进程自身**无需 CAP_NET_RAW**（注释 110-113）——最小权限原则，模组进程不持网络原始套接字权限。
- **判定用退出码而非输出**（148）：`WIFEXITED(ret) && WEXITSTATUS(ret)==0` 是权威口径，iputils/busybox 输出差异只影响 RTT 明细解析（双格式容错，解析失败不输出、不影响判定）。
- **popen 失败直接判不可达**（116-120）：fork/管道资源不足时不喂非法值给 `WIFEXITED`。

### 4.6 性能与复杂度
- 物理/本地层：纯文件读（getifaddrs / /proc / /sys），毫秒级、零网络流量。
- 连通层：1 包 ping，3s 封顶。
- 服务层：`/license` 5s（构造注入）+ 探测 3s，仅失败才做探测。
- 全程最坏 ~11s；正常路径（四层全过）≈ 1 个 RTT + 一次 HTTP，亚秒到秒级。items 打标为 O(总项数) 拷贝，可忽略。

---

## 5. 边界、异常与测试  [5min · 别一笔带过]

### 5.1 异常与降级
- 策略内 `try/catch`：各层自己兜 `*_EXC` 码返回（不向上抛）；Manager 再兜一层按层查表赋码（双保险，manager.cpp:64-86）。
- 文件读失败统一降级为展示值（`"unreadable"` / `"UNKNOWN"` / `"n/a"`），不 crash；`getifaddrs`/`popen` 失败有独立分支。
- 异步守卫：策略已析构 → 回调错误结果而非崩溃。

### 5.2 边界条件
| 场景 | 行为 |
|---|---|
| 无任何真实网卡（全虚拟口） | 10011，`interfaces=none` |
| operstate=unknown（lo/虚拟口惯例） | 视作 UP（iproute2 语义），展示 `unknown (treated as UP)` |
| 接口 UP 但 carrier 不可读（EINVAL） | 展示 `n/a`，区别于「没插线」，不算载波通过 |
| 网关字段解析异常 | 返回 "UNKNOWN"，判 10021 |
| 双默认路由（多网口） | 输出全量 + metric 对照；独立 `default routes` 行（不能只进 details——新 UI items 非空不 dump details） |
| resolv.conf 无 nameserver / 异常行 | 输出 `not configured`（空配置本身即线索）；`nameserverfoo` 前缀误匹配由第 11 字符空白校验挡掉（service_strategy.cpp:219-220） |
| ping 输出无 RTT 行（busybox 差异） | 不输出该明细，判定不受影响 |
| host 未配置 | 10041，不发起任何网络请求 |
| 健康检查 HTTP 200 但 server_code≠0 | 判失败，error 拼进明细；探测细分接手 |

### 5.3 测试覆盖
- **已有**：`DetectionResult::ToJsonString` 的 level 大小写形态有测试断言（base_network_strategy.cpp:11 注释「测试有断言勿改」）。
- **未覆盖（open）**：detection 目录内**无单测文件**（对照 network 模块有 `network_stat_test.cpp`）——四层判定、码映射、items 打标、异常分层码全部依赖真机/联调验证。`/proc` 类输入高度依赖环境，可测性改进方向是把文件读取抽成可注入接口。接手者改判定逻辑时，**0x15 联调 + Debug 页逐项目检是当前唯一回归手段**，改动请附真机截图。

---

## 6. 踩坑与接手注意事项（含审阅发现）  [审阅 + 雷区]

### 6.1 开发中坑
- **「判定过 = 配置对」的假象**：M3 实测「metric 反了」问题四层全过、最后服务层才失败——**通过前三层不代表流量路径健康**，10025 就是为堵这个洞加的。新增判定项时先问「这个问题现在会漏到哪一层误报」。
- **details 与 items 双通道**：新 UI items 非空时不 dump details——**重要信息必须进 items**，只进 details 的新版 App 看不到（local_network_strategy.cpp:68-70 已有先例：多路由为此独立成行）。
- **O2 保真清单**：`/license` 端点、callback 在网络线程回调、后判定覆盖前判定——这三处是**有意保持**的旧行为，别当 bug「修」掉。

### 6.2 接手者雷区地图（⚠️ 本次审阅发现的偏差）
| # | 现象 | 代码位置 | 风险 / 建议 |
|---|---|---|---|
| **D1** | **物理层 .h 注释的 10013/10014 与代码互换** | physical_layer_strategy.h:38-44 说「无载波→10014、无 IP→10013」；**代码实际**：无载波→`NO_CARRIER`=**10013**（.cpp:249）、无 IP→`NO_IP`=**10014**（.cpp:276） | 注释错、代码对（与码段命名一致）。**改码前先改注释**，否则按注释查码表会查反 |
| **D2** | Manager .h 注释「本地层（10021~10023）」过期 | network_detector_manager.h:27 | 本地层实际覆盖 10021~10025（10024/10025 为 O4 新增）。注释未跟上，串讲/评审以码表为准 |
| **D3** | 「阻塞秒级：ping 3s + HTTP 5s」漏算探测 | network_detector_manager.h:49 | 最坏叠加 ≈ **11s**（ping 3 + `/license` 5 + 探测 3）。调用方做超时预算时别按注释的 8s 算 |
| **D4** | level 大写（JSON）与 layer 小写（items）**双口径并存** | base_network_strategy.cpp:11-25 vs 30-43 | **有意设计**（JSON 结构字段 vs 三通道短名），测试有断言。别「顺手统一」——会同时 break 测试与下游解析 |
| **D5** | `ExecuteAsync` 的 `expired()` 检查与 `Execute()` 之间有理论竞态窗口 | 各策略 `ExecuteAsync`（如 physical_layer_strategy.cpp:315-328） | 当前安全依赖 Manager 局部构造+同步调用；**独立调用 ExecuteAsync 必须保证策略生命周期覆盖回调完成**，否则仍是 UAF 风险 |
| **D6** | 健康检查端点 `/license` 硬编码 | service_strategy.cpp:240-243 | O2 保真端点，后续端点变更需**与 O2 同步调整**（代码注释已提示）——单边改会造成双端探测结果不一致 |
| **D7** | `PerformDetectionAsync` 的 callback 在 NetworkThread 回调 | manager.cpp:117-121 | O2 语义保真。callback 里做重活/再发起阻塞请求会**阻塞网络线程**（心跳等都在这条线程上），调用方须只做轻量转发 |

**给接手者的硬提醒**：
- 🔴 新增判定项时，**诊断码必须新开一码一场景**，不要复用现有码拼 message——码是排查指路的唯一机器可读口径。
- 🔴 items 的 `layer` 字段由 Manager 打标、Strategy 内留空——新策略照此约定，别自己填（Manager 会覆盖，但阅读时容易误解）。
- 🟡 改异常码表（`kLayerExcCodes[]`/`kLayerLevels[]`）是**平行数组**，增删层必须两表同步，且顺序 = 策略注册顺序（manager.cpp:34-38）。
- 🟡 ping 拼接前必须过 `inet_pton` 校验（安全模式「先验证后拼接」），别为支持 hostname 改成直接拼——要支持 hostname 就换成不落 shell 的实现。
- 🟡 `details` 是兼容旧 UI 的旧通道，新信息一律进 `items`。

---

## 7. 参考文档
- 模块路径：`palm_app_linux/src/facility/network/diagnosis/detection/`（manager + base + physical/local/connectivity/service 六组 .h/.cpp）
- 上游门面：`diagnosis/network_diagnosis.h/.cpp`（检测+测速编排、自动触发状态机 FR-2.3）
- 断网等级判定：`diagnosis/speedtest/weak_level.h`（fail-fast 首败结果 → 断网态）
- 错误码常量：`comm/constant/unified_errcode_iot.h`（ERR_PALM_NETDIAG_* 10xxx 段）
- 依赖的网络设施：`facility/network/network.h`（`GetIotDeviceHost`/`ActivationGetRequest`）、`facility/network/http_request.h`（探测）、`facility/thread/threads.h`（NetworkThread）
- Story：137345399（O4 网络诊断 T1.2/T1.4）；关联 story 137332527（O4 错误码治理——TLS 错误集合与 `ClassifyCprError` 同源先例）
- 移植来源：O2 `iotservice_linux/src/operations/strategy/`（各 .cpp 文件头标注 1:1 移植与差异）
- 相关串讲：`schedule/work/network-facility-模块业务串讲.md`（network 主模块：签名/错误分类/trace 体系）
