<!-- 设置页：主题 + 均衡器 + 睡眠定时 + 媒体库维护
     对应 §08 阶段⑦ + §09 阶段⑧ -->
<script setup lang="ts">
import { ref, onMounted, onBeforeUnmount } from 'vue'
import { readThemeMode, writeThemeMode, type ThemeMode } from '@/utils/theme'
import { repository } from '@/data/repository'
import { usePlayerStore } from '@/service/playerService'
import { sleepTimer } from '@/service/sleepTimer'
import { equalizer } from '@/service/equalizer'
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
}

function startTimer() {
  sleepTimer.start(timerMinutes.value, player)
}

function cancelTimer() {
  sleepTimer.cancel()
}

async function clearLibrary() {
  if (!confirm('确定清空整个媒体库吗？歌单/历史/收藏/EQ 等也会一并删除。')) return
  await repository.clearAll()
  await equalizer.destroy()
  await refreshStats()
  location.reload()
}
</script>

<template>
  <section class="settings">
    <h2>设置</h2>

    <div class="card">
      <h3>外观</h3>
      <div class="theme-row">
        <button :class="{ active: theme === 'auto' }" @click="pickTheme('auto')">跟随系统</button>
        <button :class="{ active: theme === 'light' }" @click="pickTheme('light')">浅色</button>
        <button :class="{ active: theme === 'dark' }" @click="pickTheme('dark')">深色</button>
      </div>
    </div>

    <div class="card">
      <h3>睡眠定时</h3>
      <div class="row">
        <label>
          分钟
          <input type="number" min="1" max="180" v-model.number="timerMinutes" />
        </label>
        <button class="primary" @click="startTimer" :disabled="timerActive">启动</button>
        <button @click="cancelTimer" :disabled="!timerActive">取消</button>
        <span class="hint">剩余：<b>{{ timerRemaining }}</b></span>
      </div>
      <p class="tip">⚠ 标签页切后台 setTimeout 会被节流到 1Hz，剩余时间显示会有 ±1 秒误差（§06 阶段⑤考点）。</p>
    </div>

    <EqualizerPanel />

    <div class="card">
      <h3>媒体库统计</h3>
      <div class="stats">
        <div><b>{{ mediaCount }}</b> 媒体</div>
        <div><b>{{ favCount }}</b> 收藏</div>
        <div><b>{{ historyCount }}</b> 历史记录</div>
      </div>
      <div class="row">
        <button class="danger" @click="clearLibrary">清空媒体库</button>
      </div>
    </div>

    <div class="card about">
      <h3>关于</h3>
      <p>MiniPlayer · Web 端 v0.1.0</p>
      <p>Vue 3 + TypeScript + Pinia + Dexie + Web Audio</p>
      <p>对应书中 10.Apps/07.综合音视频案例/04.Web端实现(Vue3)</p>
    </div>
  </section>
</template>

<style scoped>
.settings {
  padding: 20px;
  max-width: 820px;
  margin: 0 auto;
  display: flex;
  flex-direction: column;
  gap: 16px;
}
h2 { margin: 10px 0 6px; }
.card {
  background: var(--bg-elev);
  padding: 20px;
  border-radius: var(--radius);
  box-shadow: var(--shadow-sm);
}
.card h3 { margin: 0 0 12px; }

.theme-row { display: flex; gap: 8px; }
.theme-row button, .row button {
  padding: 7px 14px;
  background: var(--hover);
  color: var(--text-sub);
  border-radius: 8px;
  font-size: 13px;
}
.theme-row button.active { background: var(--accent); color: #fff; }

.row {
  display: flex; align-items: center; gap: 10px; flex-wrap: wrap;
}
.row label { display: flex; align-items: center; gap: 6px; font-size: 13px; }
.row input[type='number'] {
  width: 72px;
  padding: 6px 10px;
  background: var(--bg);
  border: 1px solid var(--border);
  border-radius: 6px;
  color: var(--text);
  outline: none;
}
.row input[type='number']:focus { border-color: var(--accent); }
.row .primary { background: var(--accent); color: #fff; }
.row .danger { background: var(--danger); color: #fff; }
.row .hint { color: var(--text-mute); font-size: 13px; }
.tip { color: var(--text-mute); font-size: 12px; margin: 8px 0 0; }

.stats {
  display: flex; gap: 24px; margin-bottom: 12px;
}
.stats > div {
  color: var(--text-sub);
  font-size: 13px;
}
.stats b { color: var(--accent); font-size: 20px; margin-right: 4px; }

.about p { margin: 4px 0; color: var(--text-sub); font-size: 13px; }
</style>
