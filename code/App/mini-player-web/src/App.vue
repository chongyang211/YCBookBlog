<!-- 应用根组件
     关键设计：<audio>/<video> 元素放在根组件，路由切换也不断播（对应 §04 Step 4.2） -->
<script setup lang="ts">
import { onMounted, onBeforeUnmount, computed } from 'vue'
import { RouterLink, RouterView } from 'vue-router'
import { usePlayerStore } from '@/service/playerService'
import { bindMediaSession, unbindMediaSession } from '@/service/mediaSession'
import { useGlobalShortcuts } from '@/utils/useGlobalShortcuts'
import PlayerBar from '@/ui/components/PlayerBar.vue'

const player = usePlayerStore()
useGlobalShortcuts(player)

const navItems = [
  { to: '/library/all', label: '媒体库', icon: '♪' },
  { to: '/playing', label: '正在播放', icon: '▶' },
  { to: '/settings', label: '设置', icon: '⚙' },
]

const audioHidden = computed(() => player.current?.type !== 'video')

function onAudioRef(el: HTMLAudioElement | null) {
  // 把 <audio> 托管给 store；store 内部用 shallowRef + markRaw 防响应式代理
  if (el) player.attachAudioElement(el)
}

function onVideoRef(el: HTMLVideoElement | null) {
  if (el) player.attachVideoElement(el)
}

onMounted(async () => {
  await player.initialize()
  bindMediaSession(player)
})

onBeforeUnmount(() => {
  unbindMediaSession()
})
</script>

<template>
  <div class="app-shell">
    <aside class="sidebar">
      <h1 class="logo">
        <span class="logo-dot">●</span> MiniPlayer
      </h1>
      <nav>
        <RouterLink v-for="n in navItems" :key="n.to" :to="n.to" class="nav-item">
          <span class="nav-icon">{{ n.icon }}</span>{{ n.label }}
        </RouterLink>
      </nav>
      <div class="sidebar-footer">Web 端 · Vue 3</div>
    </aside>

    <main class="main">
      <RouterView />
    </main>

    <PlayerBar />

    <!-- 元素放根：切路由不打断播放 -->
    <audio
      :ref="onAudioRef"
      :hidden="true"
      preload="metadata"
      crossorigin="anonymous"
    />
    <video
      :ref="onVideoRef"
      class="video-canvas"
      :class="{ hidden: audioHidden }"
      preload="metadata"
      playsinline
      crossorigin="anonymous"
    />
  </div>
</template>

<style scoped>
.app-shell {
  display: grid;
  grid-template-columns: 220px 1fr;
  grid-template-rows: 1fr auto;
  height: 100vh;
  width: 100vw;
}
.sidebar {
  grid-row: 1 / 2;
  background: var(--sidebar-bg);
  border-right: 1px solid var(--border);
  padding: 20px 12px;
  display: flex;
  flex-direction: column;
  gap: 8px;
}
.logo {
  font-size: 18px;
  font-weight: 600;
  margin: 0 10px 20px;
  color: var(--text);
  display: flex;
  align-items: center;
  gap: 8px;
}
.logo-dot {
  color: var(--accent);
  font-size: 22px;
}
.nav-item {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 10px 14px;
  border-radius: 10px;
  color: var(--text-sub);
  text-decoration: none;
  transition: all .15s;
  font-size: 14px;
}
.nav-item:hover { background: var(--hover); color: var(--text); }
.nav-item.router-link-active { background: var(--accent-soft); color: var(--accent); font-weight: 600; }
.nav-icon { font-size: 15px; width: 16px; text-align: center; }
.sidebar-footer {
  margin-top: auto;
  padding: 10px 14px;
  color: var(--text-mute);
  font-size: 12px;
}
.main {
  grid-row: 1 / 2;
  overflow: auto;
  position: relative;
}
.video-canvas {
  position: fixed;
  inset: 0;
  width: 100%;
  height: calc(100% - var(--playerbar-h));
  background: #000;
  object-fit: contain;
  z-index: 20;
}
.video-canvas.hidden { display: none; }

@media (max-width: 640px) {
  .app-shell { grid-template-columns: 1fr; }
  .sidebar {
    position: fixed;
    bottom: var(--playerbar-h);
    left: 0; right: 0;
    display: flex;
    flex-direction: row;
    border-right: none;
    border-top: 1px solid var(--border);
    padding: 6px;
    z-index: 15;
  }
  .logo { display: none; }
  .sidebar-footer { display: none; }
  .nav-item { flex: 1; justify-content: center; }
}
</style>
