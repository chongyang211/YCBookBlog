# Tasks: 短链接核心服务（1001-short-url-core）

> 📌 **这是教学案例**（Story 1001「短链接服务」全套样例之一，导读见 `read/07-case.md`）。
> 真实 tasks 请复制 `templates/tasks-template.md` 起草。本文件刻意展示：任务粒度 15-60 分钟、实时勾选终态、偏离记录闭环（含 spec 修订回流）。

> **Story ID**: 1001
> **Related Plan**: plans/v1.0.0/1001-short-url-core-plan.md（approved）
> **Related Spec**: specs/v1.0.0/1001-short-url-core.md（ready）
> **Status**: done
> **Executor**: 陈晨（AI 结对）
> **Created**: 2026-08-09
> **Branch**: `feature/1001-short-url-core`

## Phase 1: 数据层与短码引擎

- [x] 1.1 编写 migration：建 `short_url` 表（short_code PK + idx_target 索引），本地 up/down 验证
- [x] 1.2 实现 `model.go`：ShortURL 实体、Upsert（冲突返回已有行）、GetByCode、GetByTarget
- [x] 1.3 实现 `code.go`：MurmurHash64 → Base62 截 7 位；冲突换 salt 重试 ≤3 次
- [x] 1.4 `code_test.go`：碰撞重试路径、盐耗尽路径、字符集合法性（62 字符）单测
- [x] 1.5 `model_test.go`：幂等 Upsert 并发竞态（同 target 并发，返回同一行）单测

## Phase 2: 业务服务与缓存

- [x] 2.1 实现 `service.go` Create：白名单校验 → 幂等预检 → 生成短码 → Upsert
- [x] 2.2 实现 `service.go` Resolve：查缓存 → 未命中负缓存 60s → DB 回填 → 过期判定 → 302 目标
- [x] 2.3 实现 `cache.go`：Redis 封装，TTL=min(剩余有效期, 24h)；Redis 挂载降级直查 DB
- [x] 2.4 实现 `event.go`：goroutine 异步投递点击事件至 `shorturl:click:events`，失败仅记日志
- [x] 2.5 `service_test.go`：FR-1/FR-3/FR-5 分支全覆盖（含 expire_at 前后 1 秒边界、事件失败不阻塞）
- [x] 2.6 【偏离 D-1】实现 `internal/shorturl/config.go`：白名单与超时配置加载（plan 清单外，见偏离记录）

## Phase 3: 接入与配置

- [x] 3.1 实现 `handler.go`：POST /api/shorturl（参数校验+错误码映射）与 GET 跳转入口
- [x] 3.2 `main.go` 装配路由与依赖注入（shorturl 子模块挂载）
- [x] 3.3 `routes.yaml` 新增 `s.` 域名路由；预发环境验证 302 生效
- [x] 3.4 `limits.yaml` 创建接口挂 RateLimit（60/min, key=creator_id）→ curl 验证 429
- [x] 3.5 `configs/prod.yaml` 新增白名单域名列表与 Redis/DB 连接段
- [x] 3.6 `handler_test.go`：AC-1~AC-6 接口层测试（httptest）

## Phase 4: 验收与压测

- [x] 4.1 部署预发；编写 `tests/perf/shorturl_bench.sh`（5000 QPS 跳转、预热缓存至命中率 95%）
- [x] 4.2 压测执行：P99 = 41ms（AC-7 ✅，余量 9ms）
- [x] 4.3 压测复验 AC-8：投递关闭 vs 开启，P99 波动 1.3ms（✅ <2ms）
- [x] 4.4 覆盖率：语句 86%（AC-9 ✅）
- [x] 4.5 `/spec-review` 产出评审报告 + 变更摘要；MR 描述引用 spec/plan/tasks 三件套
- [x] 4.6 【偏离 D-2】FR-4 频控阈值 60→100，spec 修订已回流（见偏离记录与 spec 修订记录）

## 偏离记录

> 实施过程中与 spec/plan 不符的，在此追加。**沉默偏离视为缺陷**。

| 日期 | 偏离点 | 原因 | 建议 spec 修订 |
|------|--------|------|----------------|
| 2026-08-09 | D-1: 新增 `internal/shorturl/config.go`（plan §2 清单外） | Phase 2 实施时发现白名单与三类超时值散落硬编码不利于预发/生产切换，抽出配置加载 | 无需修 spec；plan 补录到「改动范围」（已补） |
| 2026-08-11 | D-2: FR-4 频控阈值 60/min → 100/min | 4.1 压测预热阶段批量创建（营销批量换链工具，合法流量）持续触发 429；运营确认其峰值 80/min | **已回流 spec**：FR-4 与 AC-6 同步改为 100（spec 修订记录 2026-08-11 行） |

## 完成校验清单

- [x] 所有任务勾选完成（Phase 1-4 共 22 项）
- [x] spec 验收标准 AC-1 ~ AC-9 全部通过并在 spec 中勾选
- [x] 偏离记录 2 条已闭环（1 条补录 plan，1 条回流 spec）
- [x] 变更摘要已写入 MR 描述（`.gitlab/merge_request_templates/Default.md` 三段式）
- [x] MR 描述包含：关联 Spec / 偏离说明 / 变更摘要
- [x] commit 均带 `--story=1001`，最后一笔带 `#finish`
- [x] /spec-sync 已执行：spec Status → implemented（2026-08-12）
