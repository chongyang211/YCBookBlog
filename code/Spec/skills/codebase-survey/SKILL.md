---
name: codebase-survey
description: 代码侦察技能。给定一个需求描述或 spec，扫描 src/ 目录，输出与本次需求相关的现有模块、可复用资产、潜在冲突、推荐参考代码模式的结构化清单。被 spec-drafting 和 implementation-planning 调用，确保 spec 与 plan 都建立在「已理解现有代码」的基础上。也可由用户独立触发用于代码现状梳理。
---

# 代码侦察（Codebase Survey）

理解现有代码实现，识别新需求与现有代码的关联，输出结构化报告。

## 为什么需要这一步

AI 没看代码就起草 spec/plan，本质上是另一种"vibe coding"。代码侦察是**显式化**这个步骤，让产出可验证、可复用、可沉淀。

## 调用模式

支持两种深度，由调用方指定：

| 模式 | 调用方 | 关注粒度 | 阅读深度 | 输出位置 |
|------|--------|---------|---------|---------|
| **light** | spec-drafting（阶段零） | 模块级 | 接口 / 入口 / 注释 | spec 的「实施备注」+「关键代码参考」 |
| **deep** | implementation-planning（阶段二） | 文件 / 函数级 | 实现细节 / 调用链 / 设计模式 | plan 的「改动范围」+「关键代码参考」+「实施步骤」参考字段 |

> 用户也可以独立触发（如"帮我看下 wecardpalmapp 现状"），此时默认 light 模式。

## 执行步骤

### Step 1: 收集线索

- 从需求/spec 中提取关键词（业务名词、模块名、接口名）
- 列出潜在相关的目录（如 `src/wecardappgateway/`、`src/wecardpalmapp/`）

### Step 2: 扫码侦察（按模式）

#### Light 模式（阶段零用）

- 读取相关模块的 `interface.go` / 主入口 / `service.go`
- 识别已有功能的**作用、输入输出、调用方**
- 不深入实现细节
- 时间预算：< 5 分钟（约 5-10 个文件）

#### Deep 模式（阶段二用）

- 在 light 基础上深入：
  - 阅读 controller / service 的具体实现
  - 跟踪关键调用链（A → B → C 全路径）
  - 识别已有设计模式（如 enrollpalm 的 9 步流程）
  - 评估接口变更的影响范围
- 时间预算：< 20 分钟（约 15-30 个文件）

### Step 3: 分析与结构化

按以下维度归类：

1. **相关模块**：哪些模块与本次需求相关
2. **可复用资产**：已有的工具函数、接口、设计模式
3. **冲突 / 重叠**：与现有 spec 或代码功能是否重复
4. **推荐参考的代码模式**：实施时应模仿哪个已有实现
5. **潜在影响**：本次改动可能影响哪些调用方
6. **不确定项**：哪些代码意图不清楚，需要人确认

### Step 4: 输出报告

按调用模式选择对应输出格式（见下文）。

### Step 5: 写入目标位置

- Light：合并到 spec 的对应章节
- Deep：合并到 plan 的对应章节
- 独立触发：输出到对话，由用户决定是否单独存到 `docs/research/NNNN-codebase-survey.md`（可选）

## 输出格式

### Light 模式输出

```markdown
### 代码侦察报告（light）：[需求标题]

**相关模块**：
| 模块路径 | 作用 | 与本需求关系 |
|---------|------|-------------|
| `src/wecardappgateway/` | API 网关 | 需新增空中录掌 HTTP 接口 |
| `src/wecardpalmapp/` | 掌纹应用层 | 需新增空中录掌 controller |

**可复用资产**：
- `utils.GenerateUint64FromUUID()` — `src/common/utils/uuid.go` — 用于生成 palm_id
- `middleware.GetUserIdFromContext()` — `src/.../middleware/path_based.go` — 用于获取 user_id

**冲突 / 重叠**：
- 与现有 specs/<其他 STORYID> 的 X 功能可能重叠 — 建议[合并/独立/明确边界]
- 不与现有 spec 冲突

**推荐参考的代码模式**：
| 要做什么 | 参考文件 |
|---------|---------|
| Controller 9 步流程 | `controller/wecardpalm/enrollpalm/enroll_palm.go` |
| passvendor channel 调用 | `common/passvendor/providers/wechat/register_palm.go` |

**初步影响估计**：
- 预计需要新增：N 个文件（粗估）
- 预计需要修改：M 个文件（粗估）
- 跨模块：是 / 否
- 跨仓库：是 / 否（如涉及 proto 仓）

**不确定项 / 需要人确认**：
- [ ] 现有 X 模式是否还推荐使用？
- [ ] 是否有内部约定我没扫到的？
```

