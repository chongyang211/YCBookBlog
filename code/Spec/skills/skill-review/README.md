## 技能文档

### 基本信息
- 技能名: `skill-review`
- 创建人: @alfredyu (alfredyu@tencent.com)
- 版本: v1.0.0
- 更新时间: 2026-07-23

### 适用场景
团队内部 Skill 质量与安全评审工具，适用于以下场景：
- 团队成员编写完 Skill 后的提交前自检
- Reviewer 对 MR 中的 Skill 做标准化评审
- 定期对团队所有 Skill 做质量巡检
- 新人通过评审报告快速了解 Skill 规范要求

### 设计理念
融合两套规范的各自优势：
- **公司白皮书**（定底线）：11条安全红线 + metadata治理字段 + description排除项 + 鉴权安全
- **skill-rate**（促质量）：100分制量化评分 + Before/After示例 + Few-Shot要求 + 反模式检测

评审流程：红线扫描（阻断级）→ 治理合规（必填字段）→ 量化评分（五维度）→ 反模式检测

### 前置条件
- 被评审的 Skill 已编写完成（至少包含 SKILL.md）
- SKILL.md 采用 Markdown 格式，头部包含 YAML Frontmatter

### 使用示例
```
"帮我评审一下 /path/to/my-skill"
"审核这个 skill 能不能合入"
"给 <团队专属技能> 评分"
"检查一下这个 skill 有什么问题"
"skill 质量评估"
"提交前帮我自检一下"
```

### 注意事项
⚠️ 本 Skill 为只读评审，不会修改任何被评审文件
⚠️ 安全红线为一票否决，触发即阻断，无论其他维度得分多少
⚠️ D 维度（工程化评估）首次提交可标记 experimental，30天内补齐
⚠️ 评分规则详见 references/scoring-rules.md
⚠️ 安全红线判定详见 references/security-redlines.md

### 与其他评审工具的关系
- 本 Skill 是 `skill-rate` 的增强版，额外整合了公司白皮书的安全红线和治理要求
- 如果团队仅需快速自检，可使用 `skill-rate`；如需正式评审，使用本 Skill

### 已知问题
- [ ] D2 触发准确率需要实际运行环境支持，当前为人工评估
- [x] 融合白皮书11条红线和skill-rate评分体系 (v1.0.0)
- [x] 支持 experimental 标记的渐进式评审 (v1.0.0)
