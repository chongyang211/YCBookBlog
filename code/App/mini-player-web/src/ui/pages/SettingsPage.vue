<!-- 设置页：主题 + 均衡器 + 睡眠定时 + 媒体库维护
     对应 §08 阶段⑦ + §09 阶段⑧ -->
<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount } from 'vue'
import { readThemeMode, writeThemeMode, type ThemeMode } from '@/utils/theme'
import { repository } from '@/data/repository'
import { fileCache } from '@/service/fileCache'
import { usePlayerStore } from '@/service/playerService'
import { sleepTimer } from '@/service/sleepTimer'
import { equalizer } from '@/service/equalizer'
import { toast } from '@/service/toast'
import EqualizerPanel from '@/ui/components/EqualizerPanel.vue'

const player = usePlayerStore()
const theme = ref<ThemeMode>(readThemeMode())
const timerMinutes = ref(30)
const timerActive = ref(false)
const timerRemaining = ref('—')
let tick: number | null = null

const mediaCount = ref(0)
const favCount = ref(0)
const historyCount = ref(0)

async function refreshStats() {
  mediaCount.value = (await repository.listMedia()).length
  favCount.value = (await repository.listFavorites()).length
  historyCount.value = (await repository.listHistory(500)).length
}

onMounted(async () => {
  await refreshStats()
  tick = window.setInterval(() => {
    if (sleepTimer.isActive) {
      timerActive.value = true
      const ms = sleepTimer.remainingMs
      const m = Math.floor(ms / 60_000)
      const s = Math.floor((ms % 60_000) / 1000)
      timerRemaining.value = `${m}:${s.toString().padStart(2, '0')}`
    } else {
      timerActive.value = false
      timerRemaining.value = '—'
    }
  }, 1000)
})

onBeforeUnmount(() => {
  if (tick !== null) clearInterval(tick)
})

function pickTheme(mode: ThemeMode) {
  theme.value = mode
  writeThemeMode(mode)
  toast.success(`主题已切换：${mode === 'auto' ? '跟随系统' : mode === 'light' ? '浅色' : '深色'}`, 1600)
}

function startTimer() {
  if (!player.current) {
    toast.warn('请先选择一首歌再启动定时')
    return
  }
  sleepTimer.start(timerMinutes.value, player)
  toast.success(`${timerMinutes.value} 分钟后自动停止播放`, 2000)
}

function cancelTimer() {
  sleepTimer.cancel()
  toast.info('已取消睡眠定时')
}

async function clearLibrary() {
  if (!confirm('确定清空整个媒体库吗？歌单/历史/收藏/EQ 等也会一并删除。')) return
  await repository.clearAll()
  await equalizer.destroy()
  fileCache.clear()
  await refreshStats()
  toast.success('媒体库已清空，即将刷新', 1500)
  setTimeout(() => location.reload(), 1500)
}

const playbackRates = [0.5, 0.75, 1, 1.25, 1.5, 1.75, 2]
function setRate(r: number) {
  player.setPlaybackRate(r)
}
</script>

