<!-- 底部播放控制条：无论在哪个页面都显示。
     对应书中 §04 Step 4.5 + §04 Step 4.6 (音量/静音/全屏)。 -->
<script setup lang="ts">
import { computed } from 'vue'
import { useRouter } from 'vue-router'
import { usePlayerStore } from '@/service/playerService'
import { formatTime } from '@/utils/format'
import ProgressBar from './ProgressBar.vue'

const player = usePlayerStore()
const router = useRouter()

const hasMedia = computed(() => !!player.current)
const isFav = computed(() =>
  player.current ? !!player.favoriteOf[player.current.id] : false,
)

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

function openPlaying() { router.push('/playing') }

function toggleMuteClick(e: Event) {
  e.stopPropagation()
  player.toggleMute()
}
</script>

<template>
  <footer class="player-bar">
    <div class="info" @click="openPlaying">
      <div class="cover">
        <img v-if="player.current?.coverUrl" :src="player.current.coverUrl" alt="" />
        <span v-else>♪</span>
      </div>
      <div class="meta">
        <div class="title">{{ player.current?.title ?? '未选择曲目' }}</div>
        <div class="artist">{{ player.current?.artist ?? '—' }}</div>
      </div>
    </div>

    <div class="controls">
      <div class="buttons">
        <button :disabled="!hasMedia" @click="cycleLoop" :title="player.loopMode">{{ loopIcon }}</button>
        <button :disabled="!hasMedia" @click="player.previous">⏮</button>
        <button class="play" :disabled="!hasMedia" @click="player.togglePlay">
          {{ player.isPlaying ? '⏸' : '▶' }}
        </button>
        <button :disabled="!hasMedia" @click="player.next(false)">⏭</button>
        <button :disabled="!hasMedia" :class="{ fav: isFav }" @click="player.toggleFavorite">♥</button>
      </div>
      <div class="progress-wrap">
        <span class="time">{{ formatTime(player.position) }}</span>
        <ProgressBar
          :position="player.position"
          :duration="player.duration"
          :disabled="!hasMedia"
          @seeking="player.seeking = $event"
          @seek="player.seek"
        />
        <span class="time">{{ formatTime(player.duration) }}</span>
      </div>
    </div>

    <div class="volume">
      <button class="mute" @click="toggleMuteClick">
        {{ player.muted ? '🔇' : player.volume > 0.5 ? '🔊' : player.volume > 0 ? '🔉' : '🔈' }}
      </button>
      <input
        type="range"
        min="0" max="1" step="0.01"
        :value="player.muted ? 0 : player.volume"
        @input="player.setVolume(Number(($event.target as HTMLInputElement).value))"
      />
    </div>
  </footer>
</template>

<style scoped>
.player-bar {
  grid-column: 1 / -1;
  grid-row: 2 / 3;
  display: grid;
  grid-template-columns: 280px 1fr 200px;
  align-items: center;
  padding: 10px 20px;
  gap: 20px;
  background: var(--playerbar-bg);
  border-top: 1px solid var(--border);
  backdrop-filter: blur(10px);
  height: var(--playerbar-h);
}
.info {
  display: flex;
  align-items: center;
  gap: 12px;
  cursor: pointer;
  overflow: hidden;
}
.cover {
  width: 54px; height: 54px;
  border-radius: 8px;
  background: var(--hover);
  display: flex; align-items: center; justify-content: center;
  overflow: hidden; flex-shrink: 0;
  font-size: 22px; color: var(--text-mute);
}
.cover img { width: 100%; height: 100%; object-fit: cover; }
.meta { overflow: hidden; }
.title {
  font-weight: 600;
  color: var(--text);
  white-space: nowrap; text-overflow: ellipsis; overflow: hidden;
}
.artist { color: var(--text-mute); font-size: 12px; }

.controls { display: flex; flex-direction: column; gap: 6px; }
.buttons {
  display: flex; justify-content: center; align-items: center; gap: 14px;
}
.buttons button {
  color: var(--text-sub);
  font-size: 18px;
  padding: 6px 8px;
  border-radius: 50%;
  transition: all .15s;
}
.buttons button:hover:not(:disabled) { color: var(--accent); background: var(--hover); }
.buttons button:disabled { opacity: .35; cursor: not-allowed; }
.buttons .play {
  font-size: 20px;
  background: var(--accent);
  color: #fff;
  width: 40px; height: 40px;
}
.buttons .play:hover:not(:disabled) { background: var(--accent-strong); color: #fff; }
.buttons .fav { color: var(--danger); }

.progress-wrap {
  display: flex; align-items: center; gap: 10px;
  font-size: 12px;
}
.time { color: var(--text-mute); min-width: 42px; text-align: center; font-variant-numeric: tabular-nums; }

.volume {
  display: flex; align-items: center; gap: 8px;
  justify-self: end;
}
.mute { font-size: 18px; color: var(--text-sub); }
.volume input { width: 110px; }

@media (max-width: 640px) {
  .player-bar {
    grid-template-columns: 1fr auto;
    grid-template-rows: auto auto;
    padding: 8px 12px;
    height: auto;
    gap: 8px;
  }
  .info { grid-column: 1 / 2; }
  .volume { grid-column: 2 / 3; grid-row: 1 / 2; }
  .volume input { width: 70px; }
  .controls { grid-column: 1 / -1; grid-row: 2 / 3; }
}
</style>
