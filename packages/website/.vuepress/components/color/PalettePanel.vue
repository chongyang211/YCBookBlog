<template>
  <div class="cl-panel">
    <div class="cl-toolbar">
      <span class="cl-toolbar-label">基础色</span>
      <input type="color" class="cl-stop-color" style="width: 60px; height: 32px;" :value="base" @input="onBaseChange" />
      <input v-model="hexInput" class="cl-input cl-input-mono" style="width: 120px;" @input="onHexInput" />
    </div>

    <div v-for="scheme in schemeList" :key="scheme.key">
      <h3 style="margin: 16px 0 6px; font-size: 14px; color: #555;">{{ scheme.label }}</h3>
      <div class="cl-palette">
        <div
          v-for="(c, i) in scheme.colors"
          :key="i"
          class="cl-swatch"
          @click="clCopy(toHex(c))"
        >
          <div class="cl-swatch-color" :style="{ background: toHex(c) }"></div>
          <div class="cl-swatch-info">{{ toHex(c) }}</div>
        </div>
      </div>
    </div>

    <h3 style="margin: 16px 0 6px; font-size: 14px; color: #555;">明暗梯度</h3>
    <div class="cl-palette">
      <div v-for="(c, i) in shadeList" :key="i" class="cl-swatch" @click="clCopy(toHex(c))">
        <div class="cl-swatch-color" :style="{ background: toHex(c) }"></div>
        <div class="cl-swatch-info">{{ toHex(c) }}</div>
      </div>
    </div>
  </div>
</template>

<script>
import { hexToRgb, rgbToHex, paletteSchemes, shades, copyMixin } from './utils'

export default {
  name: 'PalettePanel',
  mixins: [copyMixin],
  data() { return { base: '#4a90d9', hexInput: '#4a90d9' } },
  computed: {
    baseRgb() { return hexToRgb(this.base) || { r: 74, g: 144, b: 217 } },
    schemes() { return paletteSchemes(this.baseRgb) },
    schemeList() {
      return [
        { key: 'complement', label: '互补色 Complement', colors: this.schemes.complement },
        { key: 'analogous', label: '类比色 Analogous', colors: this.schemes.analogous },
        { key: 'triad', label: '三角配色 Triad', colors: this.schemes.triad },
        { key: 'tetrad', label: '四方配色 Tetrad', colors: this.schemes.tetrad },
        { key: 'split', label: '分裂互补 Split-Complement', colors: this.schemes.splitComplement },
      ]
    },
    shadeList() { return shades(this.baseRgb, 9) },
  },
  methods: {
    onBaseChange(e) { this.base = e.target.value; this.hexInput = this.base },
    onHexInput() {
      const c = hexToRgb(this.hexInput)
      if (c) this.base = rgbToHex(c.r, c.g, c.b)
    },
    toHex(c) { return rgbToHex(c.r, c.g, c.b) },
  },
}
</script>
