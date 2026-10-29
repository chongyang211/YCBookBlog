# MiniPlayer · Web 端

> 对应书中 `10.Apps/07.综合音视频案例/04.Web端实现(Vue3).md` 的工程落地。
> 阶段 ①–⑨ 全链路实现：MVP → 导入 → 核心播放 → Dexie 持久化 →
> Media Session → LRC 歌词 → 搜索/主题 → 10 频段 EQ + 睡眠定时。
>
> **当前版本：v0.2 (2026-10-29)** · 重大 UI 重设计 + 关键 Bug 修复，详见下方「v0.2 更新摘要」。

## 快速上手

```bash
cd code/App/mini-player-web
pnpm install      # 或 npm install / yarn
pnpm dev          # 开发服 http://localhost:5173
pnpm build        # 静态产物在 dist/
pnpm preview      # 预览 build 产物
pnpm typecheck    # 纯类型检查，不出产物
```

> ⚠ 若 `pnpm dev` 报 `Cannot find module '@rollup/rollup-darwin-arm64'`（pnpm 已知 optional dep bug），运行一次：
> ```bash
> pnpm add -D @rollup/rollup-darwin-arm64    # Apple Silicon
> # 或 pnpm add -D @rollup/rollup-linux-x64-gnu / @rollup/rollup-win32-x64-msvc 对应你的平台
> ```

## 自备测试素材

浏览器沙盒决定了：**我们不打包任何真实音视频到仓库**，由你自己导入。两条路：

- **方式 A（推荐）**：开发服启动后点右上角「📁 导入目录」，选一个本机音乐文件夹（Chrome / Edge 支持 File System Access API）；
- **方式 B（兼容性最好）**：直接把音频文件/目录拖到页面中间，或点「📄 导入文件」多选。

想随手试一下，可以在 `public/media/` 下放 1-2 个 mp3/mp4 自测——放完刷新页面，用「导入文件」挑它们导入即可。

## v0.2 更新摘要

### 🐛 关键 Bug 修复

- **"导入文件却无法播放"**：根因是 `repository.addMediaBatch` 存库前 strip 掉 `file` 字段，reload 读回的 `items` 没有 `file`。新增 `src/service/fileCache.ts` 内存池，scanner 完成后自动注册、`loadIntoElement` 优先从 cache 取。详见文档 §04 "fileCache 修复详解"。
- **`style.css` JS 风格 `//` 注释**：改为 `/* */`。
- **音量拖动不自动取消静音、文件失效态无提示** 等 UX 细节。

### ✨ UI 全面重设计

- **紫粉渐变品牌色** + **毛玻璃卡片**（`backdrop-filter`）；
- **动态封面背景**：当前曲目封面放大 blur 作为软背景；
- **三断点响应式**（480 / 820 / 1024）：桌面侧栏、平板图标栏、手机底部 Tab；
- **页面切换动画**、**自定义 range thumb**、**ProgressBar hover 时间气泡**；
- 完整 CSS 变量设计系统（40+ tokens）。

### 🧰 功能增强

- 新增 **`QueueDrawer`**（当前播放队列）、**`EmptyState`**（空库引导）、**`CoverArt`**（渐变占位）、**`MobileNav`**（手机底部）、**`ToastHost` + toast 服务**、**`AutoplayBanner`**（自动播放策略提示条）；
- **MediaList**：正在播放动态频谱图标、文件失效徽标、单条删除；
- **SettingsPage**：播放速度 7 档选择；
- **PlayerBar**：loading spinner、队列按钮；
- 全局 Toast 覆盖 10+ 处操作反馈。

### 📊 产物指标

- `pnpm build` → `dist/` 产物 **~200KB gzip**
- `pnpm typecheck` 0 error / `pnpm build` 0 warning

## 目录结构（三段式对齐）

```text
src/
├── main.ts                       入口
├── App.vue                       根组件（<audio>/<video> 托管 + 动态背景层）
├── style.css                     v0.2 全新设计系统（CSS 变量 40+ tokens）
├── router/                       三 Tab 子路由
├── types/                        MediaItem / PlayerState / LRC 等
├── data/                         架构篇 § ② Repository 层
│   ├── db.ts                     Dexie Schema
│   ├── repository.ts             七类查询 + 写入
│   └── scanner.ts                file → MediaItem + music-metadata + 注册 fileCache
├── service/                      架构篇 § ③ PlayerService 层 + 平台适配
│   ├── playerService.ts          Pinia store · 心脏（含 fileCache 集成 + loading 状态）
│   ├── fileCache.ts              🆕 内存 File 池（Bug 修复关键）
│   ├── mediaSession.ts           navigator.mediaSession 适配
│   ├── equalizer.ts              10 频段 Web Audio EQ
│   ├── sleepTimer.ts             睡眠定时 + GainNode 渐弱
│   ├── lyricsParser.ts           LRC 解析 + 二分查找
│   └── toast.ts                  🆕 全局 Toast 服务
├── ui/                           架构篇 § ⑤ UI 层
│   ├── pages/
│   │   ├── LibraryPage.vue       媒体库（含空库引导 / 拖拽覆盖层）
│   │   ├── PlayingPage.vue       正在播放（大封面 + 歌词 + 控制）
│   │   └── SettingsPage.vue      主题 / 速度 / 睡眠 / EQ / 统计
│   └── components/
│       ├── PlayerBar.vue         底部控制条（三段式 + 响应式）
│       ├── ProgressBar.vue       拖动 + hover 时间气泡
│       ├── MediaList.vue         列表（频谱动画 / 失效徽标 / 单删）
│       ├── LyricsView.vue        歌词滚动高亮
│       ├── EqualizerPanel.vue    10 频段 EQ（自定义开关 + 渐变 preset）
│       ├── CoverArt.vue          🆕 封面统一组件
│       ├── QueueDrawer.vue       🆕 当前播放队列抽屉
│       ├── EmptyState.vue        🆕 空库引导
│       ├── ToastHost.vue         🆕 Toast 宿主
│       ├── MobileNav.vue         🆕 手机底部 Tab Bar
│       └── AutoplayBanner.vue    🆕 自动播放策略提示
└── utils/
    ├── format.ts                 时间 / 体积格式化
    ├── debounce.ts               搜索防抖
    ├── theme.ts                  三态主题
    ├── colorFromId.ts            🆕 基于 id 的稳定渐变色
    └── useGlobalShortcuts.ts     全局快捷键
```

