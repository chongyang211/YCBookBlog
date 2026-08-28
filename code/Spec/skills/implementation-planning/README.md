## 技能文档

### 基本信息
- 技能名: `implementation-planning`
- 版本: v1.0.0
- 更新时间: 2026-03-26

### 适用场景
- Spec 分析完成后，需要产出可执行的实施计划
- 评估一个 spec 的实施复杂度和改动范围
- 将大 spec 拆分为可逐步执行的子任务

### 前置条件
- 已完成 spec-analysis 或已充分理解 spec 内容
- 能访问 `src/` 目录查看现有代码

### 使用示例
```
请根据 specs/v1.6.0/0-example-feature.md 执行 spec-plan
帮我把这个 spec 拆成实施步骤
```

### 注意事项
⚠️ Plan 应在实施前获得确认
⚠️ 不要在 plan 中写具体代码

### 相关技能
- `spec-analysis`: 前置技能，先分析再做计划
- `feature-implementation`: 后续技能，按 plan 实施
