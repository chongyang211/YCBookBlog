# 短链接核心服务：生成、跳转与过期控制

> 📌 **这是教学案例**（Story 1001「短链接服务」全套样例之一，导读见 `read/07-case.md`）。
> 真实 spec 请复制 `templates/spec-template.md` 起草。本文件刻意展示：来源标注（📥🤖🔍❓）、Sibling Specs、可勾选验收标准。

> **Story ID**: 1001
> **Status**: ready
> **Author**: 陈晨（AI 起草，人审修订）
> **Created**: 2026-08-07
> **Updated**: 2026-08-08（评审意见回填）
> **Sibling Specs**: specs/v1.0.0/1001-short-url-stats.md
> **Branch**: `feature/1001-short-url-core`
> **Related Design**: designs/v1.0.0/1001-short-url-service-design.md（approved）

> 📌 章节来源标记：📥 原始需求（intake）/ 🤖 AI 推断（待人确认）/ 🔍 现有代码 / ❓ TBD

## 背景

📥 平台文章 URL 普遍 80+ 字符，分享到社交平台被截断，用户无法点击（见
`intake/v1.0.0/1001-short-url-service.md`）。技术方案已评审通过
（`designs/v1.0.0/1001-short-url-service-design.md`）：新建 `shorturl-service`，
采用 MurmurHash + Base62 短码，302 跳转，惰性过期判定。

本 spec 是 Story 1001 的**核心子 spec**；点击统计部分由兄弟 spec
`1001-short-url-stats` 承接。

## 目标

- 用户/编辑器可把本平台 URL 转换为 `s.example-content.com/{code}` 形式的短链
- 访问短链时 **302 跳转**到目标 URL，跳转接口 P99 < 50ms
- 短链到期后不报错，跳转至统一的"链接已过期"说明页
- 同一目标 URL 重复创建时返回**已有短码**（幂等）

## 非目标

- ❌ 点击统计与报表（兄弟 spec `1001-short-url-stats`）🤖 来自 design 拆分
- ❌ 自定义短码后缀（产品明确下期）📥
- ❌ 移动端分享 SDK 接入（下期）📥
- ❌ 运营后台管理界面（本期仅 API）

## 用户故事

| 作为 | 我希望 | 以便 |
|------|--------|------|
| 内容编辑 | 复制文章链接时自动得到短链 | 分享到各平台不被截断 |
| 读者 | 点击短链快速到达原文 | 不需要处理截断的残缺链接 |
| 运营 | 活动短链过期后自动跳说明页 | 用户不会看到报错页损害体验 |
| 平台安全 | 短链只能指向白名单域名 | 短链能力不被用于钓鱼外链 |

## 功能需求

### FR-1: 创建短链

`POST /api/shorturl`，body 含 `target_url`（必填）、`expire_days`（选填，默认 90）。

- target_url 必须命中**本平台域名白名单**，否则拒绝（错误码 `E_DOMAIN`）📥 design D1 白名单决策
- 同一 creator 对同一 target_url 幂等：已存在且未过期 → 直接返回已有 short_code
- 返回体：`{ "short_code": "aB3xK9q", "short_url": "https://s.example-content.com/aB3xK9q", "expire_at": "..." }`

### FR-2: 短码生成

🤖 AI 推断（来自 design §4/D1，已在评审确认）：MurmurHash64(target_url + salt) →
Base62 截取 7 位；冲突（主键已存在且 target 不同）时换 salt 重试，最多 3 次后报
`E_CODE_EXHAUSTED`。

### FR-3: 短链跳转

`GET s.example-content.com/{code}`：

- 命中且未过期 → `302 Location: target_url`，同时投递一次点击事件（异步，见 FR-5）
- 命中但已过期 → `302` 至 `https://www.example-content.com/link-expired` 📥 运营原话"别给用户报错"
- 未命中 → 同样 302 至说明页（**不返回 404**，防枚举探测）🤖 AI 推断，评审已确认
- 缓存：code→{target, expire_at} 存 Redis，TTL = min(剩余有效期, 24h)；未命中结果缓存 60s 防穿透 🤖 来自 design 风险项

### FR-4: 创建频控

🔍 现有 `middleware.RateLimit` 中间件（`src/backend/internal/middleware/ratelimit.go`）
按 creator_id 限流：60 次/分钟，超限返回 `E_RATE_LIMIT`（HTTP 429）。

### FR-5: 点击事件投递

跳转成功（过期跳说明页不计，仅正常跳转计）时，异步追加点击事件到
Redis List `shorturl:click:events`，事件结构与消费方式由兄弟 spec
`1001-short-url-stats` 定义；本 spec 只保证**投递不阻塞跳转**（投递失败仅记日志）。

## 非功能需求

