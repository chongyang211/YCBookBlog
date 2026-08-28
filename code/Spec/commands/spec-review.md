# Command: spec-review

## 命令名称

`spec-review`

## 作用

对已完成的实现进行规则级回顾与检查，确认实现是否符合 spec、rules、和项目原则。

## 输入参数

| 参数 | 必选 | 说明 |
|------|------|------|
| spec 路径 | 是 | 对应的 spec 文件路径 |
| 改动文件 | 否 | 如果不指定，自动识别本次实施涉及的文件 |

**触发方式**：

```
请执行 spec-review
```

或：

```
请根据 specs/v1.6.0/0-example-feature.md 执行 spec-review
```

## 执行步骤

0. **前置校验 — Story ID + 版本目录沿用**（强制）
   - 从 spec 文件名解析 Story ID（`specs/<VERSION>/<STORYID>-<slug>.md`）
   - **解析 `<VERSION>`，定位同版本目录下的 spec / plan / tasks 进行符合性检查**
   - 校验 spec / plan / tasks 三件套文件名 STORYID 段一致
   - 校验 spec frontmatter 的 `Story ID` 字段与文件名一致
   - Review 报告抬头必须含 Story ID（便于在工蜂等系统反向关联）

1. **Spec 符合性检查**
   - 逐条对照 spec 的验收标准
   - 确认每条标准是否在代码中得到满足
   - 标注任何偏离

2. **规则合规检查**
   - 对照 `rules/20-coding-rules.md`：
     - 是否有超出范围的修改？
     - 是否引入了不必要的依赖？
     - 是否保持了现有代码风格？
   - 对照 `rules/30-testing-rules.md`：
     - 测试覆盖是否足够？
     - 测试命名是否规范？

3. **代码质量检查**
   - 输入验证是否完整
   - 错误处理是否合理
   - 公共接口是否有文档
   - 是否有遗留的 TODO 或 FIXME

4. **影响范围检查**
   - 修改是否可能影响其他模块
   - 接口变更是否有兼容性问题
   - 数据结构变更是否需要迁移

5. **输出审查结果**

## 输出结果

```markdown
### Spec Review: [Story ID]

**Spec 符合性**：
| 验收标准 | 状态 | 说明 |
|----------|------|------|
| [标准1] | ✅ 通过 | — |
| [标准2] | ⚠️ 部分满足 | [说明] |
| [标准3] | ❌ 未满足 | [说明] |

**规则合规**：
- [✅/⚠️/❌] 最小改动原则
- [✅/⚠️/❌] 代码风格一致性
- [✅/⚠️/❌] 测试覆盖
- [✅/⚠️/❌] 错误处理

**发现的问题**：
1. [问题描述 + 建议修复方式]

**总体评价**：
[通过 / 需修改后通过 / 不通过]
```

## 适用时机

- 实现和测试都完成后，正式提交前
- 对已有代码做质量检查
- 怀疑实现可能偏离 spec

## 关联规则与技能

- **规则**：`rules/10-spec-workflow.md`（全流程）、`rules/20-coding-rules.md`、`rules/30-testing-rules.md`
- **技能**：`skills/spec-analysis.md`、`skills/change-summary.md`
