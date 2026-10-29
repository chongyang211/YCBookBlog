<!-- 正在播放页：封面大图 + 歌词
     对应 §07 阶段⑥。视频模式下让渲染层接管全屏区域（<video> 在根组件；本页压轴露出字幕/控制）
     桌面端：左封面 + 右歌词 两栏
     手机端：上封面小图 + 下歌词 单栏 -->
<script setup lang="ts">
import { computed } from 'vue'
import { usePlayerStore } from '@/service/playerService'
import { formatTime } from '@/utils/format'
import LyricsView from '@/ui/components/LyricsView.vue'
import ProgressBar from '@/ui/components/ProgressBar.vue'
import CoverArt from '@/ui/components/CoverArt.vue'
import { gradientFromId } from '@/utils/colorFromId'

const player = usePlayerStore()

const bgStyle = computed(() => {
  const url = player.current?.coverUrl
  if (url) return { backgroundImage: `url(${url})` }
  const id = player.current?.id ?? 'default'
  return { background: gradientFromId(id) }
})

const isFav = computed(() =>
  player.current ? !!player.favoriteOf[player.current.id] : false,
)

function toggleFullscreen() {
  const el = player.videoEl
  if (!el) return
  if (document.fullscreenElement) document.exitFullscreen()
  else el.requestFullscreen().catch(() => {/* noop */})
}

const loopIcon = computed(() => {
  switch (player.loopMode) {
    case 'single': return '🔂'
    case 'random': return '🔀'
    default: return '🔁'
  }
})

function cycleLoop() {
  const order: Array<'order' | 'single' | 'random'> = ['order', 'single', 'random']
  const idx = order.indexOf(player.loopMode)
  player.setLoopMode(order[(idx + 1) % order.length])
}
</script>

<template>
  <section class="playing" v-if="player.current">
    <!-- 背景大图模糊层 -->
    <div class="bg" :style="bgStyle" />
    <div class="mask" />

    <div class="content">
      <!-- 左：封面 + 曲目信息 + 控制 -->
      <div class="cover-area">
        <CoverArt
          class="big-cover"
          :src="player.current.coverUrl"
          :id="player.current.id"
          :size="300"
          :spinning="player.isPlaying && player.current.type === 'audio'"
          :circle="player.current.type === 'audio'"
        />
        <div class="meta">
          <h2 class="track-title" :title="player.current.title">{{ player.current.title }}</h2>
          <p class="track-sub">
            <span>{{ player.current.artist }}</span>
            <span v-if="player.current.album" class="dot-sep">·</span>
            <span v-if="player.current.album">{{ player.current.album }}</span>
          </p>
        </div>

        <!-- 大进度条 -->
        <div class="progress-row">
          <span class="time">{{ formatTime(player.position) }}</span>
          <ProgressBar
            tall
            :position="player.position"
            :duration="player.duration"
            @seeking="player.seeking = $event"
            @seek="player.seek"
          />
          <span class="time">{{ formatTime(player.duration) }}</span>
        </div>

        <!-- 控制按钮组 -->
        <div class="controls">
          <button class="icon-btn" @click="cycleLoop" :title="player.loopMode">{{ loopIcon }}</button>
          <button class="icon-btn" @click="player.previous" title="上一首">⏮</button>
          <button class="play-main" @click="player.togglePlay">
            <span v-if="player.status === 'loading'" class="spinner" />
            <span v-else-if="player.isPlaying">⏸</span>
            <span v-else>▶</span>
          </button>
          <button class="icon-btn" @click="player.next(false)" title="下一首">⏭</button>
          <button
            class="icon-btn"
            :class="{ fav: isFav }"
            @click="player.toggleFavorite"
            :title="isFav ? '取消收藏' : '加入收藏'"
          >
            <span v-if="isFav">♥</span><span v-else>♡</span>
          </button>
        </div>

        <button
          v-if="player.current.type === 'video'"
          class="fs-btn"
          @click="toggleFullscreen"
        >⛶ 视频全屏</button>
      </div>

      <!-- 右：歌词 -->
      <div class="lyrics-area">
        <LyricsView />
      </div>
    </div>
  </section>

  <section v-else class="empty">
    <div class="empty-icon">♪</div>
    <h3>还没有在播曲目</h3>
    <p>从媒体库选一首歌开始播放</p>
    <RouterLink to="/library/all" class="btn-link">
      前往媒体库
      <span>→</span>
    </RouterLink>
  </section>
</template>

<style scoped>
.playing {
  position: relative;
  height: 100%;
  min-height: 500px;
  overflow: hidden;
}
.bg {
  position: absolute; inset: 0;
  background-size: cover;
  background-position: center;
  filter: blur(80px) saturate(1.4);
  transform: scale(1.3);
  z-index: 0;
}
.mask {
  position: absolute; inset: 0;
  background: linear-gradient(180deg,
    rgba(var(--bg-rgb, 15, 23, 42), 0.55) 0%,
    rgba(var(--bg-rgb, 15, 23, 42), 0.85) 100%);
  background: var(--bg);
  opacity: .68;
  z-index: 1;
}

