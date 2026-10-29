<!-- 媒体库页：三 Tab 列表 + 导入入口 + 搜索框
     对应 §03 阶段② + §08 阶段⑦ -->
<script setup lang="ts">
import { ref, computed, onMounted, watch } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import type { MediaItem, MediaType } from '@/types'
import { repository } from '@/data/repository'
import { filesToMedia, walkDirectoryHandle } from '@/data/scanner'
import { fileCache } from '@/service/fileCache'
import { toast } from '@/service/toast'
import { usePlayerStore } from '@/service/playerService'
import { debounce } from '@/utils/debounce'
import MediaList from '@/ui/components/MediaList.vue'
import EmptyState from '@/ui/components/EmptyState.vue'

const player = usePlayerStore()
const route = useRoute()
const router = useRouter()

type Tab = 'all' | 'audio' | 'video' | 'favorite' | 'history'

const TAB_LABELS: Record<Tab, string> = {
  all: '全部',
  audio: '音频',
  video: '视频',
  favorite: '我喜欢',
  history: '最近播放',
}

const tab = computed<Tab>(() => (route.params.tab as Tab) ?? 'all')
const search = ref('')
const items = ref<MediaItem[]>([])
const scanning = ref(false)
const dropHover = ref(false)
const totalCount = ref(0)
const supportsDirPicker = typeof window.showDirectoryPicker === 'function'

function tabToType(t: Tab): MediaType | undefined {
  if (t === 'audio') return 'audio'
  if (t === 'video') return 'video'
  return undefined
}

async function reload() {
  if (search.value.trim()) {
    items.value = fileCache.hydrateAll(await repository.searchMedia(search.value))
    return
  }
  if (tab.value === 'favorite') {
    items.value = fileCache.hydrateAll(await repository.listFavorites())
  } else if (tab.value === 'history') {
    items.value = fileCache.hydrateAll(await repository.listHistory())
  } else {
    items.value = fileCache.hydrateAll(await repository.listMedia(tabToType(tab.value)))
  }
  totalCount.value = (await repository.listMedia()).length
}

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
  const total = files.length
  const toastId = toast.info(`正在解析 ${total} 个文件...`, 0)
  try {
    const list = await filesToMedia(files, source)
    if (list.length) await repository.addMediaBatch(list)
    toast.dismiss(toastId)
    const ignored = total - list.length
    if (list.length) {
      toast.success(
        ignored > 0
          ? `已导入 ${list.length} 条，忽略 ${ignored} 个不支持文件`
          : `已导入 ${list.length} 条曲目`,
        2600,
      )
    } else {
      toast.warn('未找到支持的音视频文件')
    }
    await reload()
  } catch (e) {
    toast.dismiss(toastId)
    toast.error(`导入失败：${String(e)}`)
  } finally {
    scanning.value = false
  }
}

async function pickDirectory() {
  if (!supportsDirPicker) {
    toast.warn('当前浏览器不支持目录选择，请使用「选择文件」或拖拽')
    return
  }
  try {
    const dir = await window.showDirectoryPicker!({ mode: 'read' })
    scanning.value = true
    const toastId = toast.info('正在扫描目录...', 0)
    const files: File[] = []
    for await (const f of walkDirectoryHandle(dir)) files.push(f)
    toast.dismiss(toastId)
    await importFiles(files, 'directory')
  } catch (e: unknown) {
    if ((e as { name?: string })?.name !== 'AbortError') {
      console.error(e)
      toast.error('目录读取失败')
    }
    scanning.value = false
  }
}

/* ---------------- 拖拽 ---------------- */
function onDragOver(e: DragEvent) {
  e.preventDefault()
  dropHover.value = true
}
function onDragLeave(e: DragEvent) {
  // 只有真正离开根 dropzone 才清除
  if (e.currentTarget === e.target) dropHover.value = false
}

