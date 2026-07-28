<template>
  <div class="dt-panel">
    <div class="dt-toolbar">
      <div class="dt-field">
        <label>模式</label>
        <select v-model="mode">
          <option value="merge">合并 PDF</option>
          <option value="split">拆分 PDF</option>
        </select>
      </div>
      <div class="dt-actions">
        <button class="dt-btn" @click="clearAll" :disabled="busy">清空</button>
        <button
          v-if="mode === 'merge'"
          class="dt-btn dt-btn-primary"
          :disabled="files.length < 2 || busy"
          @click="doMerge"
        >{{ busy ? '处理中...' : '▷ 合并并下载' }}</button>
        <button
          v-else
          class="dt-btn dt-btn-primary"
          :disabled="!splitFile || selectedPages.length === 0 || busy"
          @click="doSplit"
        >{{ busy ? '处理中...' : `▷ 提取 ${selectedPages.length} 页并下载` }}</button>
      </div>
    </div>

    <!-- 合并模式 -->
    <div v-if="mode === 'merge'">
      <label
        class="dt-pdf-box"
        :class="{ dragover }"
        @dragover.prevent="dragover = true"
        @dragleave="dragover = false"
        @drop.prevent="onDrop"
      >
        <div class="dt-pdf-box-icon">📄</div>
        <div class="dt-pdf-box-tip">
          点击或拖拽多个 <strong>PDF 文件</strong> 到此处
        </div>
        <input
          type="file"
          accept="application/pdf"
          multiple
          style="display:none"
          @change="onPick"
        />
      </label>

      <ul v-if="files.length" class="dt-file-list">
        <li v-for="(f, idx) in files" :key="idx" class="dt-file-row">
          <span class="dt-file-name">{{ idx + 1 }}. {{ f.name }}</span>
          <span class="dt-file-meta">{{ humanSize(f.size) }}</span>
          <span class="dt-file-op">
            <button class="dt-mini" :disabled="idx === 0" @click="move(idx, -1)">↑</button>
            <button class="dt-mini" :disabled="idx === files.length - 1" @click="move(idx, 1)">↓</button>
            <button class="dt-mini" @click="remove(idx)">✕</button>
          </span>
        </li>
      </ul>

      <p v-if="files.length === 1" class="dt-info">至少选择 2 个 PDF 文件再进行合并。</p>
    </div>

    <!-- 拆分模式 -->
    <div v-else>
      <label
        class="dt-pdf-box"
        :class="{ dragover }"
        @dragover.prevent="dragover = true"
        @dragleave="dragover = false"
        @drop.prevent="onDropSingle"
      >
        <div class="dt-pdf-box-icon">📄</div>
        <div class="dt-pdf-box-tip">
          点击或拖拽 <strong>单个 PDF</strong> 到此处
        </div>
        <input
          type="file"
          accept="application/pdf"
          style="display:none"
          @change="onPickSingle"
        />
      </label>

      <ul v-if="splitFile" class="dt-file-list">
        <li class="dt-file-row">
          <span class="dt-file-name">{{ splitFile.name }}</span>
          <span class="dt-file-meta">{{ totalPages }} 页 · {{ humanSize(splitFile.size) }}</span>
          <span class="dt-file-op">
            <button class="dt-mini" @click="selectAll">全选</button>
            <button class="dt-mini" @click="selectedPages = []">清空</button>
          </span>
        </li>
      </ul>
      <div v-if="totalPages > 0" class="dt-page-grid">
        <div
          v-for="p in totalPages"
          :key="p"
          class="dt-page-cell"
          :class="{ selected: selectedPages.indexOf(p) >= 0 }"
          @click="togglePage(p)"
        >第 {{ p }} 页</div>
      </div>
      <p v-if="splitFile && selectedPages.length === 0" class="dt-info">点击页码选择需要提取的页面。</p>
    </div>

    <p v-if="err" class="dt-err">⚠ {{ err }}</p>
  </div>
</template>

<script>
import { loadScript, downloadFile } from './utils.js'

