<!-- 空库引导页：媒体库为空时显示 -->
<script setup lang="ts">
defineProps<{
  title?: string
  message?: string
  showImport?: boolean
}>()
const emit = defineEmits<{
  (e: 'importDir'): void
  (e: 'importFile'): void
}>()
</script>

<template>
  <div class="empty-state">
    <div class="icon">
      <div class="disc">
        <div class="hole" />
      </div>
    </div>
    <h3>{{ title ?? '媒体库是空的' }}</h3>
    <p>{{ message ?? '导入本地音乐或视频，开始你的收藏' }}</p>
    <div v-if="showImport ?? true" class="actions">
      <button class="btn-primary" @click="emit('importDir')">
        <span>📁</span> 选择目录
      </button>
      <button class="btn-secondary" @click="emit('importFile')">
        <span>📄</span> 选择文件
      </button>
    </div>
    <p class="tip">或者直接把文件/文件夹拖到页面任意位置</p>
  </div>
</template>

<style scoped>
.empty-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  text-align: center;
  padding: 60px 20px;
  gap: var(--space-3);
  color: var(--text-sub);
  animation: fade-in .35s ease;
}
.icon {
  position: relative;
  width: 120px;
  height: 120px;
  margin-bottom: var(--space-3);
}
.disc {
  width: 100%;
  height: 100%;
  border-radius: 50%;
  background:
    radial-gradient(circle at center, #0f172a 0%, #0f172a 30%, transparent 30%),
    conic-gradient(from 0deg, var(--brand-1), var(--brand-2), var(--brand-1));
  position: relative;
  box-shadow: 0 10px 32px rgba(139, 92, 246, 0.3);
  animation: spin 6s linear infinite;
}
.hole {
  position: absolute;
  width: 20px;
  height: 20px;
  background: var(--bg-elev);
  border-radius: 50%;
  top: 50%; left: 50%;
  transform: translate(-50%, -50%);
  box-shadow: 0 0 0 4px rgba(0, 0, 0, 0.2);
}
.empty-state h3 {
  color: var(--text);
  font-size: var(--fs-xl);
  margin-top: var(--space-3);
}
.empty-state p { color: var(--text-sub); }

.actions {
  display: flex;
  gap: 10px;
  margin-top: var(--space-4);
  flex-wrap: wrap;
  justify-content: center;
}
.btn-primary, .btn-secondary {
  display: inline-flex;
  align-items: center;
  gap: 6px;
  padding: 10px 20px;
  border-radius: var(--radius-full);
  font-size: var(--fs-md);
  font-weight: 500;
  transition: all .2s;
}
.btn-primary {
  background: linear-gradient(135deg, var(--brand-1), var(--brand-2));
  color: #fff;
  box-shadow: 0 6px 18px rgba(139, 92, 246, 0.4);
}
.btn-primary:hover {
  transform: translateY(-1px);
  box-shadow: 0 10px 24px rgba(139, 92, 246, 0.5);
}
.btn-secondary {
  background: var(--bg-elev);
  border: 1px solid var(--border);
  color: var(--text);
}
.btn-secondary:hover {
  background: var(--hover);
  border-color: var(--border-strong);
}
.tip {
  margin-top: var(--space-3);
  font-size: var(--fs-sm);
  color: var(--text-mute);
}
</style>
