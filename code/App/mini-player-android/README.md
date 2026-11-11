# MiniPlayer · Android 端配套工程 (v0.1)

> 对应文档：`packages/website/10.Apps/05.综合音视频案例/02.Android端实现(Java+AndroidX).md`
> 本工程是 MiniPlayer "**5 端 × 6 平台**" 教学案例中 Android 篇的 **v0.1 最小闭环**：
> **纯 Java + AndroidX**，覆盖阶段①-④（MVP → MediaStore 扫描 → ExoPlayer 播放控制 → Room 持久化）。

---

## v0.1 覆盖的能力

| 功能 | 对应代码 | 文档章节 |
|------|---------|---------|
| ViewBinding + Material3 主题（含 values-night） | `themes.xml` + `colors.xml` | §02 / §08.3 |
| 权限三代分叉（API 24/33/34+） | `MainActivity.ensurePermissionThenScan` | §03.3 |
| MediaStore 扫描（音频+视频，DURATION≥10s SQL 过滤） | `data/MediaScanner` | §03.2 |
| 稳定 ID：SHA1(uri+size+mtime)（五端同算法） | `MediaScanner.stableId` | §03.2 |
| MediaMetadataRetriever 兜底 + finally release（防 fd 泄漏） | `MediaScanner.fillMetadataIfNeeded` | §03.4 |
| 封面提取 + cache 目录缓存 + 降采样解码 | `MediaScanner` / `ui/MediaListAdapter` | §03.4 |
| RecyclerView + ListAdapter + DiffUtil | `ui/MediaListAdapter` | §03.5 |
| 扫描后台线程 + postValue 回主线程 | `data/MediaRepository` | §03.6 |
| PlayerState 九元组（不可变对象） | `service/PlayerState` | §04.1 |
| ExoPlayer + 事件翻译层 + 200ms 位置节流 | `service/PlayerManager` | §04.2 ⭐ |
| 播放模式决策树（顺序/单曲/随机） | `PlayerManager.nextIndex` | §04.5 |
| 倍速五档（PlaybackParameters 无级变速不变调） | `PlayerManager.cycleSpeed` | §04.6 |
| LiveData<PlayerState> 唯一数据源 | `ui/MainViewModel` | §04.3 |
| SeekBar 拖动标志位（五端同款坑） | `MainActivity.setupPlayerBar` | §04.4 |
| 旋转屏幕播放不中断（PlayerManager 挂 ViewModel） | `MainViewModel.player()` | §04 |
| Room 三表：media / play_record / favorite | `data/` 三件套 | §05.1 |
| 历史 200 条上限 trim + 断点续播（有效断点判定） | `MediaDao` / `MainViewModel.playWithResume` | §05.3 |

## 🟡 v0.2 路线（文档已给完整代码，工程待迁入）

- **§06 后台播放**：把 `PlayerManager` 整体迁入 `PlaybackService`（类代码零改动）+ MediaSessionCompat 四件套 + MediaStyle 通知 + BECOMING_NOISY
- **§07 LRC**：`LyricsParser` + 自绘 `LyricsView`
- **§08 搜索**：`SearchView` + 防抖 + SpannableString 高亮（DAO 的 search 已就绪）
- **§09 EQ**：`android.media.audiofx.Equalizer` 挂 audioSessionId + 睡眠定时
- 已知简化：`pruneMissing`（设备删除的文件不清理库）TODO

---

## 快速开始

### 环境要求

| 工具 | 版本 |
|------|------|
| Android Studio | Ladybug (2024.2) 或更新 |
| JDK | 17 |
| AGP | 8.7.2 |
| compileSdk / targetSdk | 35 |
| minSdk | 24（Android 7.0） |

### 构建与安装

```bash
# 方式一：Android Studio 打开工程根目录，Sync 后 Run

# 方式二：命令行
./gradlew :app:assembleDebug
adb install app/build/outputs/apk/debug/app-debug.apk
```

### 验收流程（对照文档各阶段 🧪）

```bash
# 1. 推测试素材（不打包进 APK，扫真实 MediaStore）
adb shell mkdir -p /sdcard/Music/MiniPlayerDemo
adb push your-song.mp3 your-song-2.flac /sdcard/Music/MiniPlayerDemo/

# 2. 触发媒体扫描（否则 MediaStore 索引不到新文件）
adb shell am broadcast -a android.intent.action.MEDIA_SCANNER_SCAN_FILE \
  -d file:///sdcard/Music/MiniPlayerDemo/your-song.mp3

# 3. 启动 App → 授权 → 列表出现 → 点击播放
```

