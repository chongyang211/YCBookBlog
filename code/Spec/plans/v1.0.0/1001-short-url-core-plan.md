# Plan: 短链接核心服务（1001-short-url-core）

> 📌 **这是教学案例**（Story 1001「短链接服务」全套样例之一，导读见 `docs/07-case.md`）。
> 真实 plan 请复制 `templates/plan-template.md` 起草。本文件刻意展示：改动文件精确到路径（超出即偏离）、复用 design 决策、Phase 按依赖排序。

> **Story ID**: 1001
> **Related Spec**: specs/v1.0.0/1001-short-url-core.md（ready）
> **Status**: approved
> **Author**: 陈晨（AI 起草，人审修订）
> **Created**: 2026-08-09

## 1. 复用的技术决策（来自 Design，不重复讨论）

Design（`designs/v1.0.0/1001-short-url-service-design.md`）已定：MurmurHash64+Base62
（D1）、302 跳转（D2）、MySQL 新表（D3）、Redis 缓存（D4）、惰性过期（D5）、
异步点击采集（D6）。本 plan 只做落地拆解。

## 2. 改动范围

### 新增文件

| 路径（src/backend/ 下） | 用途 | 关联 FR |
|-------------------------|------|---------|
| `internal/shorturl/model.go` | ShortURL 实体与 DAO | FR-1 |
| `internal/shorturl/code.go` | 短码生成（MurmurHash+Base62+重试） | FR-2 |
| `internal/shorturl/service.go` | 创建/跳转/过期判定业务逻辑 | FR-1, FR-3 |
| `internal/shorturl/handler.go` | HTTP handler（创建+跳转两个入口） | FR-1, FR-3 |
| `internal/shorturl/cache.go` | Redis 缓存封装（含未命中负缓存 60s） | FR-3 |
| `internal/shorturl/event.go` | 点击事件异步投递（Redis List） | FR-5 |
| `migrations/20260810_create_short_url.sql` | 建表迁移 | 数据结构 |
| `internal/shorturl/service_test.go` | 单元测试 | AC-9 |
| `internal/shorturl/code_test.go` | 短码生成测试（含碰撞重试） | AC-9 |
| `internal/shorturl/handler_test.go` | 接口层测试 | AC-1~AC-6 |

### 修改文件

| 路径 | 修改要点 | 关联 FR |
|------|----------|---------|
| `cmd/server/main.go` | 注册 shorturl 路由与依赖装配 | FR-1, FR-3 |
| `deploy/gateway/routes.yaml` | 新增 `s.` 域名路由到 shorturl-service | FR-3 |
| `deploy/gateway/limits.yaml` | 创建接口挂 RateLimit（60/min, key=creator_id） | FR-4 |
| `configs/prod.yaml` | 白名单域名列表、Redis/DB 连接配置 | FR-1 |

### 删除文件

无。

## 3. Phase 拆分

### Phase 1: 数据层与短码引擎

**目标**：表可建、短码可生成、DAO 可用。此阶段结束可独立验证（单测通过）。

- 涉及：`migrations/20260810_create_short_url.sql`、`model.go`、`code.go` + 两个测试文件
- 依赖：无
- 验证：`go test ./internal/shorturl/... `通过；本地 migration up/down 各跑一次

### Phase 2: 业务服务与缓存

**目标**：创建（幂等+白名单）、跳转（302+过期+负缓存）、事件投递逻辑完备。

- 涉及：`service.go`、`cache.go`、`event.go`、`service_test.go`
- 依赖：Phase 1
- 验证：单测覆盖 FR-1/FR-3/FR-5 全部分支（含幂等、过期边界、Redis 挂载降级）

### Phase 3: 接入与配置

**目标**：HTTP 层、路由装配、网关配置、频控、配置项全通。

- 涉及：`handler.go`、`handler_test.go`、`main.go`、`routes.yaml`、`limits.yaml`、`prod.yaml`
- 依赖：Phase 2
- 验证：本地起服务，curl 走通 AC-1~AC-6 手动路径

### Phase 4: 验收与压测

**目标**：AC-7/AC-8 压测通过、覆盖率达标、review 材料。

- 涉及：压测脚本（`tests/perf/shorturl_bench.sh`，新增）、覆盖率报告
- 依赖：Phase 3（需部署到预发）
- 验证：AC-7（5000 QPS、命中率 95% 下 P99<50ms）、AC-8、AC-9（覆盖率 ≥80%）

## 4. 验收标准覆盖矩阵

| 验收标准 | 覆盖 Phase |
|----------|-----------|
| AC-1 ~ AC-3 | Phase 2（逻辑）+ Phase 3（接入） |
| AC-4（过期跳说明页） | Phase 2 |
| AC-5（幂等） | Phase 1（唯一索引）+ Phase 2（服务层） |
| AC-6（频控 429） | Phase 3 |
| AC-7（P99<50ms） | Phase 4 |
| AC-8（事件不阻塞跳转） | Phase 2 + Phase 4 复验 |
| AC-9（覆盖率） | Phase 1~3 持续 + Phase 4 汇总 |

## 5. 风险

| 风险 | 缓解 |
|------|------|
| `s.` 子域证书未就绪阻塞 Phase 4 压测 | 已提前申请（design 评审时确认）；阻塞则先用路径前缀 `/s/` 临时压测 |
| 幂等唯一索引与高并发创建的死锁热点 | 目标 QPS 50，风险极低；压测专项验证 |
| 点击事件 List 无消费者时积压 | stats 子 spec 并行开发；压测期间临时消费脚本兜底 |
