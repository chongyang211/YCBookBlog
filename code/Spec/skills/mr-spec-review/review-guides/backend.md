# 后台服务端 评审依据（Backend Review Guide）

> 适用：Go 后台服务（`<设备管理仓库>`、`<业务主仓库>`、`<业务主仓库>`、`<激活服务仓库>`、`<IoT 服务仓库>` 云侧、`infrastructure`）。
> 技术栈：Go 1.23+ / gRPC + grpc-gateway（双端口）/ MySQL(gorm) + Redis / Kafka·pubsub / OpenTelemetry / 国密 SM2·SM4 + KMS。
> 加载时机：MR 命中上述目录时，Step 4.1（文档层）与 Step 4.5（代码验证）前读完本文件。

---

## 1. 架构与分层约定（评审时的"正确形态"基准）

标准 DDD 分层（各后端仓基本一致）：

```
gateway/       对外入口：鉴权 / 限频 / 错误翻译 / 路由转发（自身无业务逻辑）
   │ gRPC
application/   可独立部署的微服务：service 实现 gRPC 接口，参数校验 + 编排
controller/    用例编排层（按业务域拆子包）
domain/        领域模型与领域行为（DDD 充血模型，poHolder 泛型持有 PO）
repo/          仓储实现（MySQL / Redis / ES），匿名 import 注册 DI
common/        横切能力（crypto / auth / i18n / middleware / constant …）
```

**依赖方向铁律**：`gateway → application → controller → domain → repo`，**只能单向向下**。
- ❌ domain 依赖 application/controller = 反向依赖。
- ❌ controller 横向 import 另一个业务域的 controller。
- ❌ application 层 import controller 层的 helper（如 `set_yyy.go` 里的 client 构造器）= 反向依赖（对照 code-patterns#7）。
- ✅ 跨服务调用走 gRPC + proto stub（`<业务主仓库>` → `<业务主仓库>` 经 `<ORG>/<子组>/proto`）。

**多租户（仅 `<业务主仓库>`）**：仓储按租户 `OCode` 路由数据库；评审任何 repo/SQL 改动，确认 OCode 路由没被绕过、没跨租户串数据。

---

## 2. 评审 Checklist（后台专业维度）

### 2.1 分层与落点
- [ ] 新增校验/逻辑是否放在了**正确的层**？（校验设备类型等应下沉到 domain 已有 hook，而非在 application 层硬编码 `if name=='xxx'`——对照 code-patterns#2）
- [ ] 是否复用了 domain 层已有的 `check*`/`validate*`/`is*` 方法作为天然落点，而不是新造？
- [ ] 新增方法命名/可见性是否与**同族方法**一致（`isO1Device` 私有 → 不该新增 `IsExampleDevice` 导出；对照 code-patterns#3）？

### 2.2 接口与契约（gRPC / proto）
- [ ] 改 proto 的字段号是否**只加不改**？老字段语义/类型未变（对照 contract.md）？
- [ ] gRPC 接口的错误码：只新增不复用、分段位（如 403xx 鉴权 / 409xx 冲突），跨服务对齐同一码值？
- [ ] 接口幂等性：涉及物理副作用（扣款、开门、删除、密钥变更）必须 `RequestId/InstructionNo + 单指令锁`，MQTT+轮询双通道尤其要防重复触发（对照 code-patterns#9）。
- [ ] stub 由 proto 仓流水线生成回提，业务仓 `go get -u` 更新——spec 是否交代了 proto 先行？

### 2.3 数据 / 存储
- [ ] SQL：参数绑定（禁拼接，@security_rules SQLi）；新增字段有默认值、老数据兼容；索引是否覆盖新查询条件。
- [ ] gorm 事务边界清晰；避免 N+1（循环里查库 → 批量/预加载）。
- [ ] Redis：key 有过期、有前缀命名空间；分布式锁有超时与释放；缓存击穿/雪崩防护。
- [ ] 结构体字段核对：spec 声称的 `NewXxxParams` 字段列表要与真实 struct **逐字段对齐**，别漏 `SafeConfig`/`RateLimit*`/`ExtraConfig` 等语义相关字段（对照 code-patterns#12）。

### 2.4 连接 / 性能
- [ ] gRPC/HTTP client：是否连接池/长连接单例，而非每次 `grpc.Dial + defer Close`（高频调用反模式，对照 code-patterns#6）？官方推荐 `grpc.NewClient`。
- [ ] 超时/重试/熔断：跨服务调用有 `context` 超时；重试不放大写副作用。
- [ ] 批量接口有分页/游标（如设备特征下发 ≤50/游标 7 天）；大数据量有降级。

