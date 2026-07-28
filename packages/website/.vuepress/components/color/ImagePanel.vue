<template>
  <div class="cl-panel">
    <div class="cl-image-drop" @click="pick" @dragover.prevent @drop.prevent="onDrop">
      <p style="margin: 0;">📁 点击或拖入图片 · 自动提取主色调（最多 8 个）</p>
      <p style="margin: 4px 0 0; font-size: 12px; opacity: 0.8;">所有处理在本地完成，图片不上传</p>
    </div>
    <input ref="file" type="file" accept="image/*" style="display: none;" @change="onPick" />

    <div v-if="error" class="cl-msg cl-msg-err">{{ error }}</div>

    <img v-if="imgSrc" :src="imgSrc" class="cl-image-preview" alt="preview" />

    <div v-if="colors.length" class="cl-palette">
      <div v-for="(c, i) in colors" :key="i" class="cl-swatch" @click="clCopy(toHex(c))">
        <div class="cl-swatch-color" :style="{ background: toHex(c) }"></div>
        <div class="cl-swatch-info">{{ toHex(c) }}</div>
      </div>
    </div>
  </div>
</template>

<script>
import { rgbToHex, extractColorsFromImage, copyMixin } from './utils'

export default {
  name: 'ImagePanel',
  mixins: [copyMixin],
  data() { return { imgSrc: '', colors: [], error: '' } },
  methods: {
    pick() { this.$refs.file.click() },
    onPick(e) {
      const f = e.target.files && e.target.files[0]
      if (f) this.process(f)
    },
    onDrop(e) {
      const f = e.dataTransfer.files && e.dataTransfer.files[0]
      if (f) this.process(f)
    },
    async process(file) {
      this.error = ''
      if (!file.type.startsWith('image/')) { this.error = '请选择图片文件'; return }
      const url = URL.createObjectURL(file)
      this.imgSrc = url
      try { this.colors = await extractColorsFromImage(file, 8) }
      catch (e) { this.error = e.message }
    },
    toHex(c) { return rgbToHex(c.r, c.g, c.b) },
  },
}
</script>
