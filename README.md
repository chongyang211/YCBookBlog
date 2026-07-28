# YCBookBlog

技术博客 + 小程序，共享同一份 Markdown 内容源。

## 目录结构

```
YCBookBlog/
├── packages/
│   ├── website/           # VuePress 网站 → yccoding.com
│   │   ├── .vuepress/     # 主题配置、组件
│   │   └── index.md
│   │
│   └── miniapp/           # 微信小程序 → 杨充的书架
│       ├── app.js/wxss    # 入口，CloudBase 初始化
│       ├── pages/         # 首页 / 目录 / 阅读器
│       ├── scripts/       # md → JSON 同步、CloudBase 上传
│       ├── cloudfunctions/# 云函数：按需加载章节
│       └── static/        # catalog.json（目录索引）
│
├── book/                  # 其他书籍草稿
├── vdoing-theme/          # 本地主题源码（备用，当前使用 npm 包）
└── package.json
```

## 发布网站

```bash
# 本地预览
npm run dev

# 构建
npm run build
# → 产物在 packages/website/.vuepress/dist/，部署到服务器即可
```

## 发布小程序

### 前置准备

1. 注册微信小程序，获取 AppID
2. 在微信开发者工具中开通云开发，获取环境 ID
3. 修改两处配置：
   - `packages/miniapp/project.config.json` → 填入 `appid`
   - `packages/miniapp/utils/cloud.js` → 填入 `env`（环境 ID）

### 内容同步

```bash
# 每次 md 更新后：
node packages/miniapp/scripts/sync-content.js  # 生成 JSON

# 上传章节到 CloudBase：
# 方式一：微信开发者工具 → 云开发控制台 → 存储管理 → 上传
#          目标：packages/miniapp/static/chapters/ 下全部 JSON
# 方式二：node packages/miniapp/scripts/upload-to-cloudbase.js

# 部署云函数：
# 微信开发者工具 → 右键 cloudfunctions/getContent → 上传并部署
```

### 发布

微信开发者工具打开 `packages/miniapp/` → 上传 → 提交审核。

## 数据流

```
md 源文件                         用户打开小程序
    │                                    │
    ▼                                    ▼
sync-content.js                   catalog.json (包内)
    │                             即时展示目录 / 搜索
    ▼                                    │
chapters/*.json ──上传──▶ CloudBase ────┘  点击文章
                                               │
                              fetchChapter() ──┘
                                  │
                             拉取 JSON → 缓存 → rich-text 渲染
```
