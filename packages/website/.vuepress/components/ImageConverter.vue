<template>
  <div class="ic-wrapper">
    <!-- Header -->
    <div class="ic-header">
      <h2 class="ic-title">图片格式互转 · 拖拽即可</h2>
      <p class="ic-desc">浏览器端处理，不上传服务器，支持批量转换实时下载</p>
    </div>

    <!-- Toolbar -->
    <div class="ic-toolbar">
      <div class="ic-field">
        <label>目标格式</label>
        <select v-model="targetFormat">
          <option v-for="fmt in formats" :key="fmt.value" :value="fmt.value">{{ fmt.label }}</option>
        </select>
      </div>
      <div class="ic-field" v-if="hasQuality">
        <label>质量</label>
        <input type="range" v-model.number="quality" min="10" max="100" step="5" />
        <span class="ic-qval">{{ quality }}%</span>
      </div>
      <div class="ic-actions">
        <button class="ic-btn ic-btn-sel" @click="selectAll">{{ allSelected ? '取消全选' : '全选' }}</button>
        <button class="ic-btn ic-btn-dl" :disabled="downloadQueue.length === 0" @click="downloadAll">
          {{ downloading ? '转换中...' : `批量下载 (${downloadQueue.length})` }}
        </button>
        <button class="ic-btn ic-btn-zip" :disabled="downloadQueue.length === 0" @click="downloadZip" v-if="downloadQueue.length > 0">
          打包 ZIP
        </button>
        <button class="ic-btn ic-btn-del" :disabled="downloadQueue.length === 0" @click="clearAll">清空</button>
      </div>
    </div>

    <!-- Drop Zone -->
    <div
      class="ic-dropzone"
      :class="{ 'ic-dragover': isDragover }"
      @dragover.prevent="onDragOver"
      @dragleave.prevent="onDragLeave"
      @drop.prevent="onDrop"
      @click="openFilePicker"
    >
      <div class="ic-dropzone-inner">
        <div class="ic-dropicon">
          <svg viewBox="0 0 24 24" width="48" height="48" fill="none" stroke="currentColor" stroke-width="1.5">
            <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"/>
            <polyline points="17,8 12,3 7,8"/>
            <line x1="12" y1="3" x2="12" y2="15"/>
          </svg>
        </div>
        <p class="ic-droptext">拖拽图片到此区域，或点击选择文件</p>
        <p class="ic-drophint">
          支持 JPG · PNG · GIF · SVG · ICO · BMP · WebP · AVIF
        </p>
      </div>
    </div>

    <!-- Image List -->
    <div class="ic-list" v-if="images.length > 0">
      <transition-group name="ic-card">
        <div class="ic-card" v-for="img in images" :key="img.id">
          <div class="ic-card-check" @click.stop="img.selected = !img.selected">
            <span class="ic-checkbox" :class="{ checked: img.selected }"></span>
          </div>
          <div class="ic-card-preview" @click="previewImage(img)">
            <img :src="img.thumb" :alt="img.name" />
          </div>
          <div class="ic-card-info">
            <p class="ic-card-name" :title="img.name">{{ img.name }}</p>
            <p class="ic-card-meta">
              <span>{{ img.srcFormat.toUpperCase() }}</span>
              <span>{{ formatSize(img.srcSize) }}</span>
              <span>{{ img.width }} × {{ img.height }}</span>
            </p>
            <p class="ic-card-meta" v-if="img.outSize > 0">
              <span class="ic-arrow">→</span>
              <span>{{ targetFormat.toUpperCase() }}</span>
              <span>{{ formatSize(img.outSize) }}</span>
              <span class="ic-ratio" :class="img.outSize < img.srcSize ? 'ic-smaller' : 'ic-larger'">
                {{ sizeChange(img) }}
              </span>
            </p>
            <p class="ic-card-err" v-if="img.error">{{ img.error }}</p>
          </div>
          <div class="ic-card-actions">
            <button class="ic-card-btn ic-card-convert" v-if="!img.done && !img.converting" @click="convertOne(img)">转换</button>
            <span class="ic-spinner" v-if="img.converting"></span>
            <button class="ic-card-btn ic-card-dl" v-if="img.done" @click="downloadOne(img)">下载</button>
          </div>
        </div>
      </transition-group>
    </div>

    <!-- Progress bar -->
    <div class="ic-progress" v-if="convertingBatch">
      <div class="ic-progress-bar">
        <div class="ic-progress-fill" :style="{ width: progressPct + '%' }"></div>
      </div>
      <span class="ic-progress-text">{{ progressDone }} / {{ progressTotal }}</span>
    </div>

    <!-- Stats -->
    <div class="ic-stats" v-if="totalSrcSize > 0">
      <span>原始: {{ formatSize(totalSrcSize) }}</span>
      <span v-if="totalOutSize > 0">→ 转换后: {{ formatSize(totalOutSize) }}</span>
      <span v-if="totalOutSize > 0 && totalSrcSize > 0" class="ic-ratio" :class="totalOutSize < totalSrcSize ? 'ic-smaller' : 'ic-larger'">
        {{ totalSizeChange }}
      </span>
    </div>

    <!-- Preview Modal -->
    <transition name="ic-modal">
      <div class="ic-modal" v-if="preview" @click.self="preview = null">
        <div class="ic-modal-inner">
          <img :src="preview.thumb" :alt="preview.name" />
          <button class="ic-modal-close" @click="preview = null">✕</button>
        </div>
      </div>
    </transition>

    <!-- Hidden file input -->
    <input ref="fileInput" type="file" multiple accept="image/*,.svg,.ico,.bmp,.webp,.avif" style="display:none" @change="onFileChange" />
  </div>
