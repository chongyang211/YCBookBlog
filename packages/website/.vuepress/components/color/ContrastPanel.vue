<template>
  <div class="cl-panel">
    <div class="cl-toolbar">
      <span class="cl-toolbar-label">前景</span>
      <input type="color" class="cl-stop-color" style="width: 60px; height: 32px;" v-model="fg" />
      <input v-model="fg" class="cl-input cl-input-mono" style="width: 120px;" />

      <span class="cl-toolbar-label" style="margin-left: 14px;">背景</span>
      <input type="color" class="cl-stop-color" style="width: 60px; height: 32px;" v-model="bg" />
      <input v-model="bg" class="cl-input cl-input-mono" style="width: 120px;" />

      <button class="cl-btn" @click="swap">↔ 交换</button>
    </div>

    <div class="cl-contrast-preview" :style="{ background: bg, color: fg }">
      <p style="font-size: 28px; margin: 0 0 4px;">大字号示例 28px</p>
      <p style="font-size: 16px; margin: 0;">普通正文 16px · The quick brown fox jumps over the lazy dog. 中文示例：杨充的技术博客</p>
    </div>

    <div class="cl-contrast-result">
      <div class="cl-contrast-card" :class="ratio >= 4.5 ? 'cl-contrast-card-pass' : 'cl-contrast-card-fail'">
        <div class="cl-contrast-label">对比度</div>
        <div class="cl-contrast-val">{{ ratio.toFixed(2) }} : 1</div>
      </div>
      <div class="cl-contrast-card" :class="result.aaNormal ? 'cl-contrast-card-pass' : 'cl-contrast-card-fail'">
        <div class="cl-contrast-label">WCAG AA · 普通文本</div>
        <div class="cl-contrast-val">≥ 4.5</div>
        <span class="cl-contrast-badge" :class="result.aaNormal ? 'cl-contrast-badge-pass' : 'cl-contrast-badge-fail'">
          {{ result.aaNormal ? '通过' : '不通过' }}
        </span>
      </div>
      <div class="cl-contrast-card" :class="result.aaLarge ? 'cl-contrast-card-pass' : 'cl-contrast-card-fail'">
        <div class="cl-contrast-label">WCAG AA · 大字号</div>
        <div class="cl-contrast-val">≥ 3.0</div>
        <span class="cl-contrast-badge" :class="result.aaLarge ? 'cl-contrast-badge-pass' : 'cl-contrast-badge-fail'">
          {{ result.aaLarge ? '通过' : '不通过' }}
        </span>
      </div>
      <div class="cl-contrast-card" :class="result.aaaNormal ? 'cl-contrast-card-pass' : 'cl-contrast-card-fail'">
        <div class="cl-contrast-label">WCAG AAA · 普通</div>
        <div class="cl-contrast-val">≥ 7.0</div>
        <span class="cl-contrast-badge" :class="result.aaaNormal ? 'cl-contrast-badge-pass' : 'cl-contrast-badge-fail'">
          {{ result.aaaNormal ? '通过' : '不通过' }}
        </span>
      </div>
      <div class="cl-contrast-card" :class="result.aaaLarge ? 'cl-contrast-card-pass' : 'cl-contrast-card-fail'">
        <div class="cl-contrast-label">WCAG AAA · 大字号</div>
        <div class="cl-contrast-val">≥ 4.5</div>
        <span class="cl-contrast-badge" :class="result.aaaLarge ? 'cl-contrast-badge-pass' : 'cl-contrast-badge-fail'">
          {{ result.aaaLarge ? '通过' : '不通过' }}
        </span>
      </div>
    </div>
  </div>
</template>

<script>
import { hexToRgb, contrastRatio, evalContrast } from './utils'

export default {
  name: 'ContrastPanel',
  data() { return { fg: '#ffffff', bg: '#4a90d9' } },
  computed: {
    ratio() {
      const f = hexToRgb(this.fg), b = hexToRgb(this.bg)
      if (!f || !b) return 0
      return contrastRatio(f, b)
    },
    result() { return evalContrast(this.ratio) },
  },
  methods: {
    swap() { [this.fg, this.bg] = [this.bg, this.fg] },
  },
}
</script>
