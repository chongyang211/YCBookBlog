# 识别 · 算法端 评审依据（Recognition & Algorithm Review Guide）

> 适用：业务识别 Pipeline 与算法服务（`algorithm-repo` = `<ORG>/<算法组>/algorithm-repo`；识别侧 `<业务主仓库>`/`proto` 逻辑已合并进 `<ORG>/<子组>` 的 `src/<业务主仓库>`/`src/proto`）。
> 技术栈：Go / gRPC 算法服务集 / 识别编排（`private_domain/recognition`）/ 特征库检索。
> 加载时机：MR 命中 `algorithm-repo` 或识别侧 `<业务主仓库>`/`proto` 逻辑时，Step 4.1 / 4.5 / 4.6 前读完本文件。本端是特殊后台端，通用后台项仍参照 [`backend.md`](./backend.md)。

---

## 1. 架构与调用链约定（评审基准）

**识别主链**（`private_domain/recognition`）：
```
Recognize
 └─ NewCapture(device, capturedImage)   [三路并发]
     ├─ CheckLiveness → livenessCheck       活体（RGB+IR 融合）
     ├─ CheckQuality  → qualityCheck         质量分级
     └─ extractFeatures → featureExtract       RGB+IR 提特征 + 掌方向
 └─ capture.Recognize
     ├─ retrieveInGroup(线下全量库) → retrieval.Retrieve（topN）
     │   └─ 多因子(RGB+IR)配对、同 userId 归并、按误识率 Far 排序
     │   └─ 阈值：过 Threshold→Trust，过 VerifyThreshold→Verify
     └─ 全量库空且 CanActivate → 线上库（单因子 RGB）
 └─ 结果流转：Confirmed / RecognizeFailed / Verifying(缓存 recognitionId)
```

**核心算法（主链路实际调用 6 个）**：`livenessCheck`、`onlineLiveness`、`qualityCheck`、`featureExtract`、`retrieval`、`alignment`。
其余（`shallowFeature`/`watermark`/`actionLive`/`heartbeatLive`/`attribute`/`evaluation`）**不在主识别链路**——spec 若声称调用它们，先核对是否真被 recognition 编排调用。

**两套并行 domain**：`privaterecognition` 用 `private_domain/recognition`；`<识别服务>` 用 `<业务主仓库>/domain/recognition`，编排高度一致但**不是同一份代码**——评审时别张冠李戴。

---

## 2. 评审 Checklist（算法/识别专业维度）

### 2.1 识别准确性与安全（核心，物理副作用）
- [ ] **误识率（FAR）/ 拒识率（FRR）**：改动阈值（`Threshold`/`VerifyThreshold`）时，是否说明对 FAR/FRR 的影响？业务=支付级身份认证，**降阈值提通过率必须评估误识风险**。
- [ ] **活体/防伪**：绕过或弱化 `CheckLiveness`/`CheckOnlineLiveness` 的改动 → 🔴，必须安全评审（防照片/假体攻击）。
- [ ] **多因子融合**：RGB+IR 配对、同 userId 归并逻辑改动 → 是否破坏多因子判定强度？在线单因子（RGB）路径的适用边界是否清晰？
- [ ] **线上库→线下库 Activate 迁移**：`Activate` 用设备图替换线上图并重建特征分组，涉及用户身份，改动要核对状态机是否有竞态/重复激活。

### 2.2 特征与数据
- [ ] **特征版本兼容**：RGB/IR 算法版本升级 → 老特征库能否检索？是否需要特征迁移/重算（`evaluation` 迁移工具）？灰度期新老特征共存策略？
- [ ] **特征下发（离线识别）**：`SyncPalmFeature` 批量下发（≤50/游标 7 天有效）——批量大小、游标有效期、解绑删除下发是否覆盖？
- [ ] **图像/特征数据安全**：掌图/特征是生物特征敏感数据——加解密（AES256-RSA2048 示例业务解密）、存储、日志脱敏、留存合规（`watermark`）是否到位？

### 2.3 算法服务调用
- [ ] 算法 gRPC 调用：连接复用（非每帧新建连接）、超时（活体/提特征有实时性要求）、并发（三路并发是否有共享状态竞态）。
- [ ] 识别上下文缓存（Redis，`recognition_context.go`）：`recognitionId` 的 TTL 与业务加验时间窗匹配？多候选/Verifying 态的清理？
- [ ] 算法失败降级：活体不过/质量不过/检索超时的错误码与用户提示是否明确（`ErrcodePalmLogic*`）？不静默吞错。

### 2.4 编排一致性
- [ ] 若同时改 `privaterecognition` 与 `<识别服务>` 两套编排 → 逻辑是否需同步？只改一套是否留下行为不一致？
- [ ] 1:N（`Retrieve`）vs 1:1（`Compare`）用法是否用对场景（验掌用 1:1，识别主链 1:N）。

---

## 3. 识别/算法高频坑

| 坑 | 症状 | 对策 |
|----|------|------|
| 只调准确率不评误识 | 调阈值提通过率，不提 FAR | 必附 FAR/FRR 影响评估 + 安全签字 |
| 张冠李戴两套 domain | 改 `private_domain` 却引用 `<业务主仓库>/domain` | 确认目标编排；两套需同步则都改 |
| 声称调用旁路算法 | spec 说用 `actionLive`/`attribute` | 核对是否真在主链路（多为 nil/未调用） |
| 特征版本裂开 | 升级算法不管老特征库 | 迁移/重算方案 + 灰度共存 |
| 生物特征裸奔 | 掌图/特征进日志或明文存 | 脱敏 + 加密 + 留存合规 |

---

## 4. 跨端联调契约（识别端视角）

- 识别侧 proto 已合并进 `proto`（module `<ORG>/<子组>/proto`），`privaterecognition` 接口定义也在此仓（`src/proto`）。改识别接口先看 [`contract.md`](./contract.md) 的 proto 现状说明。
- 与**设备端**：离线识别把特征下发到设备本地检索——特征格式/版本要与 [`device.md`](./device.md) 端侧算法对齐。
- 与**后台**：识别结果回流用户状态、激活迁移涉及 `<业务主仓库>`/`<业务主仓库>` 业务态，见 [`backend.md`](./backend.md)。

---

## 🖊 人工追加评审注意点（Human-Curated Review Points）

> 团队算法/识别专家在此追加专业注意点，skill 评审识别端时**必须逐条加载检查**。格式见 [`README.md`](./README.md) 第 3 节。

<!-- HUMAN-CURATED:START -->
<!-- 在本行下方按格式追加条目，勿删除首尾两个标记 -->

<!-- HUMAN-CURATED:END -->
