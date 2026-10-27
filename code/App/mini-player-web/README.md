# MiniPlayer · Web 端

> 对应书中 `10.Apps/07.综合音视频案例/04.Web端实现(Vue3).md` 的工程落地。
> 阶段 ①–⑨ 全链路实现：MVP → 导入 → 核心播放 → Dexie 持久化 →
> Media Session → LRC 歌词 → 搜索/主题 → 10 频段 EQ + 睡眠定时。

## 快速上手

```bash
cd code/App/mini-player-web
pnpm install      # 或 npm install / yarn
pnpm dev          # 开发服 http://localhost:5173
pnpm build        # 静态产物在 dist/
pnpm preview      # 预览 build 产物
pnpm typecheck    # 纯类型检查，不出产物
```

> ⚠ 首次 `pnpm install` 会下载 Vue 3、Pinia、Dexie、music-metadata-browser 等依赖，大约 100MB。

## 自备测试素材

浏览器沙盒决定了：**我们不打包任何真实音视频到仓库**，由你自己导入。两条路：

- **方式 A（推荐）**：开发服启动后点右上角「📁 导入目录」，选一个本机音乐文件夹（Chrome / Edge 支持 File System Access API）；
- **方式 B（兼容性最好）**：直接把音频文件/目录拖到页面中间，或点「📄 导入文件」多选。

想随手试一下，可以在 `public/media/` 下放 1-2 个 mp3/mp4 自测——放完刷新页面，用「选择文件」挑它们导入即可。

## 目录结构（三段式对齐）

```text
src/
├── main.ts                  入口
├── App.vue                  根组件（<audio>/<video> 托管给 store）
├── style.css                全局样式 + 主题 CSS 变量
├── router/                  三 Tab 子路由
├── types/                   MediaItem / PlayerState / LRC 等
├── data/                    架构篇 § ② Repository 层
│   ├── db.ts                Dexie Schema
│   ├── repository.ts        七类查询 + 写入
│   └── scanner.ts           file → MediaItem + music-metadata 解析
├── service/                 架构篇 § ③ PlayerService 层 + 平台适配
│   ├── playerService.ts     Pinia store · 心脏
│   ├── mediaSession.ts      navigator.mediaSession 适配
│   ├── equalizer.ts         10 频段 Web Audio EQ
│   ├── sleepTimer.ts        睡眠定时 + GainNode 渐弱
│   └── lyricsParser.ts      LRC 解析 + 二分查找
├── ui/                      架构篇 § ⑤ UI 层
│   ├── pages/
│   │   ├── LibraryPage.vue
│   │   ├── PlayingPage.vue
│   │   └── SettingsPage.vue
│   └── components/
│       ├── PlayerBar.vue    底部常驻控制条
│       ├── ProgressBar.vue  拖动标志位 + rAF 插值
│       ├── MediaList.vue    列表（预留 virtual-scroll）
│       ├── LyricsView.vue   歌词滚动高亮
│       └── EqualizerPanel.vue
└── utils/                   format / debounce / theme
```

## 功能覆盖矩阵（对照书中 9 阶段）

| 阶段 | 覆盖 | 对应代码 |
|------|------|---------|
| ① MVP 骨架 | ✅ | `main.ts` + `App.vue` 根托管 `<audio>` |
| ② 媒体扫描与导入 | ✅ | `scanner.ts` + `LibraryPage.vue` 三条通道（dir / file / drag） |
| ③ 核心播放控制 | ✅ | `playerService.ts`：模式切换、拖动标志位、rAF 插值、`play()` reject 保护 |
| ④ 视频播放增强 | ⚠️ 基础 | `<video>` 根托管 + 全屏按钮；画中画/多轨/字幕留挑战题 |
| ⑤ 本地持久化 | ✅ | `db.ts` + `repository.ts`，含断点续播 / 历史 200 条上限 |
| ⑥ 后台 + Media Session | ✅ | `mediaSession.ts` 全套 actionHandler + setPositionState |
| ⑦ LRC 歌词 | ✅ | `lyricsParser.ts` + `LyricsView.vue` CSS 滚动 + 二分定位 |
| ⑧ 搜索 + 主题 | ✅ | `LibraryPage` 搜索 + debounce；`utils/theme.ts` 三态持久化 |
| ⑨ EQ + 睡眠定时 | ✅ | `equalizer.ts` 10 BiquadFilter + 5 预设；`sleepTimer.ts` GainNode 渐弱 |

## 已知"沙盒约束"（文档 §1.1 对照）

- **刷新页面 File 对象失效**：当前实现只恢复元数据，不恢复文件引用。要做"刷新即可继续播放本地文件"需要换成持久化 `FileSystemHandle`（§05 Step 5.2 的进阶挑战）。
- **自动播放策略**：首次打开应用，`play()` 必须在用户点击/键盘手势里调用；本工程在 `playerService.loadIntoElement` 做了 catch 兜底，不会卡死 UI。
- **AudioContext 必须手势里 resume**：开启 EQ 开关这一下算用户手势，`equalizer.ensureRunning()` 就在那里调。

## 开发期常用快捷键（Playing 页）

| 键 | 功能 |
|----|------|
| Space | 播放/暂停 |
| ← / → | 后退/前进 5s |
| ↑ / ↓ | 音量 ±5% |
| M | 静音切换 |
| F | 当前为视频时切全屏 |

> 当前未全局注册——键盘控制在 `playerService` 已暴露 action，补一个 `useGlobalShortcuts.ts` 组合式函数即可挂到 `App.vue` 的 `window` 事件（挑战题 §11）。

## 常见问题

**Q：Safari 看不到「导入目录」按钮？**
A：Safari / Firefox 不支持 `showDirectoryPicker`，自动降级为「导入文件」+ 拖拽。文档 §03 Step 3.1 的兼容矩阵。

**Q：MP3 没封面？**
A：音频没内嵌 ID3 封面（APIC/METADATA_BLOCK_PICTURE）。`music-metadata-browser` 识别不到就不显示——不是 bug。

**Q：清空媒体库之后 EQ 还开着？**
A：清空操作调了 `equalizer.destroy()`，但 `AudioContext` 关闭后需重开。已经让 Settings 页在清空后 `location.reload()`。

## License

MIT · 仅供配套学习使用。
