<!-- LRC 歌词显示：对应 §07 阶段⑥
     CSS transform 滚动 + transition 惯性；点击任意行跳转播放位置 -->
<script setup lang="ts">
import { computed, ref, watch } from 'vue'
import { usePlayerStore } from '@/service/playerService'
import { parseLrc, findLineIndex, type LyricsDocument } from '@/service/lyricsParser'
import { toast } from '@/service/toast'

const player = usePlayerStore()

const lrcText = ref('')
const doc = ref<LyricsDocument>({ lines: [], meta: {}, offset: 0 })

watch(lrcText, (txt) => {
  doc.value = parseLrc(txt)
})

// 曲目变化时清空上一首的歌词
watch(() => player.current?.id, () => {
  lrcText.value = ''
  doc.value = { lines: [], meta: {}, offset: 0 }
})

const activeIdx = computed(() =>
  findLineIndex(doc.value.lines, player.position - doc.value.offset),
)

async function onLoadLrc(e: Event) {
  const input = e.target as HTMLInputElement
  const file = input.files?.[0]
  if (!file) return
  try {
    lrcText.value = await file.text()
    toast.success(`已加载歌词 (${doc.value.lines.length} 行)`, 1600)
  } catch {
    toast.error('歌词加载失败')
  }
  input.value = ''
}

function jumpTo(idx: number) {
  const line = doc.value.lines[idx]
  if (line) player.seek(line.time)
}

const translateY = computed(() => {
  // 当前行向中间居中，一行约 44px
  const ROW_H = 44
  return `translateY(${-(activeIdx.value * ROW_H)}px)`
})
</script>

<template>
  <div class="lyrics">
    <div class="toolbar">
      <label class="load">
        <input type="file" accept=".lrc,text/plain" @change="onLoadLrc" />
        <span>📄 加载 LRC</span>
      </label>
      <span v-if="doc.lines.length" class="count">{{ doc.lines.length }} 行</span>
    </div>

    <div class="viewport" v-if="doc.lines.length">
      <ul class="list" :style="{ transform: translateY }">
        <li
          v-for="(line, i) in doc.lines"
          :key="i"
          :class="{ active: i === activeIdx, past: i < activeIdx }"
          @click="jumpTo(i)"
        >
          {{ line.text || '♪' }}
        </li>
      </ul>
    </div>
    <div v-else class="placeholder">
      <div class="icon">♪</div>
      <p>暂无歌词</p>
      <p class="hint">加载 .lrc 文件以同步显示</p>
    </div>
  </div>
</template>

<style scoped>
.lyrics {
  display: flex;
  flex-direction: column;
  height: 100%;
  position: relative;
}
.toolbar {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 10px;
  flex-shrink: 0;
}
.load {
  cursor: pointer;
  color: var(--accent);
  font-size: var(--fs-sm);
  padding: 6px 14px;
  border-radius: var(--radius-full);
  border: 1px solid var(--accent);
  background: var(--accent-soft);
  transition: all .15s;
}
.load:hover {
  background: var(--accent);
  color: #fff;
}
.load input { display: none; }
.count { color: var(--text-mute); font-size: var(--fs-xs); }

.viewport {
  flex: 1;
  overflow: hidden;
  position: relative;
  mask-image: linear-gradient(to bottom, transparent 0%, black 18%, black 82%, transparent 100%);
  -webkit-mask-image: linear-gradient(to bottom, transparent 0%, black 18%, black 82%, transparent 100%);
}
.list {
  position: absolute;
  top: 50%;
  left: 0; right: 0;
  margin: 0;
  padding: 0;
  list-style: none;
  transition: transform .55s cubic-bezier(.25, .8, .5, 1);
}
.list li {
  height: 44px;
  line-height: 44px;
  text-align: center;
  color: var(--text-mute);
  font-size: var(--fs-md);
  cursor: pointer;
  transition: all .3s cubic-bezier(.25, .8, .5, 1);
  user-select: none;
  padding: 0 16px;
}
.list li:hover { color: var(--text-sub); }
.list li.past { opacity: .5; }
.list li.active {
  color: var(--text);
  font-size: var(--fs-xl);
  font-weight: 700;
  text-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
}

.placeholder {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 6px;
  color: var(--text-mute);
}
.placeholder .icon {
  font-size: 48px;
  color: var(--text-mute);
  opacity: .3;
  margin-bottom: 8px;
}
.placeholder .hint { font-size: var(--fs-sm); opacity: .7; }
</style>
