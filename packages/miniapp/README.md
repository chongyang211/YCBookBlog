# 杨充的书架 · 微信小程序

把 [编程进阶网](https://yccoding.com/) 的全部文章做成可离线阅读的小程序，基于微信云开发（CloudBase）。

## 功能特性

- **分类树导航**：首页按「一级分类 → 子目录 → 文章」三级浏览
- **全文搜索**：1081 篇文章标题 / 摘要 / 分类即时过滤
- **Markdown 渲染**：标题、列表、引用、代码块、表格（GitHub 风格）
- **阅读历史**：本地缓存最近 50 条，首页横向展示
- **上一篇 / 下一篇**：阅读页底部一键翻页
- **云端加载**：正文按需从云存储拉取，本地自动缓存

## 目录结构

```
miniapp/
├── app.js / app.json / app.wxss     # 全局入口、页面注册、全局样式
├── project.config.json              # 项目配置（appid 在此填写）
├── config.js                        # 本地私有配置（已 gitignore，集中填真实值）
├── config.example.js                # 配置模板（入库，供参考）
├── sitemap.json                     # 站点地图
├── pages/
│   ├── index/                       # 首页：搜索 + 分类树 + 最近阅读
│   ├── catalog/                     # 目录页：子目录 / 文章列表
│   └── reader/                      # 阅读页：正文渲染 + 翻页 + 分享
├── utils/
│   ├── cloud.js                     # 云初始化、拉取章节、搜索
│   └── markdown.js                  # Markdown → HTML 转换器
├── cloudfunctions/
│   └── getContent/                  # 云函数：按 ID 读章节 JSON
├── scripts/
│   ├── sync-content.js              # 把 website/*.md 解析成 JSON
│   └── upload-to-cloudbase.js       # 批量上传章节到云存储
└── static/
    ├── catalog.json                 # 轻量目录索引（随包打包）
    └── chapters/                    # 每篇独立 JSON（上传到云端）
```

## 快速开始

### 1. 填写配置

所有真实值集中在 `config.js`（本地私有文件，已加入 `.gitignore`，不会提交）：

| 字段 | 说明 |
|------|------|
| `cloudEnvId` | CloudBase 云环境 ID |
| `appid` | 小程序 AppID（记录用，实际生效见 `project.config.json`） |
| `siteUrl` | 站点域名（一般无需修改） |

```bash
# 首次使用，先复制模板为本地配置
cp config.example.js config.js
```

> ⚠️ `appid` 必须同时写在 `project.config.json` 中——微信开发者工具只读取该 JSON 文件，无法通过 JS 配置驱动，这是平台限制。

### 2. 生成数据

在仓库根目录执行：

```bash
node packages/miniapp/scripts/sync-content.js
```

会把 `packages/website/` 下所有 md 解析为：

- `static/catalog.json`（目录索引，随小程序打包）
- `static/chapters/*.json`（每篇正文，需上传云端）

### 3. 上传正文到云存储

**方式 A：CLI 批量上传**

```bash
npm i -g @cloudbase/cli
tcb login
# 修改 upload-to-cloudbase.js 里的 ENV_ID
node packages/miniapp/scripts/upload-to-cloudbase.js
```

**方式 B：开发者工具手动上传**

云开发控制台 → 存储 → 新建 `chapters` 文件夹 → 批量上传 `static/chapters/` 下全部 JSON。

### 4. 部署云函数

在微信开发者工具中，右键 `cloudfunctions/getContent` → 上传并部署（云端安装依赖）。

### 5. 运行

用微信开发者工具打开 `packages/miniapp` 目录即可预览。

## 数据流说明

```
website/*.md
   │  sync-content.js
   ├── catalog.json ──────────→ 打包进小程序（离线目录）
   └── chapters/*.json ───────→ 上传 CloudBase 云存储
                                     │
阅读页按需下载 ──── fetchChapter(id) ────┘  （本地 wx.storage 缓存）
```

## 注意事项

- 云函数 `getContent` 作为「云存储下载失败」时的降级方案，二选一即可。
- `static/chapters/` 体积较大（约几十 MB），已加入 `.gitignore`，不入库、直接上传云端。
- 正文 Markdown 中的 mermaid 流程图暂不支持在小程序内渲染（rich-text 限制），会作为代码块展示。
