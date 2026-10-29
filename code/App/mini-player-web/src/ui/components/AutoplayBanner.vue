<!-- 自动播放策略提示条：只在 needsUserGesture=true 时显示 -->
<script setup lang="ts">
import { usePlayerStore } from '@/service/playerService'

const player = usePlayerStore()

function dismiss() {
  player.needsUserGesture = false
}

function playNow() {
  player.needsUserGesture = false
  player.play()
}
</script>

<template>
  <Transition name="banner">
    <div v-if="player.needsUserGesture" class="banner">
      <span class="icon">🔇</span>
      <span class="msg">浏览器拦截了自动播放</span>
      <button class="play" @click="playNow">现在播放</button>
      <button class="close" @click="dismiss" aria-label="关闭">✕</button>
    </div>
  </Transition>
</template>

<style scoped>
.banner {
  position: fixed;
  top: calc(env(safe-area-inset-top, 0) + 16px);
  left: 50%;
  transform: translateX(-50%);
  z-index: 9999;
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 10px 10px 10px 16px;
  background: var(--glass-bg);
  backdrop-filter: blur(var(--glass-blur)) saturate(var(--glass-saturate));
  -webkit-backdrop-filter: blur(var(--glass-blur)) saturate(var(--glass-saturate));
  border: 1px solid var(--glass-border);
  border-radius: var(--radius-full);
  box-shadow: var(--shadow-lg);
  font-size: var(--fs-sm);
  color: var(--text);
}
.icon { font-size: 16px; }
.msg { color: var(--text-sub); }
.play {
  padding: 5px 14px;
  border-radius: var(--radius-full);
  background: linear-gradient(135deg, var(--brand-1), var(--brand-2));
  color: #fff;
  font-weight: 500;
  font-size: var(--fs-sm);
}
.play:hover { filter: brightness(1.08); }
.close {
  width: 24px; height: 24px;
  border-radius: 50%;
  color: var(--text-mute);
}
.close:hover { background: var(--hover); color: var(--text); }

.banner-enter-active, .banner-leave-active { transition: all .25s cubic-bezier(.2,.8,.3,1); }
.banner-enter-from, .banner-leave-to { opacity: 0; transform: translate(-50%, -8px); }
</style>