### 2.5 消息 / 异步（Kafka / pubsub / MQTT）
- [ ] MQ 消费幂等（重复投递不产生双份副作用）；消费失败有重试/死信/告警。
- [ ] "全量默认改造"（如"所有指令都走 MQTT 推送"）影响面是否被评估？下游各 application/设备 handler 是否都能承接？先灰度再全量（对照 code-patterns#5）。
- [ ] best-effort 兜底路径的真实耗时：强时效指令 `ExpireDuration` 不该复用通用 86400s；`过期 + 兜底扫描间隔` 要满足 spec 承诺的 SLA（对照 code-patterns#9）。

### 2.6 安全（@security_rules，后台重点）
- [ ] 鉴权/权限：**高危不可撤销操作（远程开门/扣款/factoryReset）必须独立细粒度 permission**，不复用 `syncNtp`/`restartDevice` 等低危权限（对照 code-patterns#10）；webgateway 侧 `CheckPermissions` 拦截器覆盖到新接口。
- [ ] 越权：多租户/多商户查询有 ownership 校验（OCode / merchantId 过滤），防横向越权。
- [ ] 密钥：国密 SM2/SM4、KMS 信封加密、RSA 私钥**只从 env/KMS 取，不落盘不进日志**；敏感字段日志脱敏。
- [ ] SSRF：对外/内网请求域名需确认；反序列化用安全方式；错误信息不泄露内部结构。

### 2.7 可观测 / 可测
- [ ] 关键路径有 OTel span / 结构化日志（含 traceId），错误不被吞（`err != nil` 有处理或上抛，不 `_ = err`）。
- [ ] 纯函数/关键分支有单测（参考 `rules/30-testing-rules.md`）；并发竞态路径有用例。

---

## 3. 后台高频坑（Anti-patterns 速查）

| 坑 | 症状 | 对策 |
|----|------|------|
| 反向分层依赖 | application import controller 的 helper | 抽到 infrastructure / 下沉 domain（code-patterns#7） |
| 硬编码落错层 | application 层 `if instructionName=='xxx'` | 用 domain 已有 hook（code-patterns#2） |
| 每次新建连接 | `grpc.Dial + defer Close` 高频调用 | 长连接单例/连接池（code-patterns#6） |
| DTO 重造 | `ToLinkDTO` 与 `ToDeviceDTO` 字段 100% 相同却并列 | 复用/合并（code-patterns#8） |
| 强时效复用长过期 | 开门指令 `ExpireDuration=86400` | 单独短过期 + 同步返回 + 幂等锁（code-patterns#9） |
| 高危复用低危权限 | 远程开门用通用 permission | 独立 permission + 安全评审（code-patterns#10） |
| 全量默认改造低估影响 | "顺手对所有 X 启用 Y" | 灰度/白名单，评估下游承接（code-patterns#5） |

---

## 4. 对外/跨端联调契约（后台视角）

- 面向**设备端**的接口（`devicegateway`/`wecarddevicegateway`/`<设备网关>`）变更 → 强制走 SKILL.md Step 4.3 设备端兼容性 + [`device.md`](./device.md) + [`contract.md`](./contract.md)。
- 面向**前端**的 webgateway 接口 → 字段增删、错误码、分页契约要与 [`frontend.md`](./frontend.md) 对齐；返回给前端的枚举/文案考虑 i18n。
- 面向**移动端 SDK**的 appgateway/opengateway 接口 → 签名/token 契约、字段兼容看 [`mobile-sdk.md`](./mobile-sdk.md)。

---

## 5. 本项目真实代码约定与专项检查（带代码依据）

> 以下是从本项目真实代码提炼的、**只有做过这套代码才知道**的专业检查项。评审时逐条对照 spec/plan/代码；括号内为可 grep 的真实文件/符号。

### 5.1 依赖注入与启动装配（`infrastructure/dig` + `common/bootstrap`）
框架是 uber `dig`（`infrastructure/dig/dig.go`），靠**匿名 import 触发 `init()` 副作用注册**。
- [ ] 新增 repo/domain 的 `RegisterInitFunc`/`Provide` **必须写在包 `init()` 里**，且承载包**被 main/server 匿名 import**（如 `server.go` 的 `_ "…/repo/examplestandard"`）——漏 import → 工厂为 nil、启动即空指针。
- [ ] 🔴 **Gateway 默认不启用 DI**，依赖 DI 工厂的 gateway 必须显式 `WithGwDI()`（`common/bootstrap/runner.go`）；App 默认启用（`WithSkipDI` 才跳过）。
- [ ] `RegisterInitFunc` 依赖的 `*XxxConfiguration` 必须有对应 `Provide`，否则 `MustInvokeInitFuncs` 启动报错。
- [ ] 装配顺序 `SetConfig → InstallOpentelemetry → InvokeInitFuncs(DI) → AfterDI(pool.Init/subscriber.Register) → 拦截器`——新增需在 DI 后初始化的资源要挂 AfterDI，别在 init 里就用还没就绪的工厂。

