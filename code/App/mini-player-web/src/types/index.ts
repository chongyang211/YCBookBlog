// 全局类型定义。
// 对应书中 §01 架构篇 §1.3 数据模型，Web 端的"物理约束"落地：
//  - id 用 string（JS number 不能安全表达 >2^53 的时间戳组合）
//  - 枚举用联合类型，比 TS enum 更贴近 Pinia 的序列化心智
//  - 时间单位一律毫秒（除调引擎 API 时转秒）

export type MediaType = 'audio' | 'video'

/** 架构篇 §1.4 PlayerState 的 Web 端落地 */
export type PlaybackStatus = 'idle' | 'loading' | 'ready' | 'playing' | 'paused' | 'ended' | 'error'

export type LoopMode = 'order' | 'single' | 'random'

/** 一条媒体条目（扫描进来的 File 元信息 + 后续累积的播放统计） */
export interface MediaItem {
  id: string
  type: MediaType
  title: string
  artist: string
  album: string
  /** 时长，单位毫秒 */
  duration: number
  /** 文件名（含扩展名），用来做扩展名/格式识别 */
  filename: string
  /** 文件大小字节 */
  size: number
  /** 文件 MIME，用于按文件头而非扩展名识别 */
  mime: string
  /** 封面的 blob URL（内存里；下次打开会在仓库层按需重建） */
  coverUrl?: string
  /** 封面二进制 (Blob)，持久化字段 */
  cover?: Blob
  /** 添加到库的时间 */
  addedAt: number
  /** 来源：选目录 vs 拖拽 vs URL */
  source: 'directory' | 'dropped' | 'url'
  /** File 对象（内存里，关页面即失效）；持久化靠 fileHandleKey */
  file?: File
  /** File System Access API 的句柄 ID（关联 Dexie 的 fileHandles 表） */
  fileHandleKey?: string
  /** 如果是 URL 来源，这里存 URL */
  externalUrl?: string
}

/** 播放历史记录 */
export interface PlayRecord {
  id: string
  mediaId: string
  position: number  // 毫秒
  playedAt: number
}

export interface Favorite {
  id: string         // 等于 mediaId
  addedAt: number
}

export interface Playlist {
  id: string
  name: string
  mediaIds: string[]
  createdAt: number
}

export interface Rating {
  mediaId: string
  score: 1 | 2 | 3 | 4 | 5
  updatedAt: number
}

/** 持久化的全局设置（EQ 曲线、主题、音量…） */
export interface Settings {
  id: 'global'
  theme: 'auto' | 'light' | 'dark'
  volume: number        // 0-1
  loopMode: LoopMode
  playbackRate: number
  eqEnabled: boolean
  eqGains: number[]     // 10 频段，单位 dB，范围 -12..+12
  eqPreset: string      // 'flat'/'pop'/'rock'/...
  locale: 'zh-CN' | 'en-US'
  lastQueueIds: string[]
  lastMediaId?: string
}

/** LRC 一行 */
export interface LyricLine {
  time: number   // 毫秒
  text: string
}

/** 架构篇 §1.4 PlayerState 的完整快照（Pinia store state 的类型） */
export interface PlayerStateSnapshot {
  current: MediaItem | null
  queue: MediaItem[]
  status: PlaybackStatus
  position: number     // 毫秒
  duration: number     // 毫秒
  playbackRate: number
  volume: number       // 0-1
  muted: boolean
  loopMode: LoopMode
  errorMessage: string
}
