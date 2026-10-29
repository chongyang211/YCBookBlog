<!-- 底部播放控制条：无论在哪个页面都显示。
     对应书中 §04 Step 4.5 + §04 Step 4.6 (音量/静音/全屏)
     手机端：隐藏音量条、简化按钮，点击左侧封面区进入全屏播放页 -->
<script setup lang="ts">
import { computed, ref } from 'vue'
import { useRouter } from 'vue-router'
import { usePlayerStore } from '@/service/playerService'
import { formatTime } from '@/utils/format'
import ProgressBar from './ProgressBar.vue'
import CoverArt from './CoverArt.vue'
import QueueDrawer from './QueueDrawer.vue'

const player = usePlayerStore()
const router = useRouter()
const queueOpen = ref(false)

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
const loopLabel = computed(() => {
  switch (player.loopMode) {
    case 'single': return '单曲循环'
    case 'random': return '随机播放'
    default: return '顺序循环'
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

function openQueue(e: Event) {
  e.stopPropagation()
  queueOpen.value = true
}
</script>

<template>
  <footer class="player-bar glass">
    <div class="info" @click="openPlaying">
      <CoverArt
        :src="player.current?.coverUrl"
        :id="player.current?.id"
        :size="54"
      />
      <div class="meta">
        <div class="title ellipsis">{{ player.current?.title ?? '未选择曲目' }}</div>
        <div class="artist ellipsis">{{ player.current?.artist ?? '从媒体库选一首歌开始播放' }}</div>
      </div>
      <button
        v-if="hasMedia"
        class="fav-mini"
        :class="{ fav: isFav }"
        @click.stop="player.toggleFavorite"
        :title="isFav ? '取消收藏' : '加入收藏'"
      >
        <span v-if="isFav">♥</span>
        <span v-else>♡</span>
      </button>
    </div>

    <div class="controls">
      <div class="buttons">
        <button class="icon-btn" :disabled="!hasMedia" @click="cycleLoop" :title="loopLabel">
          {{ loopIcon }}
        </button>
        <button class="icon-btn" :disabled="!hasMedia" @click="player.previous" title="上一首">
          ⏮
        </button>
        <button
          class="play"
          :disabled="!hasMedia"
          @click="player.togglePlay"
          :title="player.isPlaying ? '暂停' : '播放'"
        >
          <span v-if="player.status === 'loading'" class="spinner" />
          <span v-else-if="player.isPlaying">⏸</span>
          <span v-else>▶</span>
        </button>
        <button class="icon-btn" :disabled="!hasMedia" @click="player.next(false)" title="下一首">
          ⏭
        </button>
        <button class="icon-btn" :disabled="!hasMedia" @click="openQueue" title="播放队列">
          ☰
        </button>
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
      <button class="icon-btn mute" @click="toggleMuteClick" :title="player.muted ? '取消静音' : '静音'">
        {{ player.muted ? '🔇' : player.volume > 0.5 ? '🔊' : player.volume > 0 ? '🔉' : '🔈' }}
      </button>
      <input
        type="range"
        min="0" max="1" step="0.01"
        :value="player.muted ? 0 : player.volume"
        @input="player.setVolume(Number(($event.target as HTMLInputElement).value))"
        aria-label="音量"
      />
    </div>

    <QueueDrawer :open="queueOpen" @close="queueOpen = false" />
  </footer>
</template>

<style scoped>
.player-bar {
  position: fixed;
  left: 0; right: 0; bottom: 0;
  z-index: 40;
  display: grid;
  grid-template-columns: 300px 1fr 220px;
  align-items: center;
  padding: 10px 20px;
  gap: 24px;
  background: var(--playerbar-bg);
  backdrop-filter: blur(var(--glass-blur)) saturate(var(--glass-saturate));
  -webkit-backdrop-filter: blur(var(--glass-blur)) saturate(var(--glass-saturate));
  border-top: 1px solid var(--border);
  height: var(--playerbar-h);
}

/* ---- 左：曲目信息 ---- */
.info {
  display: flex;
  align-items: center;
  gap: var(--space-3);
  cursor: pointer;
  overflow: hidden;
  min-width: 0;
}
.info:hover .title { color: var(--accent); }
.meta { overflow: hidden; min-width: 0; flex: 1; }
.title {
  font-weight: 600;
  color: var(--text);
  font-size: var(--fs-md);
  transition: color .15s;
}
.artist { color: var(--text-mute); font-size: var(--fs-sm); margin-top: 2px; }
.fav-mini {
  width: 36px; height: 36px;
  border-radius: 50%;
  color: var(--text-mute);
  font-size: 18px;
  flex-shrink: 0;
}
.fav-mini:hover { background: var(--hover); color: var(--danger); }
.fav-mini.fav { color: var(--danger); }

/* ---- 中：控制 + 进度 ---- */
.controls { display: flex; flex-direction: column; align-items: stretch; gap: 6px; min-width: 0; }
.buttons {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 10px;
}
.icon-btn {
  width: 36px; height: 36px;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  color: var(--text-sub);
  font-size: 16px;
  border-radius: 50%;
  transition: all .15s;
}
.icon-btn:hover:not(:disabled) { color: var(--text); background: var(--hover); }
.icon-btn:disabled { opacity: .35; cursor: not-allowed; }

.play {
  width: 42px; height: 42px;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  background: linear-gradient(135deg, var(--brand-1), var(--brand-2));
  color: #fff;
  font-size: 16px;
  border-radius: 50%;
  box-shadow: 0 6px 18px rgba(139, 92, 246, 0.4);
  transition: all .18s;
}
.play:hover:not(:disabled) {
  transform: scale(1.06);
  box-shadow: 0 10px 24px rgba(139, 92, 246, 0.55);
}
.play:active:not(:disabled) { transform: scale(.97); }
.play:disabled { opacity: .45; box-shadow: none; }

.spinner {
  display: inline-block;
  width: 16px; height: 16px;
  border: 2px solid rgba(255, 255, 255, 0.4);
  border-top-color: #fff;
  border-radius: 50%;
  animation: spin .8s linear infinite;
}

.progress-wrap {
  display: flex;
  align-items: center;
  gap: 10px;
  font-size: var(--fs-xs);
}
.time {
  color: var(--text-mute);
  min-width: 44px;
  text-align: center;
  font-variant-numeric: tabular-nums;
}

/* ---- 右：音量 ---- */
.volume {
  display: flex;
  align-items: center;
  gap: 8px;
  justify-self: end;
  min-width: 0;
}
.volume input { width: 110px; }
.mute { width: 32px; height: 32px; font-size: 16px; }

/* ---- 响应式：平板 ---- */
@media (max-width: 1024px) {
  .player-bar { grid-template-columns: 260px 1fr 170px; gap: 16px; }
  .volume input { width: 80px; }
}

/* ---- 响应式：手机 ---- */
@media (max-width: 640px) {
  .player-bar {
    grid-template-columns: 1fr auto;
    grid-template-rows: auto auto;
    padding: 8px 10px;
    height: auto;
    gap: 6px;
    bottom: var(--mobile-nav-h);
  }
  .info { grid-column: 1 / 2; grid-row: 1 / 2; }
  .info .cover-art { width: 42px !important; height: 42px !important; font-size: 18px !important; }
  .title { font-size: var(--fs-base); }
  .artist { font-size: var(--fs-xs); }
  .fav-mini { display: none; }

  .controls { grid-column: 1 / -1; grid-row: 2 / 3; gap: 2px; }
  .buttons { gap: 4px; }
  .icon-btn { width: 32px; height: 32px; font-size: 15px; }
  .play { width: 38px; height: 38px; }
  .time { min-width: 36px; font-size: 10px; }
  .progress-wrap { gap: 6px; }

  .volume {
    grid-column: 2 / 3; grid-row: 1 / 2;
    justify-self: end;
  }
  .volume input { display: none; }
  .mute { width: 36px; height: 36px; }
}
</style>