export default {
  name: 'PdfPanel',
  data() {
    return {
      mode: 'merge',
      files: [],
      dragover: false,
      busy: false,
      err: '',
      splitFile: null,
      totalPages: 0,
      selectedPages: [],
    }
  },
  methods: {
    humanSize(b) {
      if (!b) return ''
      if (b < 1024) return b + ' B'
      if (b < 1024 * 1024) return (b / 1024).toFixed(1) + ' KB'
      return (b / 1024 / 1024).toFixed(2) + ' MB'
    },
    onPick(e) { this.addFiles(e.target.files); e.target.value = '' },
    onDrop(e) { this.dragover = false; this.addFiles(e.dataTransfer.files) },
    addFiles(list) {
      this.err = ''
      const arr = Array.from(list || []).filter(f => /pdf$/i.test(f.type) || /\.pdf$/i.test(f.name))
      if (arr.length === 0) { this.err = '请选择 PDF 文件'; return }
      this.files = this.files.concat(arr)
    },
    move(idx, dir) {
      const j = idx + dir
      if (j < 0 || j >= this.files.length) return
      const arr = this.files.slice()
      const t = arr[idx]; arr[idx] = arr[j]; arr[j] = t
      this.files = arr
    },
    remove(idx) { this.files.splice(idx, 1) },
    clearAll() {
      this.files = []
      this.splitFile = null
      this.totalPages = 0
      this.selectedPages = []
      this.err = ''
    },

    async ensureLib() {
      try {
        await loadScript('pdf-lib')
        return window.PDFLib
      } catch (e) {
        throw new Error('pdf-lib 加载失败，请检查网络')
      }
    },

    async doMerge() {
      if (this.files.length < 2) return
      this.busy = true; this.err = ''
      try {
        const PDFLib = await this.ensureLib()
        const merged = await PDFLib.PDFDocument.create()
        for (const f of this.files) {
          const buf = await f.arrayBuffer()
          const src = await PDFLib.PDFDocument.load(buf)
          const pages = await merged.copyPages(src, src.getPageIndices())
          pages.forEach(p => merged.addPage(p))
        }
        const bytes = await merged.save()
        downloadFile(new Blob([bytes], { type: 'application/pdf' }), 'merged.pdf')
      } catch (e) {
        this.err = '合并失败：' + e.message
      } finally {
        this.busy = false
      }
    },

    async onPickSingle(e) { await this.setSplitFile((e.target.files || [])[0]); e.target.value = '' },
    async onDropSingle(e) { this.dragover = false; await this.setSplitFile((e.dataTransfer.files || [])[0]) },
    async setSplitFile(f) {
      this.err = ''
      if (!f) return
      if (!/pdf$/i.test(f.type) && !/\.pdf$/i.test(f.name)) { this.err = '请选择 PDF 文件'; return }
      this.splitFile = f
      this.selectedPages = []
      try {
        const PDFLib = await this.ensureLib()
        const buf = await f.arrayBuffer()
        const src = await PDFLib.PDFDocument.load(buf)
        this.totalPages = src.getPageCount()
      } catch (e) {
        this.err = '读取失败：' + e.message
        this.totalPages = 0
      }
    },
    togglePage(p) {
      const i = this.selectedPages.indexOf(p)
      if (i >= 0) this.selectedPages.splice(i, 1)
      else { this.selectedPages.push(p); this.selectedPages.sort((a, b) => a - b) }
    },
    selectAll() {
      this.selectedPages = Array.from({ length: this.totalPages }, (_, i) => i + 1)
    },
    async doSplit() {
      if (!this.splitFile || this.selectedPages.length === 0) return
      this.busy = true; this.err = ''
      try {
        const PDFLib = await this.ensureLib()
        const buf = await this.splitFile.arrayBuffer()
        const src = await PDFLib.PDFDocument.load(buf)
        const out = await PDFLib.PDFDocument.create()
        const idxList = this.selectedPages.map(p => p - 1)
        const pages = await out.copyPages(src, idxList)
        pages.forEach(p => out.addPage(p))
        const bytes = await out.save()
        const base = this.splitFile.name.replace(/\.pdf$/i, '')
        downloadFile(new Blob([bytes], { type: 'application/pdf' }), `${base}-extracted.pdf`)
      } catch (e) {
        this.err = '拆分失败：' + e.message
      } finally {
        this.busy = false
      }
    },
  },
}
</script>
