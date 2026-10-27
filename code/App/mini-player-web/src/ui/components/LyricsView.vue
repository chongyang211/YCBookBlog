<!-- LRC 歌词显示：对应 §07 阶段⑥
     CSS transform 滚动 + transition 惯性；点击任意行跳转播放位置 -->
<script setup lang="ts">
import { computed, ref, watch } from 'vue'
import { usePlayerStore } from '@/service/playerService'
import { parseLrc, findLineIndex, type LyricsDocument } from '@/service/lyricsParser'

const player = usePlayerStore()

const lrcText = ref('')
const doc = ref<LyricsDocument>({ lines: [], meta: {}, offset: 0 })

watch(lrcText, (txt) => {
  doc.value = parseLrc(txt)
})

const activeIdx = computed(() =>
  findLineIndex(doc.value.lines, player.position - doc.value.offset),
)

async function onLoadLrc(e: Event) {
  const input = e.target as HTMLInputElement
  const file = input.files?.[0]
  if (!file) return
  lrcText.value = await file.text()
}

function jumpTo(idx: number) {
  const line = doc.value.lines[idx]
  if (line) player.seek(line.time)
}

const translateY = computed(() => {
  // 当前行向中间居中，一行约 36px
  const ROW_H = 36
  return `translateY(${-(activeIdx.value * ROW_H)}px)`
})
</script>

<template>
  <div class="lyrics">
    <div class="toolbar">
      <label class="load">
        加载 LRC
        <input type="file" accept=".lrc,text/plain" @change="onLoadLrc" />
      </label>
      <span v-if="doc.lines.length" class="count">{{ doc.lines.length }} 行</span>
    </div>

    <div class="viewport" v-if="doc.lines.length">
      <ul class="list" :style="{ transform: translateY }">
        <li
          v-for="(line, i) in doc.lines"
          :key="i"
          :class="{ active: i === activeIdx }"
          @click="jumpTo(i)"
        >
          {{ line.text }}
        </li>
      </ul>
    </div>
    <div v-else class="placeholder">暂无歌词 · 加载 .lrc 文件以显示</div>
  </div>
</template>

<style scoped>
.lyrics {
  display: flex;
  flex-direction: column;
  height: 100%;
}
.toolbar {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 10px;
}
.load {
  cursor: pointer;
  color: var(--accent);
  font-size: 13px;
  padding: 4px 10px;
  border-radius: 6px;
  border: 1px solid var(--accent);
}
.load:hover { background: var(--accent-soft); }
.load input { display: none; }
.count { color: var(--text-mute); font-size: 12px; }

.viewport {
  flex: 1;
  overflow: hidden;
  position: relative;
  mask-image: linear-gradient(to bottom, transparent 0%, black 20%, black 80%, transparent 100%);
  -webkit-mask-image: linear-gradient(to bottom, transparent 0%, black 20%, black 80%, transparent 100%);
}
.list {
  position: absolute;
  top: 50%;
  left: 0; right: 0;
  margin: 0;
  padding: 0;
  list-style: none;
  transition: transform .4s cubic-bezier(.25, .8, .5, 1);
}
.list li {
  height: 36px;
  line-height: 36px;
  text-align: center;
  color: var(--text-mute);
  font-size: 14px;
  cursor: pointer;
  transition: color .2s, font-size .2s, font-weight .2s;
}
.list li:hover { color: var(--text-sub); }
.list li.active {
  color: var(--accent);
  font-size: 16px;
  font-weight: 600;
}
.placeholder {
  flex: 1;
  display: flex; align-items: center; justify-content: center;
  color: var(--text-mute);
}
</style>
