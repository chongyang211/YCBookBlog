<!-- 当前播放队列抽屉：从右侧滑入 -->
<script setup lang="ts">
import { computed } from 'vue'
import { usePlayerStore } from '@/service/playerService'
import { formatTime } from '@/utils/format'
import CoverArt from './CoverArt.vue'

const props = defineProps<{ open: boolean }>()
const emit = defineEmits<{ (e: 'close'): void }>()
const player = usePlayerStore()

const currentId = computed(() => player.current?.id ?? '')

function play(id: string) {
  const idx = player.queue.findIndex(m => m.id === id)
  if (idx >= 0) player.loadQueue(player.queue, idx, true)
}

function remove(e: Event, id: string) {
  e.stopPropagation()
  player.removeFromQueue(id)
}

function clearAll() {
  if (player.queue.length === 0) return
  if (!confirm('清空当前播放队列？')) return
  player.clearQueue()
}
</script>

<template>
  <Transition name="drawer">
    <div v-if="props.open" class="overlay" @click="emit('close')">
      <aside class="drawer glass" @click.stop>
        <header>
          <div class="title">
            <h3>当前队列</h3>
            <span class="count">{{ player.queue.length }} 项</span>
          </div>
          <div class="actions">
            <button class="link-btn" @click="clearAll" :disabled="player.queue.length === 0">清空</button>
            <button class="close-btn" @click="emit('close')" aria-label="关闭">✕</button>
          </div>
        </header>

        <div class="list" v-if="player.queue.length">
          <div
            v-for="m in player.queue"
            :key="m.id"
            class="row"
            :class="{ active: m.id === currentId }"
            @click="play(m.id)"
          >
            <CoverArt :src="m.coverUrl" :id="m.id" :size="40" />
            <div class="info">
              <div class="title-txt ellipsis">{{ m.title }}</div>
              <div class="sub ellipsis">{{ m.artist }}</div>
            </div>
            <div class="dur">{{ formatTime(m.duration) }}</div>
            <button class="x-btn" @click="remove($event, m.id)" aria-label="移除">✕</button>
          </div>
        </div>
        <div v-else class="empty">
          <div class="emoji">🎵</div>
          <p>队列是空的</p>
          <p class="hint">在媒体库中双击某首歌即可添加</p>
        </div>
      </aside>
    </div>
  </Transition>
</template>

<style scoped>
.overlay {
  position: fixed;
  inset: 0;
  z-index: 999;
  background: rgba(0, 0, 0, 0.35);
  display: flex;
  justify-content: flex-end;
}
.drawer {
  width: 400px;
  max-width: 92vw;
  height: 100%;
  background: var(--glass-bg);
  display: flex;
  flex-direction: column;
  box-shadow: var(--shadow-xl);
}
header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 20px 20px 12px;
  border-bottom: 1px solid var(--border);
}
.title { display: flex; align-items: baseline; gap: 10px; }
.title h3 { margin: 0; }
.count { color: var(--text-mute); font-size: var(--fs-sm); }
.actions { display: flex; align-items: center; gap: 8px; }
.link-btn {
  color: var(--text-sub);
  font-size: var(--fs-sm);
  padding: 4px 10px;
  border-radius: var(--radius-sm);
}
.link-btn:hover:not(:disabled) { background: var(--hover); color: var(--text); }
.link-btn:disabled { opacity: .4; }
.close-btn {
  width: 32px; height: 32px;
  border-radius: 50%;
  color: var(--text-sub);
}
.close-btn:hover { background: var(--hover); color: var(--text); }

.list {
  flex: 1;
  overflow-y: auto;
  padding: 8px 10px;
}
.row {
  display: grid;
  grid-template-columns: 40px 1fr auto auto;
  gap: 12px;
  align-items: center;
  padding: 8px 10px;
  border-radius: var(--radius);
  cursor: pointer;
  transition: background .15s;
}
.row:hover { background: var(--hover); }
.row.active { background: var(--accent-soft); color: var(--accent); }
.row.active .sub { color: var(--accent); opacity: .8; }
.row .info { overflow: hidden; }
.title-txt { font-weight: 500; font-size: var(--fs-base); }
.sub { color: var(--text-mute); font-size: var(--fs-sm); margin-top: 2px; }
.dur { color: var(--text-mute); font-size: var(--fs-xs); font-variant-numeric: tabular-nums; }
.x-btn {
  color: var(--text-mute);
  width: 24px; height: 24px;
  border-radius: 50%;
  opacity: 0;
  transition: opacity .15s, background .15s;
}
.row:hover .x-btn { opacity: 1; }
.x-btn:hover { background: var(--danger); color: #fff; opacity: 1; }

.empty {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 6px;
  flex: 1;
  color: var(--text-mute);
  text-align: center;
  padding: 40px 20px;
}
.empty .emoji { font-size: 48px; margin-bottom: 10px; }
.empty .hint { font-size: var(--fs-sm); color: var(--text-mute); opacity: .7; }

.drawer-enter-active, .drawer-leave-active { transition: all .28s cubic-bezier(.2, .8, .3, 1); }
.drawer-enter-from .drawer, .drawer-leave-to .drawer { transform: translateX(100%); }
.drawer-enter-from, .drawer-leave-to { opacity: 0; }
</style>