**验收点**：
- 列表显示真实标题/艺术家/时长（FLAC 走 Retriever 兜底也正确）
- 播放中旋转屏幕：不中断、进度条恢复
- 拖动进度条无回跳
- 播到某处 → 划掉进程 → 重启点同一首 → 从断点继续（>5s 且距结尾 >10s 才恢复）

---

## 项目结构（三段式对齐）

```text
mini-player-android/
├── settings.gradle.kts / build.gradle.kts / gradle.properties
├── app/
│   ├── build.gradle.kts                # 依赖矩阵（media3 1.5 / room 2.6 / lifecycle 2.8）
│   ├── proguard-rules.pro              # Room/Media3 keep 规则
│   └── src/main/
│       ├── AndroidManifest.xml         # 权限三代声明 + §06 Service 预留注释
│       ├── java/com/ycbook/miniplayer/
│       │   ├── MainActivity.java       # 权限 + 列表 + PlayerBar + SeekBar 标志位
│       │   ├── data/                   # ② Repository 层
│       │   │   ├── MediaItem.java      # @Entity（对照架构篇 §1.3）
│       │   │   ├── Converters.java     # 枚举 ↔ String
│       │   │   ├── PlayRecord.java     # 播放历史 @Entity
│       │   │   ├── Favorite.java       # 我喜欢 @Entity
│       │   │   ├── MediaDao.java       # 参数绑定防注入 + 200 条 trim
│       │   │   ├── AppDatabase.java    # @Database 单例
│       │   │   ├── MediaScanner.java   # MediaStore + Retriever + 封面缓存 + 稳定 ID
│       │   │   └── MediaRepository.java # DB 优先 + 断点 + 收藏
│       │   ├── service/                # ③ PlayerService 层
│       │   │   ├── PlayerState.java    # 九元组（不可变）
│       │   │   └── PlayerManager.java  # ⭐ 心脏：ExoPlayer + 事件翻译 + 节流
│       │   └── ui/                     # ⑤ UI 层
│       │       ├── MainViewModel.java  # LiveData 唯一镜像
│       │       └── MediaListAdapter.java # ListAdapter + DiffUtil + 降采样
│       └── res/
│           ├── layout/activity_main.xml    # 列表 + PlayerBar 两段式
│           ├── layout/item_media.xml       # 封面 + 标题/艺术家 + 时长
│           ├── values/{colors,strings,themes}.xml
│           └── values-night/colors.xml     # 深色主题同名覆盖
└── README.md
```

---

## 与兄弟端对照（学完 Android 篇再看）

| 考点 | Android（本工程） | Web | Qt | Electron |
|------|-----------------|-----|-----|----------|
| 状态唯一数据源 | `LiveData<PlayerState>` | Pinia store | QObject 属性+信号 | Zustand + IPC |
| seek 拖动标志位 | `uiSeeking` + `setUserSeeking` | `dragValue` | `setSeeking` | `seeking` |
| 稳定 ID | `MessageDigest SHA-1` | SubtleCrypto | QCryptographicHash | node:crypto |
| 历史上限 200 | SQL `DELETE NOT IN` | Dexie 裁剪 | 同一 SQL | 同一 SQL |
| 断点"有效判定" | >5s 且距尾>10s | 同 | 同 | 同 |
| 旋转存活 | ViewModel 持有 Player | — | — | — |
| 后台播放 | PlaybackService（v0.2） | Media Session API | MPRIS/SMTC | 自写 C++ |

---

## 常见问题

### Q: 首次启动列表为空？
1. 检查权限是否授权（设置 → 应用 → MiniPlayer → 权限）
2. 新 push 的文件要触发 `MEDIA_SCANNER_SCAN_FILE` 广播（见上）
3. 短于 10 秒的音频被 SQL 层过滤——这是预期行为

### Q: 划掉进程播放就停？
v0.1 的预期限制（PlayerManager 在 ViewModel）。v0.2 迁入 ForegroundService 后解决，完整代码见文档 §06。

### Q: 某些 FLAC 标题显示文件名？
该文件的 Vorbis Comment 缺失 TITLE 或系统未索引。工程已走 Retriever 兜底；仍拿不到说明文件本身没有元数据（fallback 是设计行为）。

### Q: Room 编译报 "cannot find symbol Converters"？
先 Build → Rebuild Project。Room annotationProcessor 在首编译后生成 `MediaDao_Impl`，偶发 IDE 索引滞后。

### Q: 模拟器有声音真机没有？
部分国产 ROM 的媒体音量通道被"静音模式"接管。检查 `adb shell media volume --stream 3 --get`。
