# Command: spec-test

## 命令名称

`spec-test`

## 作用

根据 spec 的验收标准和测试点，为已完成的功能实现编写或补充测试。

## 输入参数

| 参数 | 必选 | 说明 |
|------|------|------|
| spec 路径 | 是 | 对应的 spec 文件路径 |
| 实现文件 | 否 | 如果不指定，自动识别本次实施涉及的文件 |

**触发方式**：

```
请执行 spec-test
```

或：

```
请根据 specs/v1.6.0/0-example-feature.md 执行 spec-test
```

## 执行步骤

0. **前置校验 — Story ID + 版本目录沿用**（强制）
   - 从 spec 文件名解析 Story ID（`specs/<VERSION>/<STORYID>-<slug>.md`）
   - **解析 `<VERSION>`，定位同版本目录下的 spec / plan / tasks**
   - 校验 spec frontmatter `Story ID` 字段与文件名一致
   - 测试报告 / 摘要中引用 Spec 时必须包含 Story ID

1. **读取 spec 的测试相关章节**
   - 验收标准
   - 测试点
   - 边界情况

2. **识别被测代码**
   - 确定本次实施的核心文件和接口
   - 了解被测模块的输入输出

3. **设计并编写测试**
   - 参考 `skills/test-writing.md` 的执行方式
   - 每个验收标准至少一个测试
   - 覆盖边界情况

4. **输出测试覆盖报告**
   - 列出测试用例与验收标准的映射关系
   - 标注无法自动化的测试

5. **完成后建议下一步**
   - 建议执行 `spec-review` 或直接输出变更摘要

## 输出结果

- 测试代码文件（写入 `tests/`）
- 测试覆盖报告（格式见 `skills/test-writing.md`）

## 适用时机

- 功能实现完成后
- 发现现有功能缺少测试
- Bug 修复后补充回归测试

## 关联规则与技能

- **规则**：`rules/10-spec-workflow.md`（阶段四）、`rules/30-testing-rules.md`
- **技能**：`skills/test-writing.md`
