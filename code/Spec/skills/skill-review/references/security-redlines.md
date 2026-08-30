# 安全红线详细判定规则

## 扫描方法

对 SKILL.md 和 scripts/ 目录下所有文件进行以下检查：

## R-1 硬编码凭据

**扫描关键词**：
- `password`、`passwd`、`token`、`secret`、`api_key`、`API_KEY`、`apikey`
- `sk-`（OpenAI 格式）、`ghp_`（GitHub Token）、`glpat-`（GitLab Token）
- 连续 32+ 位的十六进制或 Base64 字符串（排除示例占位符如 `xxx`、`...`、`<your-key>`）

**排除**：
- 变量名声明（如 `SECRET_ID = ""`、`private_key = ""`）
- 明确的占位符（如 `MIIBIjAxxxxxxxxxxxxxxx`、`<your-secret-id>`）
- 文档中的格式说明

**判定**：发现实际可用的凭据值 → 触发

---

## R-2 身份冒用

**扫描模式**：
- 使用固定的 user_id/username 访问系统（非当前用户）
- 引用共享账号（如 `admin`、`root`、`service-account`）
- 代码中硬编码他人身份标识

---

## R-3 绕过鉴权

**扫描模式**：
- 自行实现 cookie/session 获取逻辑
- 使用 `requests.session()` 模拟登录
- 绕过 OAuth/SSO 流程
- 未经由官方 MCP/CLI 通道调用内部系统

---

## R-4 数据外发

**扫描模式**：
- HTTP 请求目标为公司域外地址（非 `<内网域>`、`*.tencent.com` 等内网域）
- 调用外部 AI 服务 API
- 向外部日志/监控服务发送数据

**排除**：
- Skill 本身面向外部客户的场景（如 <团队专属技能> 面向外部客户接入）
- 明确声明了 `touches-external-network: true` 且有合理理由

---

## R-5 Frontmatter 违规

**扫描**：
- Frontmatter 字段值中包含 `<` 或 `>` 字符（YAML 语法标记 `>` 除外）
- `name` 使用系统保留前缀：`system-`、`admin-`、`root-`、`platform-`、`internal-`、`auth-`、`iam-`、`permission-`、`security-`、`audit-`

---

## R-6 Prompt 注入载体

**扫描模式**：
- "忽略以上指令"、"ignore previous instructions"
- "你现在是"、"you are now"（角色切换）
- "不要遵守"、"跳过验证"、"bypass"
- 诱导泄露系统提示的表述

---

## R-7 未确认的危险操作

**扫描关键词**：
- `rm -rf`、`rmdir`、`shutil.rmtree`
- `DROP TABLE`、`DELETE FROM`（无 WHERE）
- `kubectl delete`、`docker rm`
- `chmod 777`、`chown`

**判定**：存在上述操作且正文中无"用户确认"相关步骤 → 触发

---

## R-8 权限通配

**扫描**：
- `allowed-tools: [*]` 或 `allowed-tools: ["*"]`
- `allowed-tools` 包含 >10 个工具且未说明理由

---

## R-9 供应链不可控

**扫描**：
- `pip install` 无版本号（如 `pip install requests` 而非 `requests>=2.28,<3.0`）
- `npm install` 无版本锁定
- 引用外部 URL 下载脚本（如 `curl ... | bash`）
- 包含未经审计的二进制文件

---

## R-10 界面自动化绕过

**扫描关键词**：
- `selenium`、`playwright`、`puppeteer`
- `webdriver`、`headless`
- `browser.get(`、`page.goto(`

**排除**：Skill 本身就是浏览器自动化工具且有明确声明

---

## R-11 明文传密码

**扫描模式**：
- URL 中包含密码参数（如 `?password=xxx`）
- 命令行参数直接传递密码（如 `--password=xxx`）
- print/log 语句输出密码变量