- **性能**：跳转接口 P99 < 50ms（Redis 命中路径）；创建接口 P99 < 200ms 🤖 design §2 推算
- **可用性**：shorturl-service 与 Redis 任一可用即跳转可用（缓存穿透降级直查 DB）
- **安全**：白名单校验在服务端强制执行；短码不可枚举（见 FR-2）
- **容量**：短码空间 62^7 ≈ 3.5 万亿；单表预估 3 年 < 5000 万行，现有 MySQL 体系承载

## 数据结构 / API / 接口影响

```sql
-- 新增表（migration 由 plan Phase 1 承接）
CREATE TABLE short_url (
  short_code CHAR(7) PRIMARY KEY,
  target_url VARCHAR(2048) NOT NULL,
  creator_id BIGINT NOT NULL,
  expire_at DATETIME NOT NULL,
  created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  KEY idx_target (target_url(255), creator_id)   -- 幂等查询
) ENGINE=InnoDB;
```

新增 API：见 FR-1 / FR-3。网关新增 `s.` 域名路由（配置变更，见 plan）。

## 状态流转

```
[active] ── 到达 expire_at（惰性判定）──► [expired] ── 每日清理任务(保留90天) ──► [purged]
```

- 过期判定发生在**跳转时**（design D5：无需精确到秒）
- purged 后物理删除，行数不膨胀 🤖 AI 推断，评审确认

## 边界情况

| 场景 | 预期行为 |
|------|----------|
| target_url 非本平台域名 | 创建拒绝，E_DOMAIN |
| target_url 是合法但深层参数链接（> 2048 字符） | 创建拒绝，E_URL_TOO_LONG |
| expire_days 传 0 或负数 | 拒绝；合法区间 [1, 365] |
| 同一 target_url 被不同 creator 创建 | 各自独立短码（幂等仅按 creator 维度） |
| 短码 3 次冲突 | 返回 E_CODE_EXHAUSTED（理论概率 ~0，仅防御） |
| Redis 全挂 | 创建接口降级拒绝（写库必须幂等依赖缓存预检）；跳转直查 DB，性能降级但可用 |
| 并发创建同一 target_url（同 creator） | 数据库唯一索引兜底，后到者返回先到者短码 |

## 验收标准

- [ ] AC-1: 对白名单域名 URL 调用创建接口，返回 7 位 Base62 短码与完整 short_url
- [ ] AC-2: 对非白名单域名调用创建接口，返回 E_DOMAIN 且不产生数据
- [ ] AC-3: 浏览器访问有效短链，收到 302 且 Location 等于 target_url
- [ ] AC-4: 访问已过期短链，302 跳转至 link-expired 页面（非 404、非 5xx）
- [ ] AC-5: 同一 creator 重复创建同一 target_url，返回相同 short_code（幂等）
- [ ] AC-6: 1 分钟内第 61 次创建请求返回 429 E_RATE_LIMIT
- [ ] AC-7: 压测（5000 QPS 跳转、缓存命中率 95%）下 P99 < 50ms
- [ ] AC-8: 跳转路径上点击事件异步投递，投递失败不影响跳转耗时（P99 波动 < 2ms）
- [ ] AC-9: 单元测试覆盖 FR-1~FR-5 全部分支，语句覆盖率 ≥ 80%

## 测试点

- **正常路径**：创建→跳转→事件投递全链路；幂等创建
- **边界条件**：过期边界（expire_at 前后 1 秒）；URL 长度边界；expire_days 边界（1/365/366）
- **错误处理**：非白名单、超长 URL、频控触发、Redis 挂载降级
- **并发**：同 target 并发创建；跳转与过期清理并发
- **性能**：AC-7 压测场景

## 风险与未决问题

| 项目 | 描述 | 状态 |
|------|------|------|
| 缓存穿透 | 大量不存在 code 攻击 → 未命中结果缓存 60s 承接 | resolved（FR-3） |
| 白名单维护流程 | 运营如何申请新域名加入白名单（人工配置 or 工单） | open（stats spec 评审时定，不影响本 spec 开发） |
| 点击事件结构 | 与 stats 子 spec 的接口契约需冻结 | open（两 spec 作者已约 2026-08-09 对齐） |

## 实施备注

- 🔍 网关路由配置参考现有 `s-cdn` 子域接入方式（`deploy/gateway/routes.yaml`）
- 🔍 频控中间件直接复用 `middleware.RateLimit`，参考 `share-service` 的接入写法
- 点击事件 List 的消费组命名与 stats 子 spec 约定：`shorturl-stats-consumer`
- 实施顺序建议：先 FR-2/FR-1（生成+创建），再 FR-3（跳转+缓存），再 FR-4/FR-5

## 修订记录

| 日期 | 修订点 | 关联 tasks 偏离记录 | 修订人 |
|------|--------|---------------------|--------|
| 2026-08-07 | 初始版本（AI 起草） | — | 陈晨/AI |
| 2026-08-08 | 评审：未命中也 302 说明页（防枚举）；AC-7 从 100ms 收紧到 50ms | — | 张架构师 |
| 2026-08-08 | 评审：补 FR-5 投递失败不影响跳转的验收（AC-8） | — | 赵 TL |