### Deep 模式输出

```markdown
### 代码侦察报告（deep）：Spec NNNN

**改动文件清单**（精确到文件 + 关联 FR）：
| 操作 | 文件 | 关联 FR | 说明 |
|------|------|---------|------|
| 新增 | `src/.../online_register_palm.go` | FR-3 | gateway 服务方法 |
| 修改 | `src/.../service.go` | FR-3 | 新增 GetWecardPalmAppClient |
| 修改 | `src/.../config.yaml` | FR-3 | 新增 wecardpalmapp 服务地址 |

**调用链分析**：
```
HTTP POST /wepalm/sdk/online-register-palm
  → PathBasedMiddleware → HTTPTokenMiddleware（取 user_id）
  → wecardappgateway.OnlineRegisterPalm（透传）
  → wecardpalmapp.OnlineRegisterPalm（业务逻辑）
  → passvendor.wechat.OnlineRegisterPalm（远端调用）
  → palm_global.PrivateRecognition.OnlineRegisterPalm
```

**接口影响**：
- 新增公共接口：[列表]
- 修改公共接口：[列表 + 是否兼容]
- 影响的调用方：[列表]

**已有设计模式**（必须遵循）：
| 模式 | 参考实现 | 关键约束 |
|------|---------|---------|
| Controller 9 步流程 | `enrollpalm/enroll_palm.go` | lic 预扣 → 失败回退；状态更新失败不回退 lic |
| passvendor channel | `wechat/register_palm.go` | 30s 超时；TraceID 优先做 session_id |
| 日志脱敏 | `sensitive.MaskSensitiveValue` | 禁用 `%+v` 打印请求结构体 |

**影响范围风险**：
| 风险 | 概率 | 应对 |
|------|------|------|
| Proto 改动需先合并 palm_proto | 高 | 本地用 replace 联调，提交前去掉 |
| 鉴权路径误命中其他中间件 | 低 | 已验证不命中 |

**不确定项 / 需要人确认**：
- [ ] 错误码 100034-100037 是否有冲突？
- [ ] FRegisterType 用 Mobile 还是新增 Online？
```

## 注意事项

### ❌ 不要做

- 不要扫描超出需求范围的代码（避免上下文爆炸）
- 不要把侦察结果写成"我修改了什么"——这一步还没改任何代码
- 不要假设代码意图（标注「AI 推断」与「从代码读出的事实」）
- Light 模式不要进入实现细节
- Deep 模式不要扫毫无关联的模块

### ✅ 应该做

- 优先读 `interface.go` / 入口文件，按需向下深入
- 遇到不理解的代码意图，列入「不确定项」
- 标注每条发现的来源（哪个文件的哪一段）
- 优先复用已有资产，而不是建议新增
- 引用相关已有 spec / plan 作为关联背景

## 与其他 Skill 的关系

```
spec-drafting (阶段零)
   └─ 调用 → codebase-survey (light)
              └─ 输出合并到 spec 的「实施备注」+「关键代码参考」

implementation-planning (阶段二)
   └─ 调用 → codebase-survey (deep)
              └─ 输出合并到 plan 的「改动范围」+「关键代码参考」

feature-implementation (阶段三)
   └─ 不调用 codebase-survey（按 plan 执行；改文件前的局部读取由 rules/20-coding-rules.md 规定）
```

## 关联资产

- **被调用方**：`skills/spec-drafting/SKILL.md`、`skills/implementation-planning/SKILL.md`
- **规则**：`rules/10-spec-workflow.md` 阶段零、阶段二
- **可选独立产物**：`docs/research/NNNN-codebase-survey.md`（仅在大型跨模块需求时使用）