async function onDrop(e: DragEvent) {
  e.preventDefault()
  dropHover.value = false
  const dt = e.dataTransfer
  if (!dt) return
  const files = await collectDroppedFiles(dt)
  if (files.length) await importFiles(files, 'dropped')
  else toast.warn('拖入的文件未识别')
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
  const target = items.value[index]
  if (!target) return
  if (!target.file && !fileCache.has(target.id) && !target.externalUrl) {
    toast.warn(`"${target.title}" 文件已失效，请重新导入`)
    return
  }
  player.loadQueue(items.value, index, true)
}

const emptyIsFullLibrary = computed(
  () => totalCount.value === 0 && !search.value.trim() && tab.value !== 'favorite' && tab.value !== 'history',
)

const emptyText = computed(() => {
  if (search.value) return `无匹配 "${search.value}" 的结果`
  if (tab.value === 'favorite') return '尚未收藏任何曲目'
  if (tab.value === 'history') return '暂无播放历史'
  if (tab.value === 'audio') return '媒体库中还没有音频'
  if (tab.value === 'video') return '媒体库中还没有视频'
  return '媒体库为空'
})
</script>

<template>
  <div
    class="library"
    :class="{ 'drag-over': dropHover }"
    @dragover="onDragOver"
    @dragleave="onDragLeave"
    @drop="onDrop"
  >
    <!-- 顶部：标题 + 搜索 -->
    <header class="page-header">
      <div class="title-area">
        <h2>{{ TAB_LABELS[tab] }}</h2>
        <span class="count" v-if="items.length">{{ items.length }} 项</span>
      </div>
      <div class="header-actions">
        <input
          type="search"
          class="search"
          placeholder="搜索标题 / 艺术家 / 专辑..."
          v-model="search"
          aria-label="搜索"
        />
        <button class="btn-primary" @click="pickDirectory" :disabled="scanning">
          <span>📁</span>
          <span class="btn-text">导入目录</span>
        </button>
        <button class="btn-secondary" @click="pickFiles" :disabled="scanning">
          <span>📄</span>
          <span class="btn-text">导入文件</span>
        </button>
        <input
          ref="fileInputRef"
          type="file"
          multiple
          accept="audio/*,video/*,.mp3,.flac,.m4a,.wav,.ogg,.mp4,.mkv,.mov,.webm"
          style="display:none"
          @change="onFilesPicked"
        />
      </div>
    </header>

    <!-- Tab 切换 -->
    <div class="tabs">
      <button :class="{ active: tab === 'all' }" @click="switchTab('all')">全部</button>
      <button :class="{ active: tab === 'audio' }" @click="switchTab('audio')">
        <span class="dot" />音频
      </button>
      <button :class="{ active: tab === 'video' }" @click="switchTab('video')">
        <span class="dot" />视频
      </button>
      <span class="tab-sep" />
      <button :class="{ active: tab === 'favorite' }" @click="switchTab('favorite')">♥ 我喜欢</button>
      <button :class="{ active: tab === 'history' }" @click="switchTab('history')">⟲ 最近</button>
    </div>

    <!-- 列表区 / 空库引导 -->
    <div class="list-area">
      <EmptyState
        v-if="emptyIsFullLibrary"
        @importDir="pickDirectory"
        @importFile="pickFiles"
      />
      <MediaList
        v-else
        :items="items"
        :empty-text="emptyText"
        @play="play"
        @reload="reload"
      />
    </div>

    <!-- 拖拽覆盖提示层 -->
    <Transition name="drop-fade">
      <div v-if="dropHover" class="drop-overlay">
        <div class="drop-tip glass">
          <div class="emoji">📁</div>
          <div>松开导入</div>
        </div>
      </div>
    </Transition>
  </div>
</template>

<style scoped>
.library {
  padding: 24px;
  padding-bottom: 20px;
  display: flex;
  flex-direction: column;
  gap: 16px;
  min-height: 100%;
  position: relative;
}