</template>

<script>
const FORMAT_CFG = [
  { value: 'webp',  label: 'WebP（推荐·体积最小）', mime: 'image/webp',  ext: '.webp' },
  { value: 'jpeg',  label: 'JPEG（照片适用）',       mime: 'image/jpeg', ext: '.jpg' },
  { value: 'png',   label: 'PNG（透明支持）',        mime: 'image/png',  ext: '.png' },
  { value: 'avif',  label: 'AVIF（新一代·高压缩）',   mime: 'image/avif', ext: '.avif' },
  { value: 'gif',   label: 'GIF（动画·首帧）',        mime: 'image/gif',  ext: '.gif' },
  { value: 'bmp',   label: 'BMP（位图）',            mime: 'image/bmp',  ext: '.bmp' },
  { value: 'ico',   label: 'ICO（图标）',            mime: 'image/x-icon', ext: '.ico' },
]

let uid = 0
function nextId() {
  return 'img_' + (++uid) + '_' + Date.now()
}

export default {
  name: 'ImageConverter',

  data() {
    return {
      formats: FORMAT_CFG,
      targetFormat: 'webp',
      quality: 80,
      images: [],
      isDragover: false,
      preview: null,
      downloading: false,
      convertingBatch: false,
      progressDone: 0,
      progressTotal: 0,
      autoConvert: true,
    }
  },

  computed: {
    hasQuality() {
      return ['webp', 'jpeg', 'avif'].includes(this.targetFormat)
    },
    downloadQueue() {
      return this.images.filter(i => i.done && i.selected)
    },
    allSelected() {
      if (this.images.length === 0) return false
      return this.images.every(i => i.selected)
    },
    progressPct() {
      if (this.progressTotal === 0) return 0
      return Math.round((this.progressDone / this.progressTotal) * 100)
    },
    totalSrcSize() {
      return this.images.reduce((s, i) => s + i.srcSize, 0)
    },
    totalOutSize() {
      return this.images.filter(i => i.done).reduce((s, i) => s + i.outSize, 0)
    },
    totalSizeChange() {
      if (this.totalSrcSize === 0) return ''
      const pct = ((this.totalOutSize - this.totalSrcSize) / this.totalSrcSize * 100).toFixed(1)
      return pct > 0 ? `+${pct}%` : `${pct}%`
    },
  },

  methods: {
    openFilePicker() {
      this.$refs.fileInput.click()
    },

    onDragOver() {
      this.isDragover = true
    },

    onDragLeave() {
      this.isDragover = false
    },

    onDrop(e) {
      this.isDragover = false
      const files = Array.from(e.dataTransfer.files).filter(f => f.type.startsWith('image/') || /\.(svg|ico|bmp|webp|avif)$/i.test(f.name))
      if (files.length === 0) {
        alert('请拖入图片文件')
        return
      }
      this.addFiles(files)
    },

    onFileChange(e) {
      const files = Array.from(e.target.files)
      if (files.length > 0) {
        this.addFiles(files)
      }
      e.target.value = ''
    },

    addFiles(files) {
      for (const file of files) {
        const format = this.detectFormat(file.name)
        const id = nextId()
        const img = {
          id,
          file,
          name: file.name,
          srcFormat: format,
          srcSize: file.size,
          width: 0,
          height: 0,
          thumb: '',
          selected: true,
          done: false,
          converting: false,
          outSize: 0,
          outBlob: null,
          error: '',
        }
        this.images.push(img)
        this.makeThumb(img)
      }
      if (this.autoConvert && files.length > 0) {
        this.$nextTick(() => this.convertAllUnconverted())
      }
    },

    detectFormat(name) {
      const ext = name.split('.').pop().toLowerCase()
      if (ext === 'jpg' || ext === 'jpeg') return 'jpeg'
      if (ext === 'avif') return 'avif'
      if (ext === 'ico') return 'ico'
      if (ext === 'bmp') return 'bmp'
      if (ext === 'webp') return 'webp'
      if (ext === 'svg') return 'svg'
      if (ext === 'gif') return 'gif'
      return 'png'
    },

    makeThumb(img) {
      const url = URL.createObjectURL(img.file)
      const imageEl = new Image()
      imageEl.onload = () => {
        const maxW = 260
        const maxH = 180
        let w = imageEl.naturalWidth
        let h = imageEl.naturalHeight
        img.width = w
        img.height = h
        if (w > maxW || h > maxH) {
          const ratio = Math.min(maxW / w, maxH / h)
          w = Math.round(w * ratio)
          h = Math.round(h * ratio)
        }
        const canvas = document.createElement('canvas')
        canvas.width = w
        canvas.height = h
        const ctx = canvas.getContext('2d')
        ctx.drawImage(imageEl, 0, 0, w, h)
        img.thumb = canvas.toDataURL('image/jpeg', 0.7)
        URL.revokeObjectURL(url)
      }
      imageEl.onerror = () => {
        img.error = '缩略图生成失败'
        img.thumb = ''
        URL.revokeObjectURL(url)
      }
      imageEl.src = url
    },

    async convertAllUnconverted() {
      const pending = this.images.filter(i => !i.done)
      if (pending.length === 0) return
      this.convertingBatch = true
      this.progressDone = 0
      this.progressTotal = pending.length
      for (const img of pending) {
        await this.convertOne(img)
        this.progressDone++
      }
      this.convertingBatch = false
    },

    async convertOne(img) {
      if (img.converting || img.done) return
      img.converting = true
      img.error = ''
      try {
        const { blob, size } = await this.doConvert(img.file, this.targetFormat, this.quality)
        img.outSize = size
        img.outBlob = blob
        img.done = true
      } catch (err) {
        img.error = '转换失败：' + (err.message || '未知错误')
      }
      img.converting = false
    },

    doConvert(file, targetFormat, quality) {
      return new Promise((resolve, reject) => {
        const cfg = FORMAT_CFG.find(f => f.value === targetFormat)
        const mime = cfg.mime

        const url = URL.createObjectURL(file)
        const img = new Image()

        img.onload = () => {
          URL.revokeObjectURL(url)

          // ICO 尺寸限制：浏览器加载 ICO 得到的尺寸可能为 0
          const w = img.naturalWidth || 256
          const h = img.naturalHeight || 256
          if (w === 0 && h === 0) {
            // 无法获取尺寸，使用默认值
            const fallbackCanvas = document.createElement('canvas')
            fallbackCanvas.width = 256
            fallbackCanvas.height = 256
            fallbackCanvas.toBlob(
              b => b ? resolve({ blob: b, size: b.size }) : reject(new Error('Canvas 转换失败')),
              mime, quality / 100
            )
            return
          }

          const canvas = document.createElement('canvas')
          canvas.width = w
          canvas.height = h
          const ctx = canvas.getContext('2d')

          // 绘制前清除画布
          ctx.clearRect(0, 0, w, h)

          // 对于有透明通道的格式，确保背景处理
          if (targetFormat === 'jpeg' || targetFormat === 'bmp') {
            ctx.fillStyle = '#FFFFFF'
            ctx.fillRect(0, 0, w, h)
          }

          ctx.drawImage(img, 0, 0, w, h)

          // 尝试用指定 MIME 类型导出
          if (typeof canvas.toBlob !== 'function') {
            reject(new Error('浏览器不支持 Canvas.toBlob'))
            return
          }

          canvas.toBlob(
            (blob) => {
              if (blob && blob.size > 0) {
                resolve({ blob, size: blob.size })
              } else if (targetFormat !== 'png') {
                // 降级：尝试 PNG
                canvas.toBlob(
                  b2 => b2 ? resolve({ blob: b2, size: b2.size }) : reject(new Error('降级 PNG 也失败')),
                  'image/png'
                )
              } else {
                reject(new Error('转换产物为空'))
              }
            },
            mime,
            quality / 100
          )
        }

        img.onerror = () => {
          URL.revokeObjectURL(url)
          reject(new Error('图片加载失败，无法转换该格式，请尝试其他目标格式'))
        }

        img.src = url
      })
    },

    downloadOne(img) {
      if (!img.outBlob) return
      const cfg = FORMAT_CFG.find(f => f.value === this.targetFormat)
      const name = img.name.replace(/\.[^.]+$/, '') + cfg.ext
      this.triggerDownload(img.outBlob, name)
    },

    downloadAll() {
      if (this.downloadQueue.length === 0) return
      this.downloading = true
      // 逐个下载（浏览器安全策略不允许自动批量触发下载）
      const delay = 300
      this.downloadQueue.forEach((img, i) => {
        setTimeout(() => {
          this.downloadOne(img)
          if (i === this.downloadQueue.length - 1) {
            this.downloading = false
          }
        }, i * delay)
      })
    },

    async downloadZip() {
      if (this.downloadQueue.length === 0) return
      // 动态加载 JSZip
      try {
        await this.loadJSZip()
        const JSZip = window.JSZip
        const zip = new JSZip()
        const folder = zip.folder('converted_images')
        for (const img of this.downloadQueue) {
          if (!img.outBlob) continue
          const cfg = FORMAT_CFG.find(f => f.value === this.targetFormat)
          const name = img.name.replace(/\.[^.]+$/, '') + cfg.ext
          folder.file(name, img.outBlob)
        }
        const zipBlob = await zip.generateAsync({ type: 'blob' })
        this.triggerDownload(zipBlob, 'converted_images.zip')
      } catch (err) {
        alert('ZIP 打包失败，请逐个下载。' + err.message)
      }
    },

    async loadJSZip() {
      if (window.JSZip) return
      return new Promise((resolve, reject) => {
        const script = document.createElement('script')
        script.src = 'https://cdnjs.cloudflare.com/ajax/libs/jszip/3.10.1/jszip.min.js'
        script.onload = resolve
        script.onerror = () => reject(new Error('JSZip 加载失败，请检查网络'))
        document.head.appendChild(script)
      })
    },

    triggerDownload(blob, filename) {
      const url = URL.createObjectURL(blob)
      const a = document.createElement('a')
      a.href = url
      a.download = filename
      document.body.appendChild(a)
      a.click()
      document.body.removeChild(a)
      setTimeout(() => URL.revokeObjectURL(url), 1000)
    },

    selectAll() {
      if (this.allSelected) {
        this.images.forEach(i => { i.selected = false })
      } else {
        this.images.forEach(i => { i.selected = true })
      }
    },

    clearAll() {
      this.images = []
      this.progressDone = 0
      this.progressTotal = 0
      this.convertingBatch = false
    },

    previewImage(img) {
      this.preview = img
    },

    formatSize(bytes) {
      if (bytes < 1024) return bytes + ' B'
      if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(1) + ' KB'
      return (bytes / (1024 * 1024)).toFixed(2) + ' MB'
    },

    sizeChange(img) {
      if (!img.outSize || !img.srcSize) return ''
      const delta = img.outSize - img.srcSize
      const pct = (delta / img.srcSize * 100).toFixed(0)
      return delta > 0 ? '+' + pct + '%' : pct + '%'
    },
  },
}
</script>

