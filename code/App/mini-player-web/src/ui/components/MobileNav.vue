<!-- 手机底部导航：3 Tab 栏 -->
<script setup lang="ts">
import { RouterLink, useRoute } from 'vue-router'
import { computed } from 'vue'

const route = useRoute()
const items = [
  { to: '/library/all', label: '媒体库', icon: '♪' },
  { to: '/playing',     label: '播放',   icon: '▶' },
  { to: '/settings',    label: '设置',   icon: '⚙' },
]
const activeTo = computed(() => route.path)
</script>

<template>
  <nav class="mobile-nav glass">
    <RouterLink
      v-for="n in items"
      :key="n.to"
      :to="n.to"
      class="nav-item"
      :class="{ active: activeTo.startsWith(n.to.split('/').slice(0, 2).join('/')) }"
    >
      <span class="icon">{{ n.icon }}</span>
      <span class="label">{{ n.label }}</span>
    </RouterLink>
  </nav>
</template>

<style scoped>
.mobile-nav {
  position: fixed;
  bottom: 0;
  left: 0; right: 0;
  z-index: 50;
  display: flex;
  justify-content: space-around;
  align-items: stretch;
  height: var(--mobile-nav-h);
  padding-bottom: env(safe-area-inset-bottom, 0);
  background: var(--playerbar-bg);
  backdrop-filter: blur(var(--glass-blur)) saturate(var(--glass-saturate));
  -webkit-backdrop-filter: blur(var(--glass-blur)) saturate(var(--glass-saturate));
  border-top: 1px solid var(--border);
}
.nav-item {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 2px;
  color: var(--text-mute);
  font-size: var(--fs-xs);
  transition: color .15s;
}
.nav-item:active { background: var(--active); }
.nav-item.active {
  color: var(--accent);
}
.nav-item.active .icon {
  transform: scale(1.1);
  text-shadow: 0 0 10px var(--accent-soft);
}
.icon {
  font-size: 20px;
  transition: transform .2s;
}
.label { font-weight: 500; }
</style>
