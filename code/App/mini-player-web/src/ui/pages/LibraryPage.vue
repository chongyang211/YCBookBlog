<!-- 媒体库页：三 Tab 列表 + 导入入口 + 搜索框
     对应 §03 阶段② + §08 阶段⑦ -->
<script setup lang="ts">
import { ref, computed, onMounted, watch } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import type { MediaItem, MediaType } from '@/types'
import { repository } from '@/data/repository'
import { filesToMedia, walkDirectoryHandle } from '@/data/scanner'
import { usePlayerStore } from '@/service/playerService'
import { debounce } from '@/utils/debounce'
import MediaList from '@/ui/components/MediaList.vue'

const player = usePlayerStore()
const route = useRoute()
const router = useRouter()

type Tab = 'all' | 'audio' | 'video' | 'favorite' | 'history'

const tab = computed<Tab>(() => (route.params.tab as Tab) ?? 'all')
const search = ref('')
const items = ref<MediaItem[]>([])
const scanning = ref(false)
const scanMsg = ref('')
const dropHover = ref(false)
const supportsDirPicker = typeof window.showDirectoryPicker === 'function'

function tabToType(t: Tab): MediaType | undefined {
  if (t === 'audio') return 'audio'
  if (t === 'video') return 'video'
  return undefined
}

async function reload() {
  if (search.value.trim()) {
    items.value = await repository.searchMedia(search.value)
    return
  }
  if (tab.value === 'favorite') items.value = await repository.listFavorites()
  else if (tab.value === 'history') items.value = await repository.listHistory()
  else items.value = await repository.listMedia(tabToType(tab.value))
}

/** search 变化节流重查 */
const reloadDebounced = debounce(reload, 220)
watch(search, () => reloadDebounced())
watch(tab, reload)

onMounted(reload)

/* ---------------- 导入 ---------------- */
const fileInputRef = ref<HTMLInputElement | null>(null)

function pickFiles() { fileInputRef.value?.click() }

async function onFilesPicked(e: Event) {
  const files = Array.from((e.target as HTMLInputElement).files ?? [])
  if (!files.length) return
  await importFiles(files, 'dropped')
  ;(e.target as HTMLInputElement).value = ''
}

async function importFiles(files: File[], source: MediaItem['source']) {
  scanning.value = true
  scanMsg.value = `正在解析 ${files.length} 个文件...`
  try {
    const list = await filesToMedia(files, source)
    if (list.length) await repository.addMediaBatch(list)
    scanMsg.value = `已导入 ${list.length} 条 · 忽略 ${files.length - list.length} 个不支持文件`
    await reload()
  } catch (e) {
    scanMsg.value = `导入失败：${String(e)}`
  } finally {
    scanning.value = false
    setTimeout(() => (scanMsg.value = ''), 3200)
  }
}

async function pickDirectory() {
  if (!window.showDirectoryPicker) {
    alert('当前浏览器不支持选择目录（File System Access API）。请使用「选择文件」或拖拽目录。')
    return
  }
  try {
    const dir = await window.showDirectoryPicker({ mode: 'read' })
    scanning.value = true
    scanMsg.value = '正在扫描目录...'
    const files: File[] = []
    for await (const f of walkDirectoryHandle(dir)) files.push(f)
    await importFiles(files, 'directory')
  } catch (e: unknown) {
    if ((e as { name?: string })?.name !== 'AbortError') {
      console.error(e)
    }
    scanning.value = false
  }
}

/* ---------------- 拖拽 ---------------- */
function onDragOver(e: DragEvent) {
  e.preventDefault()
  dropHover.value = true
}
function onDragLeave() { dropHover.value = false }

async function onDrop(e: DragEvent) {
  e.preventDefault()
  dropHover.value = false
  const dt = e.dataTransfer
  if (!dt) return
  const files = await collectDroppedFiles(dt)
  if (files.length) await importFiles(files, 'dropped')
}

async function collectDroppedFiles(dt: DataTransfer): Promise<File[]> {
  const out: File[] = []
  const items = Array.from(dt.items)
  const dirEntries: FileSystemEntry[] = []

  for (const it of items) {
    const entry = (it as unknown as { webkitGetAsEntry?: () => FileSystemEntry | null })
      .webkitGetAsEntry?.()
    if (entry) {
      if (entry.isDirectory) dirEntries.push(entry)
      else if (entry.isFile) {
        const f = await filePromise(entry as FileSystemFileEntry)
        if (f) out.push(f)
      }
    } else if (it.kind === 'file') {
      const f = it.getAsFile()
      if (f) out.push(f)
    }
  }

  for (const dir of dirEntries) {
    await walkEntry(dir as FileSystemDirectoryEntry, out)
  }
  return out
}

function filePromise(entry: FileSystemFileEntry): Promise<File | null> {
  return new Promise(res => entry.file(f => res(f), () => res(null)))
}

