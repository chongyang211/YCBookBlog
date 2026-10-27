<!-- 正在播放页：封面大图 + 歌词
     对应 §07 阶段⑥。视频模式下让渲染层接管全屏区域（<video> 在根组件；本页压轴露出字幕/控制） -->
<script setup lang="ts">
import { computed } from 'vue'
import { usePlayerStore } from '@/service/playerService'
import { formatTime } from '@/utils/format'
import LyricsView from '@/ui/components/LyricsView.vue'

const player = usePlayerStore()

const bgStyle = computed(() => {
  const url = player.current?.coverUrl
  return url
    ? { backgroundImage: `url(${url})` }
    : { background: 'linear-gradient(135deg, var(--accent) 0%, var(--accent-strong) 100%)' }
})

function toggleFullscreen() {
  const el = player.videoEl
  if (!el) return
  if (document.fullscreenElement) document.exitFullscreen()
  else el.requestFullscreen().catch(() => {/* noop */})
}
</script>

<template>
  <section class="playing" v-if="player.current">
    <div class="bg" :style="bgStyle" />
    <div class="mask" />

    <div class="content">
      <div class="cover-box">
        <div class="cover" :class="{ playing: player.isPlaying }">
          <img v-if="player.current.coverUrl" :src="player.current.coverUrl" alt="" />
          <span v-else>♪</span>
        </div>
        <div class="meta">
          <h2>{{ player.current.title }}</h2>
          <p>{{ player.current.artist }} · {{ player.current.album }}</p>
          <p class="time">{{ formatTime(player.position) }} / {{ formatTime(player.duration) }}</p>
          <button
            v-if="player.current.type === 'video'"
            class="fs-btn"
            @click="toggleFullscreen"
          >⛶ 全屏</button>
        </div>
      </div>

      <div class="lyrics-box">
        <LyricsView />
      </div>
    </div>
  </section>
  <section v-else class="empty">
    <p>还没有在播曲目</p>
    <RouterLink to="/library/all" class="link">去媒体库选一首</RouterLink>
  </section>
</template>

<style scoped>
.playing {
  position: relative;
  height: 100%;
  overflow: hidden;
}
.bg {
  position: absolute; inset: 0;
  background-size: cover;
  background-position: center;
  filter: blur(60px) saturate(1.2);
  transform: scale(1.1);
  z-index: 0;
}
.mask {
  position: absolute; inset: 0;
  background: var(--bg);
  opacity: .78;
  z-index: 1;
}
.content {
  position: relative;
  z-index: 2;
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 40px;
  padding: 40px;
  height: 100%;
  box-sizing: border-box;
}
.cover-box {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 24px;
}
.cover {
  width: 280px; height: 280px;
  border-radius: 50%;
  overflow: hidden;
  background: var(--bg-elev);
  display: flex; align-items: center; justify-content: center;
  font-size: 80px; color: var(--text-mute);
  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.35);
  animation: spin 20s linear infinite;
  animation-play-state: paused;
}
.cover.playing { animation-play-state: running; }
.cover img { width: 100%; height: 100%; object-fit: cover; }
@keyframes spin {
  from { transform: rotate(0deg); }
  to { transform: rotate(360deg); }
}

.meta {
  text-align: center;
}
.meta h2 { margin: 0 0 6px; }
.meta p { margin: 2px 0; color: var(--text-sub); }
.meta .time { color: var(--text-mute); font-size: 12px; font-variant-numeric: tabular-nums; }
.fs-btn {
  margin-top: 12px;
  padding: 6px 14px;
  background: var(--accent);
  color: #fff;
  border-radius: 999px;
  font-size: 13px;
}

.lyrics-box {
  display: flex;
  flex-direction: column;
  justify-content: center;
}

.empty {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  height: 100%;
  gap: 10px;
  color: var(--text-sub);
}
.link { color: var(--accent); }

@media (max-width: 820px) {
  .content { grid-template-columns: 1fr; grid-template-rows: auto 1fr; padding: 24px; gap: 24px; }
  .cover { width: 200px; height: 200px; font-size: 60px; }
}
</style>