### 5.2 DDD 充血模型与依赖反转（真实写法）
- [ ] 领域实体内嵌 `*poHolder[XxxPo]`（`domain/domain.go` 泛型），getter/setter 读写 `po.XXX`；**状态变更要维护 Version 自增**（参考 `domain/common/organization/organization.go` 的 `SetIdleImageURL`/`mergePageStyleConfig` 用 `reflect.DeepEqual` 判变再升版本）。
- [ ] 仓储走 **provider 反转**：domain 定义 `SetXxxRepoProvider` + `NewXxxRepository`，repo 在 `init()` 里 `SetXxxProvider`（`repo/examplestandard/*_repo.go`）——**domain 绝不能直接 import repo**（反向依赖）。
- [ ] 构造遵循三件套：`NewXxx`（新建）/`RebuildXxx`（repo load 重建）/`NewXxxRepository`（拿仓储）。

### 5.3 错误码 errcodes（`infrastructure/errcodes/errcodes.go` 头部注释即硬规范）
- [ ] 命名 `Errcode[模块][描述]` PascalCase；每模块独立 const 块、分配 10 万段位；返回错误**必须 `status` 库封装**。
- [ ] 🔴 **错误码只能追加，不能删除或调整定义顺序**——码值靠 `iota` 递推（如 `iota + 90000`），在中间插入/删除会让**后续所有码值整体漂移**，跨服务/跨端全部错位。改错误码文件时重点看有没有动到已有条目顺序。
- [ ] 新增有特定 HTTP 语义的错误码 → 要在 `HttpStatusCode()` 补 `case`（否则前端拿到默认映射）。
- [ ] 错误码文件不暴露给前端；网关接口在**声明处显式列出**返回码。

### 5.4 多租户 OCode（`common/middleware/ocode_middleware.go`）
- [ ] 🔴 **`GetOCodeFromContext` 解析不到会静默返回 `DefaultOCode="2"`**——租户隔离依赖 OCode，静默降级到默认租户可能**串数据/越权**。评审：新接口是否正确挂 OCode 中间件（`HTTPOCodeMiddleware`/`GRPCOCodeMiddleware`）？关键写操作**不允许**在无 OCode 时落默认租户。
- [ ] 跨服务调用的租户上下文靠 `ContextValuesClientInterceptor` 透传 OCode/UserId/DeviceSN——新的 gRPC client 必须带这个 client 拦截器（用 `option.ClientDialOptions()`），否则下游拿不到租户上下文。
- [ ] 仓储按 OCode 路由 DB，改 repo 不得绕过路由直连固定库。

### 5.5 gRPC 跨服务调用（现网有反模式活样本）
- [ ] 🟠 现网大量 `grpc.NewClient(addr,…) + defer conn.Close()` **写在每次业务调用里**（如 `controller/merchant/findmerchantdevice/find_device.go` 每次 `BatchGetDeviceOnlineStatus` 都新建连接）——高频路径应复用连接/连接池（对照 code-patterns#6）。评审新增跨服务调用别照抄这个反模式。
- [ ] `insecure.NewCredentials()`（明文无 TLS）在内网可接受，但**跨网/传敏感数据要确认是否该 mTLS**。
- [ ] 统一用 `option.ClientDialOptions()` 构造 dial 选项（含拦截器/超时），别裸 `grpc.Dial` 漏掉上下文透传与超时。

### 5.6 核心领域：审批 / 属性变更 / 指令 / 设备影子（`<设备管理仓库>`、`<业务主仓库>`）
- [ ] 高危/敏感变更是否接**审批流**（`domain/approval_ticket.go`、`T_Approval_Ticket`）？新增高危操作别绕开审批。
- [ ] 属性变更（PropertyChange）有**发布/上下线/回滚/灰度**全生命周期——改 PropertyChange 必须覆盖灰度与回滚路径。
- [ ] 指令服务启动初始化默认配置（`InitDefaultInstructionConfig`）+ 有**超时置位巡检脚本**（`script/`）——新增指令要进默认配置，且强时效指令的超时兜底要与 SLA 对齐（对照 code-patterns#9）。
- [ ] 设备影子（DeviceShadow）期望态 vs 上报态：改动要保证两态收敛逻辑与一致性校验不被破坏。

---

## 🖊 人工追加评审注意点（Human-Curated Review Points）

> 团队后台专家在此追加专业注意点，skill 评审后台端时**必须逐条加载检查**。格式见 [`README.md`](./README.md) 第 3 节。

<!-- HUMAN-CURATED:START -->
<!-- 在本行下方按格式追加条目，勿删除首尾两个标记 -->

<!-- HUMAN-CURATED:END -->
