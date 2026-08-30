# Codebase Survey Skill

理解现有代码实现，识别新需求与现有代码的关联，输出结构化报告。

## 用途

被 `spec-drafting` 和 `implementation-planning` 调用，确保 spec 与 plan 都建立在「已理解现有代码」的基础上，避免"vibe coding"。

## 两种调用模式

- **light**：spec 起草前的轻量侦察（模块级，< 5 分钟）
- **deep**：plan 起草前的深度侦察（文件 / 函数级，< 20 分钟）

## 调用方式

通常被其他 skill 自动调用。也可由用户独立触发：
> 帮我看下 wecardexample-app 模块的代码现状

## 详细执行流程

见 [SKILL.md](./SKILL.md)。
