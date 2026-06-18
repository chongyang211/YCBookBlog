<p align="center">
  <a href="https://vuepress.vuejs.org/" target="_blank">
    <img src="https://img.shields.io/badge/VuePress-2.x-brightgreen.svg" alt="VuePress">
  </a>
  <a href="https://github.com/xugaoyi/vuepress-theme-vdoing" target="_blank">
    <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License">
  </a>
</p>


# YCBookBlog

基于 **VuePress 2** + **vuepress-theme-hope** 的个人技术博客，涵盖前端、后端、移动开发、Linux、Go、Java、C/C++、历史人文等多个领域。

## 快速开始

### 环境要求

- [Node.js](https://nodejs.org/) >= 18（推荐 v20+）
- npm

### 安装

```bash
git clone <repo-url>
cd YCBookBlog

# 安装依赖（--legacy-peer-deps 仅首次需要）
npm install --legacy-peer-deps
```

### 本地开发

```bash
# macOS / Linux
npm run dev 
# Windows         
npm run dev:win      
```

启动后访问 `http://localhost:8080` 预览博客，支持热更新。

### 构建 & 部署

```bash
# 生成静态文件 → docs/.vuepress/dist/
npm run build
# 统计构建耗时
npm run build:time
# Windows
npm run build:win    

# 推送至 gh-pages 分支
npm run deploy
```

### 其他命令

| 命令 | 说明 |
|------|------|
| `npm run editFm` | 批量编辑文章 frontmatter |
| `npm run baiduPush` | 向百度提交站点链接 |

## 项目结构

```
YCBookBlog/
├── docs/                    # 文档源目录
│   ├── .vuepress/           # VuePress 配置
│   │   ├── config.ts        # 站点配置（导航、侧边栏、插件）
│   │   ├── client.ts        # 客户端增强入口
│   │   ├── styles/          # 自定义 CSS
│   │   ├── public/          # 静态资源（favicon、图片）
│   │   └── config/          # 辅助配置模块
│   ├── index.md             # 首页
│   └── **/*.md              # 文章页面
├── utils/                   # 工具脚本
├── deploy.sh                # 一键部署脚本
├── package.json
└── README.md
```

## 写作指南

所有文章放在 `docs/` 目录下，使用 Markdown 编写。每篇文章头部需包含 frontmatter：

```yaml
---
title: 文章标题           # 必填
date: 2024-01-01          # 必填
categories:               # 分类
  - 前端
tags:                     # 标签
  - Vue
  - JavaScript
author:                   # 作者（可选，默认站点作者）
  name: 你的名字
sidebar: false            # 是否隐藏侧边栏（可选）
---
```

### 目录组织

按类别创建子文件夹，例如：

```
docs/
├── 01.前端/
│   ├── 01.Vue/
│   └── 02.React/
├── 02.后端/
├── 03.技术/
└── 04.随笔/
```

文件夹以序号前缀命名可以在侧边栏中保持排序。

### 特殊页面

- `_posts/` — 随笔、短文（不按侧边栏分类）
- `@pages/` — 独立页面（关于、友链等）

## 常见问题

### 安装报 peer dependency 冲突

```bash
npm install --legacy-peer-deps
```

hope theme 仍处于 RC 阶段，部分 peer 依赖尚未对齐，使用 `--legacy-peer-deps` 可绕开。

### 构建报 "Illegal '/' in tags"

部分老文章的裸 `<https://...>` URL 会被 Vue SFC 编译器误解析，将其改为 `&lt;https://...&gt;` 或包裹在 `` ` `` 中即可。

### 端口被占用

```bash
# 查看占用端口的进程
lsof -i :8080

# 或指定其他端口
npx vuepress dev docs --port 3000
```

## 许可证

[MIT](LICENSE)
