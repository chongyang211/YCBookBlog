## 技能文档

### 基本信息
- 技能名: `spec-analysis`
- 版本: v1.0.0
- 更新时间: 2026-03-26

### 适用场景
- 收到新的功能开发任务，需要理解对应 spec
- 评估一个 spec 是否完整可实施
- 对比多个 spec 之间的依赖或冲突

### 前置条件
- `specs/` 目录中存在目标 spec 文件
- spec 文件遵循 `specs/templates/spec-template.md` 格式

### 使用示例
```
请分析 specs/v1.6.0/0-example-feature.md
请根据 specs/<VERSION>/<STORYID>-<slug>.md 执行 spec-plan（第一步会触发本技能）
```

### 注意事项
⚠️ 本技能只负责分析和评估，不负责产出实施计划
⚠️ 发现重大缺失时应停下等待补充，不要假设

### 相关技能
- `implementation-planning`: 分析完成后，用于产出实施计划