async function walkEntry(entry: FileSystemDirectoryEntry, out: File[]): Promise<void> {
  const reader = entry.createReader()
  // 浏览器一次最多返回 100 条，必须循环读完
  while (true) {
    const batch = await new Promise<FileSystemEntry[]>((res, rej) =>
      reader.readEntries(res, rej),
    )
    if (!batch.length) break
    for (const e of batch) {
      if (e.isFile) {
        const f = await filePromise(e as FileSystemFileEntry)
        if (f) out.push(f)
      } else if (e.isDirectory) {
        await walkEntry(e as FileSystemDirectoryEntry, out)
      }
    }
  }
}

/* ---------------- 列表交互 ---------------- */
function switchTab(t: Tab) {
  router.push(`/library/${t}`)
}

function play(index: number) {
  // 用当前列表作为队列
  player.loadQueue(items.value, index, true)
}
</script>

<template>
  <div class="library">
    <div class="toolbar">
      <div class="tabs">
        <button :class="{ active: tab === 'all' }" @click="switchTab('all')">全部</button>
        <button :class="{ active: tab === 'audio' }" @click="switchTab('audio')">音频</button>
        <button :class="{ active: tab === 'video' }" @click="switchTab('video')">视频</button>
        <span class="sep" />
        <button :class="{ active: tab === 'favorite' }" @click="switchTab('favorite')">我喜欢</button>
        <button :class="{ active: tab === 'history' }" @click="switchTab('history')">历史</button>
      </div>
      <input class="search" placeholder="搜索标题 / 艺术家 / 专辑..." v-model="search" />
      <div class="actions">
        <button class="primary" @click="pickDirectory" :title="supportsDirPicker ? '' : '当前浏览器仅支持文件选择'">
          📁 导入目录
        </button>
        <button @click="pickFiles">📄 导入文件</button>
        <input
          ref="fileInputRef"
          type="file"
          multiple
          accept="audio/*,video/*,.mp3,.flac,.m4a,.wav,.ogg,.mp4,.mkv,.mov,.webm"
          style="display:none"
          @change="onFilesPicked"
        />
      </div>
    </div>

    <div
      class="dropzone"
      :class="{ hover: dropHover }"
      @dragover="onDragOver"
      @dragleave="onDragLeave"
      @drop="onDrop"
    >
      <div v-if="dropHover" class="drop-tip">松开导入</div>

      <MediaList
        :items="items"
        :empty-text="search ? '无匹配结果' : tab === 'favorite' ? '尚未收藏任何曲目' : tab === 'history' ? '暂无播放历史' : '媒体库为空 · 点击右上角导入'"
        @play="play"
      />
    </div>

    <div v-if="scanMsg" class="status" :class="{ running: scanning }">{{ scanMsg }}</div>
  </div>
</template>

<style scoped>
.library {
  padding: 20px;
  display: flex;
  flex-direction: column;
  gap: 14px;
  min-height: 100%;
}
.toolbar {
  display: flex; flex-wrap: wrap; gap: 10px;
  align-items: center;
}
.tabs { display: flex; align-items: center; gap: 4px; flex-wrap: wrap; }
.tabs button {
  padding: 6px 14px;
  border-radius: 999px;
  color: var(--text-sub);
  background: var(--bg-elev);
  font-size: 13px;
}
.tabs button.active { background: var(--accent); color: #fff; }
.tabs .sep { width: 1px; height: 20px; background: var(--border); margin: 0 6px; }

.search {
  flex: 1;
  min-width: 180px;
  padding: 8px 14px;
  background: var(--bg-elev);
  border: 1px solid var(--border);
  border-radius: 999px;
  color: var(--text);
  font-size: 13px;
  outline: none;
}
.search:focus { border-color: var(--accent); }

.actions { display: flex; gap: 8px; }
.actions button {
  padding: 7px 14px;
  background: var(--bg-elev);
  color: var(--text-sub);
  border-radius: 8px;
  font-size: 13px;
}
.actions button:hover { background: var(--hover); color: var(--text); }
.actions button.primary { background: var(--accent); color: #fff; }
.actions button.primary:hover { background: var(--accent-strong); color: #fff; }

.dropzone {
  flex: 1;
  position: relative;
  border-radius: var(--radius);
  transition: outline-color .2s;
  outline: 2px dashed transparent;
  outline-offset: -4px;
}
.dropzone.hover { outline-color: var(--accent); background: var(--accent-soft); }
.drop-tip {
  position: absolute; inset: 0;
  display: flex; align-items: center; justify-content: center;
  color: var(--accent); font-size: 18px; font-weight: 600;
  pointer-events: none; z-index: 2;
}

.status {
  padding: 8px 14px;
  background: var(--bg-elev);
  border-radius: 8px;
  color: var(--text-sub);
  font-size: 12px;
}
.status.running { color: var(--accent); }
</style>
