<!-- 媒体列表：对应 §03 Step 3.5
     大列表的性能墙 —— 当前方案是朴素 v-for，1k 条内够用；
     阶段②的 §03 Step 3.5 挑战是接入 virtual-scroll，这里留了 TODO 注释。 -->
<script setup lang="ts">
import { computed } from 'vue'
import type { MediaItem } from '@/types'
import { formatTime } from '@/utils/format'
import { usePlayerStore } from '@/service/playerService'
import { repository } from '@/data/repository'
import { fileCache } from '@/service/fileCache'
import { toast } from '@/service/toast'
import CoverArt from './CoverArt.vue'

const props = defineProps<{
  items: MediaItem[]
  emptyText?: string
}>()
const emit = defineEmits<{
  (e: 'play', index: number): void
  (e: 'reload'): void
}>()

const player = usePlayerStore()

const currentId = computed(() => player.current?.id ?? '')

function onPlay(index: number) {
  emit('play', index)
}

async function onDelete(e: Event, id: string, title: string) {
  e.stopPropagation()
  if (!confirm(`从媒体库移除 "${title}"？`)) return
  await repository.deleteMedia(id)
  fileCache.remove(id)
  toast.success('已从媒体库移除', 1600)
  emit('reload')
}

function fileAvailable(item: MediaItem): boolean {
  return !!(item.file || fileCache.has(item.id) || item.externalUrl)
}
</script>

<template>
  <div class="list" v-if="props.items.length">
    <!-- TODO: 1 万条以上接 virtual-scroll；现在朴素 v-for 够用 -->
    <div
      v-for="(it, i) in props.items"
      :key="it.id"
      class="row"
      :class="{ active: it.id === currentId, 'is-missing': !fileAvailable(it) }"
      @dblclick="onPlay(i)"
    >
      <div class="idx">
        <span v-if="it.id !== currentId">{{ i + 1 }}</span>
        <span v-else class="playing-icon">
          <span v-if="player.isPlaying" class="bars">
            <i /><i /><i />
          </span>
          <span v-else>▶</span>
        </span>
      </div>
      <CoverArt :src="it.coverUrl" :id="it.id" :size="44" :placeholder="it.type === 'video' ? '▸' : '♪'" />
      <div class="info">
        <div class="title ellipsis" :title="it.title">
          <span v-if="!fileAvailable(it)" class="missing-badge" title="文件已失效">!</span>
          {{ it.title }}
        </div>
        <div class="sub ellipsis">{{ it.artist }}<span v-if="it.album"> · {{ it.album }}</span></div>
      </div>
      <div class="type-badge" v-if="it.type === 'video'">视频</div>
      <div class="dur">{{ formatTime(it.duration) }}</div>
      <div class="row-actions">
        <button class="play-btn" @click.stop="onPlay(i)" title="播放">▶</button>
        <button class="more-btn" @click.stop="onDelete($event, it.id, it.title)" title="删除">✕</button>
      </div>
    </div>
  </div>
  <div v-else class="empty-row">{{ props.emptyText ?? '媒体库为空' }}</div>
</template>

<style scoped>
.list {
  display: flex;
  flex-direction: column;
  background: var(--bg-elev);
  border-radius: var(--radius-lg);
  overflow: hidden;
  box-shadow: var(--shadow-sm);
  border: 1px solid var(--border);
}
.row {
  display: grid;
  grid-template-columns: 40px 44px 1fr auto 60px auto;
  align-items: center;
  gap: 14px;
  padding: 8px 16px;
  cursor: pointer;
  border-bottom: 1px solid var(--border);
  transition: background .12s;
}
.row:last-child { border-bottom: none; }
.row:hover { background: var(--hover); }
.row.active {
  background: var(--accent-soft);
}
.row.active .title { color: var(--accent); font-weight: 600; }
.row.active .sub { color: var(--accent); opacity: .75; }
.row.is-missing .title { color: var(--text-mute); }

.idx {
  color: var(--text-mute);
  font-size: var(--fs-xs);
  text-align: center;
  font-variant-numeric: tabular-nums;
}
.playing-icon { color: var(--accent); font-size: 14px; }
.bars {
  display: inline-flex;
  align-items: flex-end;
  gap: 2px;
  height: 14px;
}
.bars i {
  display: inline-block;
  width: 3px;
  background: var(--accent);
  border-radius: 1px;
  animation: wave 1s ease-in-out infinite;
}
.bars i:nth-child(1) { animation-delay: 0s; }
.bars i:nth-child(2) { animation-delay: .18s; }
.bars i:nth-child(3) { animation-delay: .36s; }
@keyframes wave {
  0%, 100% { height: 20%; }
  50%      { height: 100%; }
}

.info { overflow: hidden; min-width: 0; }
.title { font-size: var(--fs-base); color: var(--text); display: flex; align-items: center; gap: 6px; }
.sub { color: var(--text-mute); font-size: var(--fs-sm); margin-top: 2px; }
.missing-badge {
  display: inline-flex;
  width: 16px; height: 16px;
  background: var(--warn);
  color: #fff;
  border-radius: 50%;
  font-size: 10px;
  font-weight: 700;
  align-items: center;
  justify-content: center;
  flex-shrink: 0;
}

.type-badge {
  background: var(--bg);
  color: var(--text-mute);
  padding: 2px 8px;
  border-radius: var(--radius-full);
  font-size: 10px;
  border: 1px solid var(--border);
}

.dur {
  color: var(--text-mute);
  font-size: var(--fs-xs);
  text-align: right;
  font-variant-numeric: tabular-nums;
}

.row-actions {
  display: flex;
  gap: 2px;
  align-items: center;
}
.play-btn, .more-btn {
  width: 32px; height: 32px;
  border-radius: 50%;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  font-size: 14px;
  opacity: 0;
  transition: all .15s;
}
.play-btn { color: var(--accent); }
.play-btn:hover { background: var(--accent-soft); transform: scale(1.1); }
.more-btn { color: var(--text-mute); }
.more-btn:hover { background: var(--danger); color: #fff; }
.row:hover .play-btn, .row:hover .more-btn { opacity: 1; }
.row.active .play-btn { opacity: 1; }

.empty-row {
  padding: 60px 20px;
  text-align: center;
  color: var(--text-mute);
  background: var(--bg-elev);
  border-radius: var(--radius-lg);
  border: 1px solid var(--border);
}

@media (max-width: 640px) {
  .row {
    grid-template-columns: 28px 40px 1fr auto;
    gap: 10px;
    padding: 8px 10px;
  }
  .type-badge, .dur { display: none; }
  .row-actions .more-btn { display: none; }
  .play-btn { opacity: 1; }
}
</style>