<template>
  <section class="settings">
    <header class="page-header">
      <h2>设置</h2>
    </header>

    <!-- 外观 -->
    <div class="card">
      <div class="card-header">
        <h3>外观</h3>
        <p class="desc">选择颜色主题，auto 会跟随操作系统</p>
      </div>
      <div class="theme-row">
        <button
          v-for="t in ['auto', 'light', 'dark'] as const"
          :key="t"
          :class="{ active: theme === t }"
          @click="pickTheme(t)"
        >
          <span class="theme-emoji">
            {{ t === 'auto' ? '🖥️' : t === 'light' ? '☀️' : '🌙' }}
          </span>
          {{ t === 'auto' ? '跟随系统' : t === 'light' ? '浅色' : '深色' }}
        </button>
      </div>
    </div>

    <!-- 播放速度 -->
    <div class="card">
      <div class="card-header">
        <h3>播放速度</h3>
        <p class="desc">当前：<b>{{ player.playbackRate }}x</b></p>
      </div>
      <div class="rate-row">
        <button
          v-for="r in playbackRates"
          :key="r"
          :class="{ active: player.playbackRate === r }"
          @click="setRate(r)"
        >{{ r }}x</button>
      </div>
    </div>

    <!-- 睡眠定时 -->
    <div class="card">
      <div class="card-header">
        <h3>睡眠定时</h3>
        <p class="desc">N 分钟后自动暂停，最后 10 秒做音量淡出</p>
      </div>
      <div class="row">
        <div class="input-group">
          <input type="number" min="1" max="180" v-model.number="timerMinutes" />
          <span class="suffix">分钟</span>
        </div>
        <button class="btn-primary" @click="startTimer" :disabled="timerActive">
          启动
        </button>
        <button class="btn-ghost" @click="cancelTimer" :disabled="!timerActive">
          取消
        </button>
        <span class="status">剩余：<b>{{ timerRemaining }}</b></span>
      </div>
      <p class="tip" v-if="timerActive">定时器正在运行...</p>
      <p class="tip">⚠ 标签页切后台 setTimeout 会被节流到 1Hz，剩余时间显示会有 ±1 秒误差（§06 阶段⑤考点）</p>
    </div>

    <!-- 均衡器（独立组件） -->
    <EqualizerPanel />

    <!-- 媒体库统计 -->
    <div class="card">
      <div class="card-header">
        <h3>媒体库</h3>
        <p class="desc">本地 IndexedDB 存储（Dexie）</p>
      </div>
      <div class="stats">
        <div class="stat">
          <div class="n">{{ mediaCount }}</div>
          <div class="l">曲目</div>
        </div>
        <div class="stat">
          <div class="n">{{ favCount }}</div>
          <div class="l">收藏</div>
        </div>
        <div class="stat">
          <div class="n">{{ historyCount }}</div>
          <div class="l">历史</div>
        </div>
      </div>
      <button class="btn-danger" @click="clearLibrary">清空媒体库</button>
    </div>

    <!-- 关于 -->
    <div class="card about">
      <div class="card-header"><h3>关于</h3></div>
      <p>MiniPlayer · Web 端 <b>v0.2.0</b></p>
      <p class="mut">Vue 3 · TypeScript · Pinia · Dexie · Web Audio</p>
      <p class="mut">配套：书中 10.Apps/07.综合音视频案例/04.Web 端实现(Vue3)</p>
    </div>
  </section>
</template>

