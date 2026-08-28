# 移动端 · 接入 SDK 评审依据（Mobile & Integration SDK Review Guide）

> 适用：被商户 App / 收银端集成的刷掌 SDK 与移动应用。
> - `PaymaxPalmSdk`：`android`（Kotlin）、`ios`（Swift）、`web`（JS/TS SDK）—— 对外发布的接入 SDK。
> - `palm-wepay/Android`：刷掌收银 Android App（Java）。
> 加载时机：MR 命中上述目录时，Step 4.1 / 4.5 / 4.6 前读完本文件。（其 `web` 部分同时参照 [`frontend.md`](./frontend.md)。）

---

## 1. 评审基准（SDK 的特殊性）

SDK 是**被第三方集成**的，评审第一性原理：

1. **API 向后兼容 > 一切** —— 已发布的公开 API 一旦变更，所有接入方都要改代码。删除/改签名 = 🔴 破坏性。
2. **接入体验** —— 初始化简单、错误可诊断、文档/示例齐全。
3. **体积与依赖** —— SDK 引入的传递依赖会污染宿主 App，冲突/膨胀是硬伤。
4. **多端一致** —— Android/iOS/Web 三端的 API 语义、错误码、回调时序应一致。

---

## 2. 评审 Checklist（移动端/SDK 专业维度）

### 2.1 公开 API 兼容性（🔴 硬性）
- [ ] 公开 class/方法/回调**只增不删不改签名**；确需废弃走 `@Deprecated`/`@available` 标注 + 过渡期，不直接删。
- [ ] 回调/事件时序、参数语义不变；错误码只加不改义。
- [ ] 版本号遵循 semver：破坏性变更 → major；接入方能据版本判断影响。
- [ ] 三端（Android/iOS/Web）同一能力的 API 命名、参数、错误码语义**保持一致**。

### 2.2 依赖与体积
- [ ] 新增第三方依赖是否必要？是否可能与宿主 App 冲突（重复库/版本冲突）？
- [ ] 传递依赖最小化；so/framework 体积增量可接受。
- [ ] 混淆规则（`*.pro`）随 SDK 提供，避免接入方混淆后崩溃。

### 2.3 安全（移动端重点）
- [ ] **密钥/签名**：AppKey/密钥不硬编码进 SDK 或宿主；敏感数据加密传输（mTLS/RSA-OAEP/AES）。
- [ ] **掌图/生物特征**：采集、加密、上传、内存清理链路安全；不落盘明文、不进日志。
- [ ] 鉴权：SDK 与后台 appgateway/opengateway 的签名/token 契约（HMAC、AeonKey、SDK 签名验签）——对照 [`backend.md`](./backend.md) 2.4 与 [`contract.md`](./contract.md)。
- [ ] 防逆向/防篡改的基本措施（关键校验不放纯客户端）；日志级别在 release 下收敛。

### 2.4 稳定性与体验
- [ ] **崩溃隔离**：SDK 内部异常不能崩溃宿主 App（try/catch 边界、空安全）。
- [ ] 线程：耗时操作（采集、网络、加解密）不占主线程；回调线程明确（主线程/工作线程）文档说明。
- [ ] 生命周期：Activity/ViewController 销毁后回调不空引用（内存泄漏/野指针）。
- [ ] 权限：相机等运行时权限的申请与拒绝分支都有处理。
- [ ] 弱网/超时：网络失败有明确错误码与重试策略，不无限等待。

### 2.5 平台特性
- [ ] Android：min/target SDK 兼容；AndroidX；`AndroidManifest` 权限最小化；R8/混淆通过。
- [ ] iOS：Swift API 命名符合规范；隐私清单（PrivacyInfo）声明相机/数据用途；bitcode/arch 兼容。
- [ ] Web SDK：见 [`frontend.md`](./frontend.md)（TS 类型、打包、XSS）；同时保证 npm 包的 API 兼容。

### 2.6 文档与示例
- [ ] 公开 API 变更同步更新 `docs/`、`README`、`CHANGELOG`；破坏性变更有迁移指引。
- [ ] demo/示例能跑通新 API；接入步骤准确。

---

## 3. 移动端/SDK 高频坑

| 坑 | 症状 | 后果 | 对策 |
|----|------|------|------|
| 破坏公开 API | 删/改公开方法签名 | 接入方编译失败 | 只增不删 + Deprecated + semver major |
| 三端不一致 | Android 有的能力 iOS 语义不同 | 接入方困惑、bug | 三端 API 语义对齐 |
| 依赖冲突 | 引入库与宿主重复/冲突 | 宿主崩溃 | 最小依赖 + 冲突排查 |
| SDK 崩溃殃及宿主 | 内部异常未捕获 | 宿主 App crash | 边界 try/catch + 空安全 |
| 密钥硬编码 | AppKey 写死在代码 | 密钥泄露 | 由接入方配置 + 加密 |
| 掌图裸奔 | 生物特征明文/进日志 | 合规/隐私事故 | 加密 + 内存清理 + 脱敏 |

---

## 4. 跨端联调契约（移动端视角）

- 与**后台**：appgateway/opengateway 的签名、token、字段、错误码契约，见 [`backend.md`](./backend.md) 与 [`contract.md`](./contract.md)。
- 与**设备端**：若 SDK 与设备协同（如收银端联动刷掌设备），协议对齐见 [`device.md`](./device.md)。

---

## 🖊 人工追加评审注意点（Human-Curated Review Points）

> 团队移动端/SDK 专家在此追加专业注意点，skill 评审移动端时**必须逐条加载检查**。格式见 [`README.md`](./README.md) 第 3 节。

<!-- HUMAN-CURATED:START -->
<!-- 在本行下方按格式追加条目，勿删除首尾两个标记 -->

<!-- HUMAN-CURATED:END -->
