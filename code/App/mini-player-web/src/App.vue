<!-- 应用根组件
     关键设计：<audio>/<video> 元素放在根组件，路由切换也不断播（对应 §04 Step 4.2）
     布局：
       - 桌面：左侧栏（220px） + 主区（flex）+ 底部 PlayerBar（固定）
       - 手机：主区（上） + 底部 PlayerBar（固定）+ 底部 Tab Bar（固定更下）
     动态背景：当前曲目封面放大 blur 作为软背景 -->
<script setup lang="ts">
import { onMounted, onBeforeUnmount, computed, ref } from 'vue'
import { RouterLink, RouterView, useRoute } from 'vue-router'
import { usePlayerStore } from '@/service/playerService'
import { bindMediaSession, unbindMediaSession } from '@/service/mediaSession'
import { useGlobalShortcuts } from '@/utils/useGlobalShortcuts'
import PlayerBar from '@/ui/components/PlayerBar.vue'
import ToastHost from '@/ui/components/ToastHost.vue'
import AutoplayBanner from '@/ui/components/AutoplayBanner.vue'
import MobileNav from '@/ui/components/MobileNav.vue'

const player = usePlayerStore()
const route = useRoute()
useGlobalShortcuts(player)

const navItems = [
  { to: '/library/all',      label: '媒体库',  icon: '♪' },
  { to: '/library/favorite', label: '我喜欢',  icon: '♥' },
  { to: '/library/history',  label: '最近播放', icon: '⟲' },
  { to: '/playing',          label: '正在播放', icon: '▶' },
  { to: '/settings',         label: '设置',    icon: '⚙' },
]

const audioHidden = computed(() => player.current?.type !== 'video')
const isPlayingPage = computed(() => route.path.startsWith('/playing'))

const bgCoverStyle = computed(() => {
  const url = player.current?.coverUrl
  if (!url) return {}
  return { backgroundImage: `url(${url})` }
})

const sidebarCollapsed = ref(false)

function onAudioRef(el: unknown) {
  if (el instanceof HTMLAudioElement) player.attachAudioElement(el)
}
function onVideoRef(el: unknown) {
  if (el instanceof HTMLVideoElement) player.attachVideoElement(el)
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
  <div class="app-shell" :class="{ 'has-cover': !!player.current?.coverUrl }">
    <!-- 动态封面背景层：仅在有封面时显示 -->
    <div class="bg-cover" :style="bgCoverStyle" />
    <div class="bg-mask" />

    <aside class="sidebar glass" :class="{ collapsed: sidebarCollapsed }">
      <div class="logo-row">
        <div class="logo">
          <span class="logo-dot" />
          <span class="logo-text">MiniPlayer</span>
        </div>
        <button class="collapse-btn" @click="sidebarCollapsed = !sidebarCollapsed" :title="sidebarCollapsed ? '展开' : '折叠'">
          {{ sidebarCollapsed ? '»' : '«' }}
        </button>
      </div>

      <nav>
        <RouterLink
          v-for="n in navItems"
          :key="n.to"
          :to="n.to"
          class="nav-item"
          :title="n.label"
        >
          <span class="nav-icon">{{ n.icon }}</span>
          <span class="nav-label">{{ n.label }}</span>
        </RouterLink>
      </nav>

      <div class="sidebar-footer">
        <span class="hint">Vue 3 · Pinia · Dexie</span>
      </div>
    </aside>

    <main class="main">
      <RouterView v-slot="{ Component }">
        <Transition name="page" mode="out-in">
          <component :is="Component" />
        </Transition>
      </RouterView>
    </main>

    <PlayerBar />
    <MobileNav />

    <!-- 元素放根：切路由不打断播放 -->
    <audio
      :ref="onAudioRef"
      preload="metadata"
      crossorigin="anonymous"
    />
    <video
      :ref="onVideoRef"
      class="video-canvas"
      :class="{ hidden: audioHidden, 'in-playing': isPlayingPage }"
      preload="metadata"
      playsinline
      crossorigin="anonymous"
    />

    <!-- 全局提示系统 -->
    <ToastHost />
    <AutoplayBanner />
  </div>
</template>

<style scoped>
.app-shell {
  position: relative;
  display: grid;
  grid-template-columns: var(--sidebar-w) 1fr;
  height: 100%;
  overflow: hidden;
  padding-bottom: var(--playerbar-h);
}

/* ---- 动态封面背景 ---- */
.bg-cover {
  position: absolute; inset: 0;
  background-size: cover;
  background-position: center;
  filter: blur(60px) saturate(1.3);
  transform: scale(1.2);
  opacity: 0;
  transition: opacity 1.2s ease;
  pointer-events: none;
  z-index: 0;
}
.app-shell.has-cover .bg-cover { opacity: 0.5; }
.bg-mask {
  position: absolute; inset: 0;
  background: linear-gradient(180deg, var(--bg-grad-start) 0%, var(--bg-grad-end) 100%);
  opacity: .72;
  pointer-events: none;
  z-index: 1;
}

/* ---- 侧栏 ---- */
.sidebar {
  position: relative;
  z-index: 10;
  padding: 20px 14px 20px;
  display: flex;
  flex-direction: column;
  gap: 4px;
  background: var(--sidebar-bg);
  backdrop-filter: blur(var(--glass-blur)) saturate(var(--glass-saturate));
  -webkit-backdrop-filter: blur(var(--glass-blur)) saturate(var(--glass-saturate));
  border-right: 1px solid var(--border);
  transition: all .3s cubic-bezier(.2,.8,.3,1);
}
.sidebar.collapsed { padding-left: 10px; padding-right: 10px; }
.sidebar.collapsed .logo-text,
.sidebar.collapsed .nav-label,
.sidebar.collapsed .sidebar-footer { display: none; }
.sidebar.collapsed .nav-item { justify-content: center; padding: 10px; }

.logo-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin: 0 6px 16px;
}
.logo {
  display: flex;
  align-items: center;
  gap: 10px;
  font-size: var(--fs-lg);
  font-weight: 700;
  color: var(--text);
  letter-spacing: -0.01em;
}
.logo-dot {
  width: 14px; height: 14px;
  border-radius: 50%;
  background: linear-gradient(135deg, var(--brand-1), var(--brand-2));
  box-shadow: 0 0 12px rgba(139, 92, 246, 0.6);
  animation: pulse 2.4s ease-in-out infinite;
}
.collapse-btn {
  width: 24px; height: 24px;
  color: var(--text-mute);
  border-radius: var(--radius-sm);
  font-size: var(--fs-sm);
}
.collapse-btn:hover { background: var(--hover); color: var(--text); }

