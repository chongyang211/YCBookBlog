<!-- 媒体列表：对应 §03 Step 3.5
     大列表的性能墙 —— 当前方案是朴素 v-for，1k 条内够用；
     阶段②的 §03 Step 3.5 挑战是接入 virtual-scroll，这里留了 TODO 注释。 -->
<script setup lang="ts">
import { computed } from 'vue'
import type { MediaItem } from '@/types'
import { formatTime } from '@/utils/format'
import { usePlayerStore } from '@/service/playerService'

const props = defineProps<{
  items: MediaItem[]
  emptyText?: string
}>()
const emit = defineEmits<{
  (e: 'play', index: number): void
}>()

const player = usePlayerStore()

const currentId = computed(() => player.current?.id ?? '')

function onPlay(index: number) {
  emit('play', index)
}
</script>

<template>
  <div class="list" v-if="props.items.length">
    <!-- TODO: 1 万条以上接 virtual-scroll；现在朴素 v-for 够用 -->
    <div
      v-for="(it, i) in props.items"
      :key="it.id"
      class="row"
      :class="{ active: it.id === currentId }"
      @dblclick="onPlay(i)"
    >
      <div class="idx">{{ i + 1 }}</div>
      <div class="cover">
        <img v-if="it.coverUrl" :src="it.coverUrl" alt="" />
        <span v-else>{{ it.type === 'video' ? '🎬' : '♪' }}</span>
      </div>
      <div class="info">
        <div class="title">{{ it.title }}</div>
        <div class="sub">{{ it.artist }} · {{ it.album }}</div>
      </div>
      <div class="dur">{{ formatTime(it.duration) }}</div>
      <button class="play-btn" @click="onPlay(i)" title="播放">▶</button>
    </div>
  </div>
  <div v-else class="empty">{{ props.emptyText ?? '媒体库为空' }}</div>
</template>

<style scoped>
.list {
  display: flex;
  flex-direction: column;
  background: var(--bg-elev);
  border-radius: var(--radius);
  overflow: hidden;
  box-shadow: var(--shadow-sm);
}
.row {
  display: grid;
  grid-template-columns: 40px 54px 1fr 70px 40px;
  align-items: center;
  gap: 12px;
  padding: 8px 16px;
  cursor: pointer;
  border-bottom: 1px solid var(--border);
  transition: background .12s;
}
.row:last-child { border-bottom: none; }
.row:hover { background: var(--hover); }
.row.active { background: var(--accent-soft); color: var(--accent); }
.row.active .sub { color: var(--accent); opacity: .8; }

.idx { color: var(--text-mute); font-size: 12px; text-align: center; font-variant-numeric: tabular-nums; }
.cover {
  width: 44px; height: 44px;
  border-radius: 6px;
  background: var(--hover);
  display: flex; align-items: center; justify-content: center;
  overflow: hidden;
  font-size: 18px; color: var(--text-mute);
}
.cover img { width: 100%; height: 100%; object-fit: cover; }

.info { overflow: hidden; }
.title { font-weight: 500; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
.sub { color: var(--text-mute); font-size: 12px; }

.dur { color: var(--text-mute); font-size: 12px; text-align: right; font-variant-numeric: tabular-nums; }
.play-btn {
  color: var(--accent);
  font-size: 16px;
  padding: 4px;
  border-radius: 50%;
  width: 32px; height: 32px;
}
.play-btn:hover { background: var(--accent-soft); }

.empty {
  padding: 60px 20px;
  text-align: center;
  color: var(--text-mute);
  background: var(--bg-elev);
  border-radius: var(--radius);
}
</style>
