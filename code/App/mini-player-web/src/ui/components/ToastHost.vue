<!-- 全局 Toast 宿主：顶部中央，自动叠加 -->
<script setup lang="ts">
import { toastState, toast } from '@/service/toast'
</script>

<template>
  <TransitionGroup name="toast" tag="div" class="toast-host">
    <div
      v-for="t in toastState.list"
      :key="t.id"
      class="toast"
      :class="t.kind"
      @click="toast.dismiss(t.id)"
    >
      <span class="icon">
        <template v-if="t.kind === 'success'">✓</template>
        <template v-else-if="t.kind === 'error'">✕</template>
        <template v-else-if="t.kind === 'warn'">!</template>
        <template v-else>i</template>
      </span>
      <span class="msg">{{ t.message }}</span>
    </div>
  </TransitionGroup>
</template>

<style scoped>
.toast-host {
  position: fixed;
  top: 20px;
  left: 50%;
  transform: translateX(-50%);
  z-index: 10000;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 10px;
  pointer-events: none;
}
.toast {
  pointer-events: auto;
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 10px 16px 10px 14px;
  border-radius: var(--radius-full);
  background: var(--glass-bg);
  backdrop-filter: blur(var(--glass-blur)) saturate(var(--glass-saturate));
  -webkit-backdrop-filter: blur(var(--glass-blur)) saturate(var(--glass-saturate));
  border: 1px solid var(--glass-border);
  color: var(--text);
  font-size: var(--fs-sm);
  box-shadow: var(--shadow-lg);
  cursor: pointer;
  max-width: 90vw;
}
.toast .icon {
  width: 20px;
  height: 20px;
  border-radius: 50%;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  font-size: 11px;
  font-weight: 700;
  color: #fff;
  flex-shrink: 0;
}
.toast.success .icon { background: var(--success); }
.toast.error   .icon { background: var(--danger); }
.toast.warn    .icon { background: var(--warn); color: #000; }
.toast.info    .icon { background: var(--info); }

.toast.success { box-shadow: 0 10px 32px rgba(34, 197, 94, 0.25); }
.toast.error   { box-shadow: 0 10px 32px rgba(239, 68, 68, 0.25); }
.toast.warn    { box-shadow: 0 10px 32px rgba(245, 158, 11, 0.25); }

.toast .msg { line-height: 1.4; }

.toast-enter-active, .toast-leave-active {
  transition: all .28s cubic-bezier(.2, .8, .3, 1);
}
.toast-enter-from {
  opacity: 0;
  transform: translateY(-10px) scale(.96);
}
.toast-leave-to {
  opacity: 0;
  transform: translateY(-6px) scale(.98);
}
</style>