<style scoped>
/* === Reset & Wrapper === */
.ic-wrapper {
  max-width: 940px;
  margin: 0 auto;
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
}
.ic-wrapper *,
.ic-wrapper *::before,
.ic-wrapper *::after {
  box-sizing: border-box;
}

/* === Header === */
.ic-header {
  text-align: center;
  margin-bottom: 20px;
}
.ic-title {
  font-size: 20px;
  font-weight: 700;
  margin: 0 0 6px;
}
.ic-desc {
  margin: 0;
  color: #888;
  font-size: 13px;
}

/* === Toolbar === */
.ic-toolbar {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 14px;
  padding: 14px 16px;
  background: var(--codeBg, #f8f8f8);
  border-radius: 10px;
  margin-bottom: 16px;
}
.ic-field {
  display: flex;
  align-items: center;
  gap: 8px;
}
.ic-field label {
  font-size: 13px;
  white-space: nowrap;
  color: #555;
}
.ic-field select {
  padding: 6px 30px 6px 10px;
  border: 1px solid #d0d0d0;
  border-radius: 6px;
  font-size: 13px;
  background: #fff;
  cursor: pointer;
  appearance: none;
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='12' height='12' viewBox='0 0 24 24' fill='none' stroke='%23888' stroke-width='2'%3E%3Cpolyline points='6,9 12,15 18,9'/%3E%3C/svg%3E");
  background-repeat: no-repeat;
  background-position: right 8px center;
}
.ic-field select:focus {
  outline: none;
  border-color: #4a90d9;
}
.ic-field input[type=range] {
  width: 80px;
  accent-color: #4a90d9;
}
.ic-qval {
  font-size: 12px;
  color: #666;
  min-width: 32px;
}

.ic-actions {
  display: flex;
  gap: 8px;
  flex-wrap: wrap;
  margin-left: auto;
}
.ic-btn {
  padding: 7px 16px;
  border: 1px solid #d0d0d0;
  border-radius: 6px;
  font-size: 13px;
  cursor: pointer;
  background: #fff;
  transition: all 0.15s;
}
.ic-btn:hover:not(:disabled) {
  background: #eef5ff;
  border-color: #4a90d9;
}
.ic-btn:disabled {
  opacity: 0.4;
  cursor: not-allowed;
}
.ic-btn-dl {
  background: #4a90d9;
  color: #fff;
  border-color: #4a90d9;
}
.ic-btn-dl:hover:not(:disabled) {
  background: #3a7bc8;
}
.ic-btn-zip {
  background: #52b788;
  color: #fff;
  border-color: #52b788;
}
.ic-btn-zip:hover:not(:disabled) {
  background: #40916c;
}
.ic-btn-del {
  color: #e74c3c;
  border-color: #e74c3c;
}
.ic-btn-del:hover:not(:disabled) {
  background: #fff0f0;
}

/* === Drop Zone === */
.ic-dropzone {
  border: 2px dashed #d0d0d0;
  border-radius: 12px;
  padding: 40px 20px;
  text-align: center;
  cursor: pointer;
  transition: all 0.2s;
  background: var(--codeBg, #fafafa);
}
.ic-dropzone:hover,
.ic-dragover {
  border-color: #4a90d9;
  background: #f0f7ff;
}
.ic-dropzone-inner {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 8px;
}
.ic-dropicon {
  color: #4a90d9;
  opacity: 0.7;
}
.ic-droptext {
  font-size: 15px;
  color: #444;
  margin: 0;
}
.ic-drophint {
  font-size: 12px;
  color: #999;
  margin: 0;
}

/* === Image List === */
.ic-list {
  margin-top: 16px;
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
  gap: 12px;
}

/* === Card === */
.ic-card {
  display: flex;
  align-items: flex-start;
  gap: 10px;
  padding: 10px;
  border: 1px solid #e8e8e8;
  border-radius: 10px;
  background: #fff;
  transition: all 0.2s;
}
.ic-card:hover {
  border-color: #c0d0e8;
  box-shadow: 0 2px 8px rgba(0,0,0,0.06);
}
.ic-card-check {
  cursor: pointer;
  padding-top: 2px;
  flex-shrink: 0;
}
.ic-checkbox {
  display: inline-block;
  width: 18px;
  height: 18px;
  border: 2px solid #ccc;
  border-radius: 4px;
  position: relative;
  transition: all 0.15s;
}
.ic-checkbox.checked {
  background: #4a90d9;
  border-color: #4a90d9;
}
.ic-checkbox.checked::after {
  content: '';
  position: absolute;
  left: 5px;
  top: 1px;
  width: 5px;
  height: 10px;
  border: solid #fff;
  border-width: 0 2px 2px 0;
  transform: rotate(45deg);
}
.ic-card-preview {
  width: 72px;
  height: 54px;
  border-radius: 6px;
  overflow: hidden;
  flex-shrink: 0;
  cursor: zoom-in;
  background: #f0f0f0;
}
.ic-card-preview img {
  width: 100%;
  height: 100%;
  object-fit: cover;
}
.ic-card-info {
  flex: 1;
  min-width: 0;
  overflow: hidden;
}
.ic-card-name {
  font-size: 13px;
  font-weight: 600;
  margin: 0 0 4px;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}
.ic-card-meta {
  font-size: 11px;
  color: #888;
  margin: 0 0 2px;
  display: flex;
  gap: 8px;
  flex-wrap: wrap;
}
.ic-arrow {
  color: #4a90d9;
  font-weight: bold;
}
.ic-ratio {
  font-weight: 600;
}
.ic-smaller {
  color: #52b788;
}
.ic-larger {
  color: #e74c3c;
}
.ic-card-err {
  font-size: 11px;
  color: #e74c3c;
  margin: 2px 0 0;
}
.ic-card-actions {
  display: flex;
  align-items: center;
  flex-shrink: 0;
  gap: 6px;
}
.ic-card-btn {
  padding: 4px 12px;
  font-size: 12px;
  border-radius: 5px;
  border: 1px solid #d0d0d0;
  cursor: pointer;
  background: #fff;
  transition: all 0.15s;
  white-space: nowrap;
}
.ic-card-btn:hover {
  background: #eef5ff;
  border-color: #4a90d9;
}
.ic-card-dl {
  background: #52b788;
  color: #fff;
  border-color: #52b788;
}
.ic-card-dl:hover {
  background: #40916c;
}

/* Spinner */
.ic-spinner {
  display: inline-block;
  width: 18px;
  height: 18px;
  border: 2px solid #e0e0e0;
  border-top-color: #4a90d9;
  border-radius: 50%;
  animation: ic-spin 0.6s linear infinite;
}
@keyframes ic-spin {
  to { transform: rotate(360deg); }
}

/* Progress */
.ic-progress {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-top: 14px;
}
.ic-progress-bar {
  flex: 1;
  height: 6px;
  background: #e8e8e8;
  border-radius: 3px;
  overflow: hidden;
}
.ic-progress-fill {
  height: 100%;
  background: linear-gradient(90deg, #4a90d9, #52b788);
  transition: width 0.3s;
  border-radius: 3px;
}
.ic-progress-text {
  font-size: 12px;
  color: #666;
  white-space: nowrap;
}

/* Stats */
.ic-stats {
  margin-top: 12px;
  text-align: center;
  font-size: 13px;
  color: #666;
  display: flex;
  justify-content: center;
  gap: 12px;
}
.ic-stats .ic-ratio {
  font-weight: 700;
  font-size: 14px;
}

/* Modal */
.ic-modal {
  position: fixed;
  inset: 0;
  z-index: 9999;
  background: rgba(0,0,0,0.8);
  display: flex;
  align-items: center;
  justify-content: center;
}
.ic-modal-inner {
  position: relative;
  max-width: 90vw;
  max-height: 90vh;
}
.ic-modal-inner img {
  max-width: 90vw;
  max-height: 85vh;
  object-fit: contain;
  border-radius: 8px;
}
.ic-modal-close {
  position: absolute;
  top: -36px;
  right: 0;
  background: none;
  border: none;
  color: #fff;
  font-size: 24px;
  cursor: pointer;
  padding: 4px 8px;
}

/* Transitions */
.ic-card-enter,
.ic-card-leave-to {
  opacity: 0;
  transform: translateY(10px);
}
.ic-card-leave-active {
  position: absolute;
}

.ic-modal-enter-active,
.ic-modal-leave-active {
  transition: opacity 0.2s;
}
.ic-modal-enter,
.ic-modal-leave-to {
  opacity: 0;
}

/* Dark mode */
.theme--dark .ic-dropzone {
  background: #1e1e2e;
  border-color: #3a3a4a;
}
.theme--dark .ic-dropzone:hover,
.theme--dark .ic-dragover {
  background: #25253a;
  border-color: #5a9ae0;
}
.theme--dark .ic-card {
  background: #1e1e2e;
  border-color: #3a3a4a;
}
.theme--dark .ic-card-preview {
  background: #2a2a3a;
}
.theme--dark .ic-field select,
.theme--dark .ic-btn {
  background: #2a2a3a;
  border-color: #3a3a4a;
  color: #ddd;
}
.theme--dark .ic-toolbar {
  background: #181825;
}
.theme--dark .ic-droptext { color: #ccc; }
.theme--dark .ic-drophint { color: #777; }
.theme--dark .ic-card-name { color: #ddd; }
.theme--dark .ic-card-meta { color: #999; }

/* Responsive */
@media (max-width: 640px) {
  .ic-toolbar {
    flex-direction: column;
    align-items: stretch;
  }
  .ic-actions {
    margin-left: 0;
  }
  .ic-list {
    grid-template-columns: 1fr;
  }
}
</style>