<style scoped>
.settings {
  padding: 24px;
  max-width: 920px;
  margin: 0 auto;
  display: flex;
  flex-direction: column;
  gap: 16px;
}
.page-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 4px;
}
.page-header h2 {
  background: linear-gradient(135deg, var(--text) 0%, var(--text-sub) 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}

.card {
  background: var(--bg-elev);
  padding: 20px 24px;
  border-radius: var(--radius-lg);
  border: 1px solid var(--border);
  box-shadow: var(--shadow-sm);
}
.card-header { margin-bottom: 16px; }
.card-header h3 { margin: 0 0 4px; }
.card-header .desc { color: var(--text-mute); font-size: var(--fs-sm); }

/* ---- theme ---- */
.theme-row { display: flex; gap: 8px; flex-wrap: wrap; }
.theme-row button {
  padding: 10px 20px;
  display: inline-flex;
  align-items: center;
  gap: 8px;
  background: var(--bg);
  border: 1px solid var(--border);
  color: var(--text-sub);
  border-radius: var(--radius-full);
  font-size: var(--fs-sm);
  font-weight: 500;
  transition: all .15s;
}
.theme-row button:hover:not(.active) { background: var(--hover); color: var(--text); }
.theme-row button.active {
  background: linear-gradient(135deg, var(--brand-1), var(--brand-2));
  color: #fff;
  border-color: transparent;
  box-shadow: 0 4px 14px rgba(139, 92, 246, 0.35);
}
.theme-emoji { font-size: 14px; }

/* ---- rate ---- */
.rate-row { display: flex; gap: 6px; flex-wrap: wrap; }
.rate-row button {
  padding: 6px 14px;
  background: var(--bg);
  border: 1px solid var(--border);
  color: var(--text-sub);
  border-radius: var(--radius-full);
  font-size: var(--fs-sm);
  min-width: 56px;
  font-variant-numeric: tabular-nums;
  transition: all .15s;
}
.rate-row button:hover:not(.active) { background: var(--hover); color: var(--text); }
.rate-row button.active {
  background: var(--accent-soft);
  color: var(--accent);
  border-color: var(--accent);
  font-weight: 600;
}

/* ---- row shared ---- */
.row { display: flex; align-items: center; gap: 10px; flex-wrap: wrap; }
.input-group {
  display: inline-flex;
  align-items: center;
  background: var(--bg);
  border: 1px solid var(--border);
  border-radius: var(--radius);
  padding: 0 10px;
  gap: 2px;
}
.input-group:focus-within { border-color: var(--accent); box-shadow: 0 0 0 3px var(--accent-soft); }
.input-group input {
  width: 60px;
  padding: 8px 0;
  background: transparent;
  border: none;
  color: var(--text);
  font-size: var(--fs-base);
  text-align: center;
  outline: none;
}
.input-group input::-webkit-outer-spin-button,
.input-group input::-webkit-inner-spin-button {
  -webkit-appearance: none;
  margin: 0;
}
.input-group .suffix {
  color: var(--text-mute);
  font-size: var(--fs-sm);
}

.btn-primary, .btn-ghost, .btn-danger {
  padding: 8px 18px;
  border-radius: var(--radius-full);
  font-size: var(--fs-sm);
  font-weight: 500;
  transition: all .18s;
}
.btn-primary {
  background: linear-gradient(135deg, var(--brand-1), var(--brand-2));
  color: #fff;
  box-shadow: 0 4px 14px rgba(139, 92, 246, 0.35);
}
.btn-primary:hover:not(:disabled) { transform: translateY(-1px); }
.btn-primary:disabled { opacity: .45; cursor: not-allowed; }
.btn-ghost {
  background: var(--bg);
  border: 1px solid var(--border);
  color: var(--text-sub);
}
.btn-ghost:hover:not(:disabled) { background: var(--hover); color: var(--text); }
.btn-ghost:disabled { opacity: .4; }
.btn-danger {
  background: var(--danger);
  color: #fff;
}
.btn-danger:hover { filter: brightness(1.1); }

.status { color: var(--text-mute); font-size: var(--fs-sm); margin-left: auto; }
.status b { color: var(--accent); font-variant-numeric: tabular-nums; }
.tip { color: var(--text-mute); font-size: var(--fs-xs); margin: 10px 0 0; }

/* ---- stats ---- */
.stats {
  display: flex;
  gap: 32px;
  margin-bottom: 20px;
}
.stat {
  flex: 1;
  display: flex;
  flex-direction: column;
  padding: 12px 14px;
  background: var(--bg);
  border-radius: var(--radius);
  border: 1px solid var(--border);
}
.stat .n {
  font-size: 24px;
  font-weight: 700;
  color: var(--accent);
  font-variant-numeric: tabular-nums;
  background: linear-gradient(135deg, var(--brand-1), var(--brand-2));
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}
.stat .l {
  margin-top: 2px;
  color: var(--text-mute);
  font-size: var(--fs-xs);
}

.about p { margin: 4px 0; color: var(--text-sub); font-size: var(--fs-sm); }
.about .mut { color: var(--text-mute); font-size: var(--fs-xs); }

@media (max-width: 640px) {
  .settings { padding: 16px 12px; }
  .card { padding: 16px; }
  .stats { gap: 10px; }
  .stat { padding: 10px; }
  .stat .n { font-size: 20px; }
}
</style>