.content {
  position: relative;
  z-index: 2;
  display: grid;
  grid-template-columns: 1.1fr 1fr;
  gap: 50px;
  padding: 40px 60px;
  height: 100%;
  min-height: 500px;
  box-sizing: border-box;
  align-items: center;
}

.cover-area {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 24px;
  max-width: 480px;
  width: 100%;
  margin: 0 auto;
}
.big-cover {
  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.35) !important;
}

.meta { text-align: center; max-width: 100%; }
.track-title {
  margin: 0;
  font-size: var(--fs-2xl);
  line-height: 1.2;
  overflow: hidden;
  text-overflow: ellipsis;
  display: -webkit-box;
  -webkit-line-clamp: 2;
  -webkit-box-orient: vertical;
}
.track-sub {
  margin-top: 8px;
  color: var(--text-sub);
  font-size: var(--fs-md);
  display: flex;
  justify-content: center;
  gap: 8px;
  flex-wrap: wrap;
}
.dot-sep { color: var(--text-mute); }

.progress-row {
  display: flex;
  align-items: center;
  gap: 10px;
  width: 100%;
  font-size: var(--fs-xs);
}
.time {
  color: var(--text-sub);
  min-width: 44px;
  text-align: center;
  font-variant-numeric: tabular-nums;
}

.controls {
  display: flex;
  align-items: center;
  gap: 14px;
}
.icon-btn {
  width: 48px; height: 48px;
  border-radius: 50%;
  color: var(--text-sub);
  font-size: 20px;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  transition: all .15s;
}
.icon-btn:hover { background: var(--hover); color: var(--text); }
.icon-btn.fav { color: var(--danger); }

.play-main {
  width: 68px; height: 68px;
  border-radius: 50%;
  background: linear-gradient(135deg, var(--brand-1), var(--brand-2));
  color: #fff;
  font-size: 24px;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  box-shadow: 0 10px 32px rgba(139, 92, 246, 0.45);
  transition: all .2s;
}
.play-main:hover {
  transform: scale(1.06);
  box-shadow: 0 16px 40px rgba(139, 92, 246, 0.6);
}
.play-main:active { transform: scale(.96); }

.spinner {
  display: inline-block;
  width: 24px; height: 24px;
  border: 3px solid rgba(255, 255, 255, 0.4);
  border-top-color: #fff;
  border-radius: 50%;
  animation: spin .9s linear infinite;
}

.fs-btn {
  margin-top: 12px;
  padding: 8px 18px;
  background: var(--accent-soft);
  border: 1px solid var(--accent);
  color: var(--accent);
  border-radius: var(--radius-full);
  font-size: var(--fs-sm);
  transition: all .18s;
}
.fs-btn:hover {
  background: var(--accent);
  color: #fff;
}

.lyrics-area {
  display: flex;
  flex-direction: column;
  justify-content: center;
  height: 100%;
  min-height: 400px;
}

/* --- 空态 --- */
.empty {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  height: 100%;
  min-height: 500px;
  gap: 10px;
  color: var(--text-sub);
}
.empty-icon {
  font-size: 72px;
  color: var(--text-mute);
  opacity: .4;
  margin-bottom: 10px;
}
.empty h3 { color: var(--text); margin: 0; }
.btn-link {
  margin-top: 16px;
  padding: 10px 24px;
  background: linear-gradient(135deg, var(--brand-1), var(--brand-2));
  color: #fff;
  border-radius: var(--radius-full);
  font-weight: 500;
  display: inline-flex;
  align-items: center;
  gap: 8px;
  transition: all .18s;
  box-shadow: 0 6px 18px rgba(139, 92, 246, 0.4);
}
.btn-link:hover { transform: translateY(-1px); box-shadow: 0 10px 24px rgba(139, 92, 246, 0.55); }

/* --- 响应式 --- */
@media (max-width: 1024px) {
  .content { padding: 30px 40px; gap: 30px; }
  .big-cover { width: 240px !important; height: 240px !important; }
}
@media (max-width: 820px) {
  .content {
    grid-template-columns: 1fr;
    grid-template-rows: auto 1fr;
    padding: 20px;
    gap: 20px;
  }
  .big-cover { width: 200px !important; height: 200px !important; }
  .track-title { font-size: var(--fs-xl); }
  .play-main { width: 56px; height: 56px; font-size: 20px; }
  .icon-btn { width: 40px; height: 40px; font-size: 17px; }
  .lyrics-area { min-height: 240px; }
}
@media (max-width: 480px) {
  .content { padding: 16px; }
  .big-cover { width: 180px !important; height: 180px !important; }
  .controls { gap: 10px; }
}
</style>