## 功能覆盖矩阵（对照书中 9 阶段）

| 阶段 | 覆盖 | 对应代码 |
|------|------|---------|
| ① MVP 骨架 | ✅ | `main.ts` + `App.vue` 根托管 `<audio>` |
| ② 媒体扫描与导入 | ✅ | `scanner.ts` + `LibraryPage.vue` 三条通道（dir / file / drag） |
| ③ 核心播放控制 | ✅ | `playerService.ts`：fileCache 集成、模式切换、拖动标志位、rAF 插值、`play()` reject 保护 |
| ④ 视频播放增强 | ⚠️ 基础 | `<video>` 根托管 + 全屏按钮；画中画/多轨/字幕留挑战题 |
| ⑤ 本地持久化 | ✅ | `db.ts` + `repository.ts`，含断点续播 / 历史 200 条上限 |
| ⑥ 后台 + Media Session | ✅ | `mediaSession.ts` 全套 actionHandler + setPositionState |
| ⑦ LRC 歌词 | ✅ | `lyricsParser.ts` + `LyricsView.vue` CSS 滚动 + 二分定位 |
| ⑧ 搜索 + 主题 | ✅ | `LibraryPage` 搜索 + debounce；`utils/theme.ts` 三态持久化 |
| ⑧.5 UI 升级 (v0.2) | 🆕 | 设计系统 + 毛玻璃 + 响应式三断点 + 6 新组件（详见书 §08.5） |
| ⑨ EQ + 睡眠定时 | ✅ | `equalizer.ts` 10 BiquadFilter + 5 预设；`sleepTimer.ts` GainNode 渐弱 |

## 快捷键

| 键 | 功能 |
|----|------|
| Space | 播放 / 暂停 |
| ← / → | 后退 / 前进 5s |
| ↑ / ↓ | 音量 ±5% |
| M | 静音切换 |
| N / P | 下一首 / 上一首 |
| F | 当前为视频时切全屏 |

> 输入框聚焦时所有快捷键自动放行，不会抢焦点。

## 响应式断点

| 断点 | 布局 |
|------|------|
| ≥ 1025px (桌面) | 侧栏 240px + 主区 + 底部 PlayerBar |
| 641-1024px (平板) | 侧栏自动变 72px 图标栏 |
| ≤ 640px (手机) | 单栏主区 + 底部 PlayerBar + 底部 Tab Bar |

## 已知"沙盒约束"

- **刷新页面 File 对象失效**：当前实现只恢复元数据，不恢复文件引用（因为 File 对象无法稳定跨 session 持久化）。要做"刷新即可继续播放本地文件"需要换成持久化 `FileSystemHandle`（见文档 §12.3 挑战题）。
- **自动播放策略**：首次打开应用，`play()` 必须在用户点击/键盘手势里调用；本工程在 `playerService.loadIntoElement` 做了 catch 兜底，不会卡死 UI。还会显示 `AutoplayBanner` 一键恢复。
- **AudioContext 必须手势里 resume**：开启 EQ 开关这一下算用户手势，`equalizer.ensureRunning()` 就在那里调。

## 常见问题

**Q：导入文件成功，但播放没声音？**
A：**v0.2 已修复**。如果仍遇到：① 检查文件格式是否浏览器支持（Safari 对 FLAC 不佳）；② 检查浏览器是否静音了标签页；③ 控制台看是否被自动播放策略拦截（会有 Toast 提示）。

**Q：Safari 看不到「导入目录」按钮？**
A：Safari / Firefox 不支持 `showDirectoryPicker`，点击时会 Toast 提示"请使用导入文件或拖拽"。

**Q：MP3 没封面？**
A：音频没内嵌 ID3 封面（APIC/METADATA_BLOCK_PICTURE）。v0.2 已改为用 **基于 mediaId 的稳定渐变色占位**——每首歌颜色不同但稳定。

**Q：清空媒体库之后 EQ 还开着？**
A：清空操作调了 `equalizer.destroy()`，但 `AudioContext` 关闭后需重开。Settings 页在清空后自动 `location.reload()`。

**Q：手机端一直是单栏，PlayerBar 挤成一团？**
A：确认视口宽度 ≤ 640px（可能被模拟器缩放混淆）；真实手机上 iOS 的地址栏变化不会导致布局跳（已用 `100dvh` 兜底）。

## License

MIT · 仅供配套学习使用。
