# Spec Drafting Skill

把原始需求（口语化、非结构化）转化为符合 `specs/templates/spec-template.md` 格式的 spec 文档草稿。

## 用途

工作流的**最起点**——把"产品/业务方提的一段话"转成结构化的 spec 草稿，供后续 plan/tasks/implement 各阶段使用。

## 调用方式

由 `/spec-draft` 命令自动触发，或用户直接说：
> 帮我把 docs/intake/v1.6.0/2026-06-09-payment-retry.md 写成 spec

## 关键原则

- **AI 是起草助手，不是业务方**
- **必须主动问 2-5 个澄清问题**
- **业务目标 / 验收标准 / 安全合规 → 标 [TBD]，不臆造**
- **输出 status 一定是 draft**

## 详细执行流程

见 [SKILL.md](./SKILL.md)。
