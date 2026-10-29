// playerService：Web 端的"心脏"。
// 对应书中 §04 阶段③：播放控制与状态管理。
//
// 架构篇 §1.2 的 ③ PlayerService 层 + ④ ViewModel 层在 Web 端合二为一：
// Pinia store 自身既是状态持有者，又是 <audio>/<video> 的指挥者。
//
// 四个关键取舍（文档"灵魂三问"的落地）：
//   1. <audio> 元素由 store 用 shallowRef + markRaw 持有，不进响应式代理（代价见注释）
//   2. 事件翻译在 attachAudioElement 里一次性接完，store 外无需关心 DOM
//   3. 进度条顺滑靠 rAF 插值；timeupdate 只做校准
//   4. play() 的 Promise 必须 catch——否则一次拒绝就卡死 UI（§04 BUG 现场）
//
// 2026-10-29 修复：
//   以前 loadIntoElement 只从 item.file 读 File → 从 db reload 回来的 items 无 file → 判为"文件已失效"
//   现在优先从 fileCache 内存池取 File，自动 hydrate；真的没有才提示"重新导入"

import { defineStore } from 'pinia'
import { shallowRef, markRaw, ref, computed, watch } from 'vue'
import type { MediaItem, LoopMode, PlaybackStatus } from '@/types'
import { repository } from '@/data/repository'
import { fileCache } from './fileCache'
import { toast } from './toast'

const POSITION_RECORD_INTERVAL = 5000  // 每 5 秒持久化一次断点