/* ---- 顶部 ---- */
.page-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 16px;
  flex-wrap: wrap;
}
.title-area {
  display: flex;
  align-items: baseline;
  gap: 10px;
}
.title-area h2 {
  background: linear-gradient(135deg, var(--text) 0%, var(--text-sub) 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}
.count { color: var(--text-mute); font-size: var(--fs-sm); }

.header-actions {
  display: flex;
  align-items: center;
  gap: 10px;
  flex: 1;
  justify-content: flex-end;
  min-width: 0;
}
.search {
  flex: 1;
  max-width: 320px;
  min-width: 120px;
  padding: 8px 16px;
  background: var(--bg-elev);
  border: 1px solid var(--border);
  border-radius: var(--radius-full);
  color: var(--text);
  font-size: var(--fs-sm);
}
.search:focus {
  border-color: var(--accent);
  box-shadow: 0 0 0 3px var(--accent-soft);
}

.btn-primary, .btn-secondary {
  display: inline-flex;
  align-items: center;
  gap: 6px;
  padding: 8px 16px;
  border-radius: var(--radius-full);
  font-size: var(--fs-sm);
  font-weight: 500;
  white-space: nowrap;
  transition: all .18s;
}
.btn-primary {
  background: linear-gradient(135deg, var(--brand-1), var(--brand-2));
  color: #fff;
  box-shadow: 0 4px 14px rgba(139, 92, 246, 0.35);
}
.btn-primary:hover:not(:disabled) {
  transform: translateY(-1px);
  box-shadow: 0 8px 20px rgba(139, 92, 246, 0.45);
}
.btn-primary:disabled { opacity: .5; cursor: not-allowed; }
.btn-secondary {
  background: var(--bg-elev);
  color: var(--text-sub);
  border: 1px solid var(--border);
}
.btn-secondary:hover:not(:disabled) {
  background: var(--hover);
  color: var(--text);
}

/* ---- Tab ---- */
.tabs {
  display: flex;
  align-items: center;
  gap: 4px;
  flex-wrap: wrap;
}
.tabs button {
  padding: 6px 14px;
  border-radius: var(--radius-full);
  color: var(--text-sub);
  background: transparent;
  font-size: var(--fs-sm);
  font-weight: 500;
  transition: all .15s;
  display: inline-flex;
  align-items: center;
  gap: 6px;
}
.tabs button:hover:not(.active) {
  background: var(--hover);
  color: var(--text);
}
.tabs button.active {
  background: var(--accent-soft);
  color: var(--accent);
  font-weight: 600;
}
.tabs .dot {
  width: 6px; height: 6px;
  background: currentColor;
  border-radius: 50%;
  opacity: .5;
}
.tab-sep {
  width: 1px; height: 16px;
  background: var(--border);
  margin: 0 6px;
}

/* ---- 列表区 ---- */
.list-area {
  flex: 1;
  min-height: 300px;
  animation: fade-in .3s ease;
}

/* ---- 拖拽覆盖层 ---- */
.library.drag-over { outline: 2px dashed var(--accent); outline-offset: -8px; border-radius: var(--radius-lg); }
.drop-overlay {
  position: absolute;
  inset: 20px;
  z-index: 100;
  display: flex;
  align-items: center;
  justify-content: center;
  pointer-events: none;
  background: rgba(139, 92, 246, 0.08);
  border-radius: var(--radius-lg);
}
.drop-tip {
  padding: 40px 60px;
  border-radius: var(--radius-lg);
  text-align: center;
  color: var(--accent);
  font-size: var(--fs-xl);
  font-weight: 600;
  box-shadow: var(--shadow-xl);
}
.drop-tip .emoji { font-size: 48px; margin-bottom: 12px; }
.drop-fade-enter-active, .drop-fade-leave-active { transition: all .18s; }
.drop-fade-enter-from, .drop-fade-leave-to { opacity: 0; transform: scale(.96); }

/* ---- 响应式 ---- */
@media (max-width: 820px) {
  .btn-text { display: none; }
  .btn-primary, .btn-secondary { padding: 8px 12px; }
  .search { max-width: 180px; }
}
@media (max-width: 640px) {
  .library { padding: 16px 12px; }
  .page-header { gap: 10px; }
  .search { max-width: none; min-width: 100%; order: 10; }
  .header-actions { flex-wrap: wrap; gap: 8px; }
}
</style>
