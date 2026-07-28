<template>
  <div class="cl-panel">
    <div class="cl-picker">
      <div>
        <div class="cl-picker-preview" :style="{ background: hex }"></div>
        <input type="color" class="cl-picker-native" :value="hex" @input="onNative" />
        <div style="margin-top: 10px;">
          <label style="font-size: 12px; color: #888;">从文本解析</label>
          <input v-model="textInput" class="cl-input cl-input-mono" style="width: 100%; margin-top: 4px;" placeholder="#e91e63 / rgb(...) / hsl(...) / red" @input="parseText" />
          <div v-if="textErr" class="cl-msg cl-msg-err" style="margin-top: 6px;">{{ textErr }}</div>
        </div>
      </div>

      <div>
        <div class="cl-slider-row">
          <span class="cl-slider-label">R</span>
          <input v-model.number="r" type="range" min="0" max="255" class="cl-slider" />
          <span class="cl-slider-val">{{ r }}</span>
        </div>
        <div class="cl-slider-row">
          <span class="cl-slider-label">G</span>
          <input v-model.number="g" type="range" min="0" max="255" class="cl-slider" />
          <span class="cl-slider-val">{{ g }}</span>
        </div>
        <div class="cl-slider-row">
          <span class="cl-slider-label">B</span>
          <input v-model.number="b" type="range" min="0" max="255" class="cl-slider" />
          <span class="cl-slider-val">{{ b }}</span>
        </div>
        <div class="cl-slider-row">
          <span class="cl-slider-label">A</span>
          <input v-model.number="a" type="range" min="0" max="100" class="cl-slider" />
          <span class="cl-slider-val">{{ (a / 100).toFixed(2) }}</span>
        </div>

        <div class="cl-picker-formats" style="margin-top: 10px;">
          <div v-for="f in formats" :key="f.label" class="cl-picker-format">
            <div class="cl-picker-format-label">{{ f.label }}</div>
            <div class="cl-picker-format-val">
              <span>{{ f.value }}</span>
              <button class="cl-picker-format-copy" @click="clCopy(f.value)">复制</button>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { hexToRgb, rgbToHex, rgbToHsl, rgbToHsv, parseColor, copyMixin } from './utils'

export default {
  name: 'PickerPanel',
  mixins: [copyMixin],
  data() {
    return { r: 233, g: 30, b: 99, a: 100, textInput: '#e91e63', textErr: '' }
  },
  computed: {
    rgb() { return { r: this.r, g: this.g, b: this.b, a: this.a / 100 } },
    hex() { return rgbToHex(this.r, this.g, this.b) },
    formats() {
      const { r, g, b } = this
      const alpha = (this.a / 100).toFixed(2)
      const hsl = rgbToHsl(r, g, b)
      const hsv = rgbToHsv(r, g, b)
      const hexAlpha = Math.round((this.a / 100) * 255).toString(16).padStart(2, '0')
      return [
        { label: 'HEX',   value: this.hex },
        { label: 'HEXA',  value: this.hex + hexAlpha },
        { label: 'RGB',   value: `rgb(${r}, ${g}, ${b})` },
        { label: 'RGBA',  value: `rgba(${r}, ${g}, ${b}, ${alpha})` },
        { label: 'HSL',   value: `hsl(${hsl.h}, ${hsl.s}%, ${hsl.l}%)` },
        { label: 'HSLA',  value: `hsla(${hsl.h}, ${hsl.s}%, ${hsl.l}%, ${alpha})` },
        { label: 'HSV',   value: `hsv(${hsv.h}, ${hsv.s}%, ${hsv.v}%)` },
        { label: 'CSS Var', value: `--color: ${this.hex};` },
      ]
    },
  },
  methods: {
    onNative(e) {
      const c = hexToRgb(e.target.value)
      if (c) { this.r = c.r; this.g = c.g; this.b = c.b }
    },
    parseText() {
      this.textErr = ''
      const c = parseColor(this.textInput)
      if (!c) { this.textErr = '无法识别该颜色'; return }
      this.r = c.r; this.g = c.g; this.b = c.b
      if (c.a !== undefined) this.a = Math.round(c.a * 100)
    },
  },
}
</script>