nav {
  display: flex;
  flex-direction: column;
  gap: 2px;
}
.nav-item {
  display: flex;
  align-items: center;
  gap: 12px;
  padding: 10px 14px;
  border-radius: var(--radius);
  color: var(--text-sub);
  font-size: var(--fs-base);
  font-weight: 500;
  transition: all .15s;
  position: relative;
}
.nav-item:hover { background: var(--hover); color: var(--text); }
.nav-item.router-link-active {
  background: var(--accent-soft);
  color: var(--accent);
  font-weight: 600;
}
.nav-item.router-link-active::before {
  content: '';
  position: absolute;
  left: 0; top: 20%; bottom: 20%;
  width: 3px;
  background: linear-gradient(135deg, var(--brand-1), var(--brand-2));
  border-radius: 2px;
}
.nav-icon {
  font-size: 16px;
  width: 20px;
  text-align: center;
  flex-shrink: 0;
}

.sidebar-footer {
  margin-top: auto;
  padding: 10px 14px;
  color: var(--text-mute);
  font-size: var(--fs-xs);
}
.hint {
  display: inline-block;
  opacity: .7;
}

/* ---- 主区 ---- */
.main {
  position: relative;
  z-index: 5;
  overflow-y: auto;
  overflow-x: hidden;
  height: 100%;
}

/* ---- 视频层 ---- */
.video-canvas {
  position: fixed;
  inset: 0;
  width: 100%;
  height: calc(100% - var(--playerbar-h));
  background: #000;
  object-fit: contain;
  z-index: 2;
}
.video-canvas.in-playing {
  /* 在正在播放页上更明显 */
  z-index: 3;
}
.video-canvas.hidden { display: none; }

/* ---- 页面切换动画 ---- */
.page-enter-active, .page-leave-active {
  transition: opacity .22s ease, transform .22s cubic-bezier(.2,.8,.3,1);
}
.page-enter-from { opacity: 0; transform: translateY(8px); }
.page-leave-to   { opacity: 0; transform: translateY(-4px); }

/* ---- 响应式：平板 ---- */
@media (max-width: 1024px) {
  .app-shell { grid-template-columns: 72px 1fr; }
  .sidebar { padding-left: 10px; padding-right: 10px; }
  .sidebar .logo-text,
  .sidebar .nav-label,
  .sidebar .sidebar-footer { display: none; }
  .sidebar .nav-item { justify-content: center; padding: 10px; }
  .sidebar .collapse-btn { display: none; }
  .logo-row { justify-content: center; }
}

/* ---- 响应式：手机 ---- */
@media (max-width: 640px) {
  .app-shell {
    grid-template-columns: 1fr;
    padding-bottom: calc(var(--playerbar-h) + var(--mobile-nav-h));
  }
  .sidebar { display: none; }
  .video-canvas { height: calc(100% - var(--playerbar-h) - var(--mobile-nav-h)); }
}

/* 桌面端隐藏手机导航 */
@media (min-width: 641px) {
  :deep(.mobile-nav) { display: none; }
}
</style>