export const usePlayerStore = defineStore('player', () => {
  /* ================ state ================ */
  const audioEl = shallowRef<HTMLAudioElement | null>(null)
  const videoEl = shallowRef<HTMLVideoElement | null>(null)

  const current = ref<MediaItem | null>(null)
  const queue = ref<MediaItem[]>([])
  const status = ref<PlaybackStatus>('idle')
  const position = ref(0)           // ms
  const duration = ref(0)           // ms
  const playbackRate = ref(1)
  const volume = ref(0.8)
  const muted = ref(false)
  const loopMode = ref<LoopMode>('order')
  const errorMessage = ref('')
  /** 首次需要用户手势时的提示标志（autoplay policy） */
  const needsUserGesture = ref(false)

  // 可见与否：用来做"视频页/音频页"的渲染层切换
  const mediaType = computed(() => current.value?.type ?? 'audio')
  const isPlaying = computed(() => status.value === 'playing')
  const favoriteOf = ref<Record<string, boolean>>({})

  // 内部辅助
  let rafId = 0
  let intervalId: number | null = null
  /** 进度条拖动中标志位：拖动期间不被引擎覆盖（五端同款坑） */
  const seeking = ref(false)
  let lastRecordedAt = 0

  /** 当前媒体播放对象（<audio> 或 <video>） */
  function activeElement(): HTMLMediaElement | null {
    const t = current.value?.type
    return t === 'video' ? videoEl.value : audioEl.value
  }

  /* ================ element binding ================ */
  function attachAudioElement(el: HTMLAudioElement) {
    audioEl.value = markRaw(el)
    bindEvents(el)
    applyVolume()
    applyRate()
  }

  function attachVideoElement(el: HTMLVideoElement) {
    videoEl.value = markRaw(el)
    bindEvents(el)
    applyVolume()
    applyRate()
  }

  function bindEvents(el: HTMLMediaElement) {
    el.addEventListener('loadedmetadata', () => {
      duration.value = Math.floor(el.duration * 1000) || current.value?.duration || 0
    })
    el.addEventListener('canplay', () => {
      if (status.value === 'loading') status.value = 'ready'
    })
    el.addEventListener('play', () => { status.value = 'playing'; needsUserGesture.value = false; startTicker() })
    el.addEventListener('pause', () => {
      if (status.value !== 'ended') status.value = 'paused'
      stopTicker()
    })
    el.addEventListener('waiting', () => { status.value = 'loading' })
    el.addEventListener('playing', () => { status.value = 'playing' })
    el.addEventListener('timeupdate', () => {
      if (!seeking.value) position.value = Math.floor(el.currentTime * 1000)
      maybeRecordPosition()
    })
    el.addEventListener('ended', () => {
      status.value = 'ended'
      stopTicker()
      next(true)
    })
    el.addEventListener('error', () => {
      const err = el.error
      const code = err?.code
      const codeMap: Record<number, string> = {
        1: '加载已被中断',
        2: '网络错误',
        3: '解码失败（格式可能不被支持）',
        4: '媒体源不可用',
      }
      errorMessage.value = code ? codeMap[code] ?? `媒体错误 ${code}` : '未知错误'
      toast.error(errorMessage.value)
      status.value = 'error'
      stopTicker()
    })
  }

  /** rAF 平滑进度条（timeupdate 只有 4Hz） */
  function startTicker() {
    stopTicker()
    const tick = () => {
      const el = activeElement()
      if (el && !el.paused && !seeking.value) {
        position.value = Math.floor(el.currentTime * 1000)
      }
      rafId = requestAnimationFrame(tick)
    }
    rafId = requestAnimationFrame(tick)

    // 兜底 interval：后台标签页时 rAF 停工（§06 阶段⑤考点）
    if (intervalId !== null) clearInterval(intervalId)
    intervalId = window.setInterval(() => {
      const el = activeElement()
      if (document.hidden && el && !el.paused && !seeking.value) {
        position.value = Math.floor(el.currentTime * 1000)
      }
    }, 1000)
  }

  function stopTicker() {
    if (rafId) { cancelAnimationFrame(rafId); rafId = 0 }
    if (intervalId !== null) { clearInterval(intervalId); intervalId = null }
  }

  /* ================ 操作 API ================ */
  async function initialize() {
    const s = await repository.loadSettings()
    volume.value = s.volume
    loopMode.value = s.loopMode
    playbackRate.value = s.playbackRate
    // 恢复队列（但不自动播放——浏览器自动播放策略禁止无手势起播）
    if (s.lastQueueIds.length) {
      const items = await Promise.all(s.lastQueueIds.map(id => repository.getMedia(id)))
      queue.value = items.filter((x): x is MediaItem => !!x)
      if (s.lastMediaId) {
        const cur = queue.value.find(m => m.id === s.lastMediaId)
        if (cur) {
          current.value = cur
          const lastPos = await repository.getLastPosition(cur.id)
          position.value = lastPos
          // 从 fileCache 补 file（关浏览器后 cache 为空 → 保持错误态由用户感知）
          const hydrated = fileCache.hydrate(cur)
          if (hydrated.file) {
            loadIntoElement(hydrated, /*autoplay*/ false, lastPos)
          }
        }
      }
    }
    applyVolume()
    applyRate()
  }

  /** 替换队列并指定起播 index */
  async function loadQueue(items: MediaItem[], startIndex = 0, autoplay = true) {
    queue.value = items
    const cur = items[startIndex] ?? null
    current.value = cur
    if (cur) {
      position.value = 0
      loadIntoElement(cur, autoplay)
      await persistQueue()
    }
  }

  function loadIntoElement(item: MediaItem, autoplay: boolean, startMs = 0) {
    const el = item.type === 'video' ? videoEl.value : audioEl.value
    if (!el) return

    // 释放上一次的 ObjectURL（防内存泄漏，§03 BUG 现场）
    try {
      if (el.src && el.src.startsWith('blob:')) URL.revokeObjectURL(el.src)
    } catch {/* noop */}

    // 关键：优先从 fileCache 内存池取 File——store 外统一入口
    const effectiveFile = item.file ?? fileCache.get(item.id)

    let src = ''
    if (effectiveFile) {
      src = URL.createObjectURL(effectiveFile)
      // 顺手把 File 挂回 item，避免下次又走 cache 查询
      item.file = effectiveFile
    } else if (item.externalUrl) {
      src = item.externalUrl
    } else {
      // 没文件、也没 URL：触发错误态
      status.value = 'error'
      errorMessage.value = '文件已失效，请重新导入此曲目'
      toast.warn(`"${item.title}" 文件已失效，请重新导入`)
      return
    }

    status.value = 'loading'
    duration.value = item.duration
    el.src = src
    el.load()

    // 断点续播：loadedmetadata 后再 seek 一次
    if (startMs > 0) {
      const onReady = () => {
        try { el.currentTime = startMs / 1000 } catch {/* noop */}
        el.removeEventListener('loadedmetadata', onReady)
      }
      el.addEventListener('loadedmetadata', onReady)
    }

    if (autoplay) {
      // play() 返回 Promise——必须 catch，不然浏览器拒绝一次就卡死
      el.play().catch(err => {
        status.value = 'paused'
        if (err?.name === 'NotAllowedError') {
          needsUserGesture.value = true
          toast.info('浏览器拦截了自动播放，点一下播放按钮即可', 4200)
        } else if (err?.name !== 'AbortError') {
          errorMessage.value = `播放失败：${err?.message ?? err}`
          toast.error(errorMessage.value)
        }
      })
    }
  }

  function play() {
    const el = activeElement()
    if (!el || !current.value) return
    // 若还没 src（恢复队列但未补 file 的情况），再尝试一次
    if (!el.src) {
      loadIntoElement(current.value, true)
      return
    }
    el.play().catch(err => {
      status.value = 'paused'
      if (err?.name === 'NotAllowedError') {
        needsUserGesture.value = true
        toast.info('浏览器拦截了自动播放，再点一次播放即可', 3200)
      } else if (err?.name !== 'AbortError') {
        errorMessage.value = `播放失败：${err?.message ?? err}`
        toast.error(errorMessage.value)
      }
    })
  }

  function pause() {
    activeElement()?.pause()
  }

  function togglePlay() {
    isPlaying.value ? pause() : play()
  }

  /** 跳转到指定毫秒 */
  function seek(ms: number) {
    const el = activeElement()
    if (!el) return
    const clamped = Math.max(0, Math.min(ms, duration.value))
    el.currentTime = clamped / 1000
    position.value = clamped
  }

  function setPlaybackRate(rate: number) {
    playbackRate.value = Math.max(0.25, Math.min(4, rate))
    applyRate()
    repository.saveSettings({ playbackRate: playbackRate.value })
  }

  function setVolume(v: number) {
    volume.value = Math.max(0, Math.min(1, v))
    if (muted.value && v > 0) muted.value = false   // 拖动音量自动取消静音
    applyVolume()
    repository.saveSettings({ volume: volume.value })
  }

  function toggleMute() {
    muted.value = !muted.value
    applyVolume()
  }

  function setLoopMode(mode: LoopMode) {
    loopMode.value = mode
    repository.saveSettings({ loopMode: mode })
  }

  function applyVolume() {
    for (const el of [audioEl.value, videoEl.value]) {
      if (!el) continue
      el.volume = volume.value
      el.muted = muted.value
    }
  }

  function applyRate() {
    for (const el of [audioEl.value, videoEl.value]) {
      if (el) el.playbackRate = playbackRate.value
    }
  }

  /** 上一首 / 下一首：架构篇 §1.5 的决策树 */
  function next(auto = false) {
    if (!current.value) return
    const idx = queue.value.findIndex(m => m.id === current.value!.id)
    if (idx < 0 || queue.value.length === 0) return

    let nextIdx: number
    if (auto && loopMode.value === 'single') {
      // 单曲循环：重放
      nextIdx = idx
    } else if (loopMode.value === 'random') {
      if (queue.value.length === 1) nextIdx = 0
      else {
        do { nextIdx = Math.floor(Math.random() * queue.value.length) }
        while (nextIdx === idx)
      }
    } else {
      nextIdx = (idx + 1) % queue.value.length
    }

    const target = queue.value[nextIdx]
    current.value = target
    position.value = 0
    loadIntoElement(target, true)
    persistQueue()
  }

  function previous() {
    if (!current.value) return
    const idx = queue.value.findIndex(m => m.id === current.value!.id)
    if (idx < 0) return
    // 大于 3 秒的时候，上一首变"回到开头"（音乐软件惯例）
    if (position.value > 3000) {
      seek(0)
      return
    }
    const prevIdx = (idx - 1 + queue.value.length) % queue.value.length
    const target = queue.value[prevIdx]
    current.value = target
    position.value = 0
    loadIntoElement(target, true)
    persistQueue()
  }

  /** 从队列移除某项（如果是当前曲目则切下一首） */
  function removeFromQueue(id: string) {
    const idx = queue.value.findIndex(m => m.id === id)
    if (idx < 0) return
    const isCurrent = current.value?.id === id
    queue.value.splice(idx, 1)
    if (isCurrent) {
      if (queue.value.length === 0) {
        pause()
        current.value = null
      } else {
        const nextIdx = Math.min(idx, queue.value.length - 1)
        const target = queue.value[nextIdx]
        current.value = target
        position.value = 0
        loadIntoElement(target, isPlaying.value)
      }
    }
    persistQueue()
  }

  /** 清空队列 */
  function clearQueue() {
    pause()
    queue.value = []
    current.value = null
    persistQueue()
  }

  async function toggleFavorite() {
    if (!current.value) return
    const favored = await repository.toggleFavorite(current.value.id)
    favoriteOf.value[current.value.id] = favored
    toast.success(favored ? '已加入收藏' : '已取消收藏', 1600)
  }

  async function maybeRecordPosition() {
    if (!current.value) return
    const now = Date.now()
    if (now - lastRecordedAt < POSITION_RECORD_INTERVAL) return
    lastRecordedAt = now
    await repository.recordPlay(current.value.id, position.value)
    await repository.saveSettings({ lastMediaId: current.value.id })
  }

  async function persistQueue() {
    await repository.saveSettings({
      lastQueueIds: queue.value.map(m => m.id),
      lastMediaId: current.value?.id,
    })
  }

  // 当前曲目变化 → 同步 favorite 缓存
  watch(current, async (cur) => {
    if (!cur) return
    favoriteOf.value[cur.id] = await repository.isFavorite(cur.id)
  })

  return {
    // state
    audioEl, videoEl,
    current, queue, status, position, duration,
    playbackRate, volume, muted, loopMode, errorMessage,
    seeking, favoriteOf, needsUserGesture,
    // getters
    mediaType, isPlaying,
    // actions
    attachAudioElement, attachVideoElement,
    initialize, loadQueue,
    play, pause, togglePlay, seek,
    setPlaybackRate, setVolume, toggleMute, setLoopMode,
    next, previous, toggleFavorite,
    removeFromQueue, clearQueue,
  }
})

export type PlayerStore = ReturnType<typeof usePlayerStore>
