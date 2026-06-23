<template>
  <div class="cl-wrapper">
    <div class="cl-header">
      <h2 class="cl-title">颜色设计工具箱 · 浏览器端</h2>
      <p class="cl-desc">取色板 / 渐变 / 调色板 / 对比度 / 图片取色，本地运算</p>
    </div>

    <div class="cl-tabs">
      <button
        v-for="tab in tabs"
        :key="tab.key"
        class="cl-tab"
        :class="{ active: active === tab.key }"
        @click="active = tab.key"
      >
        <span>{{ tab.emoji }}</span>
        <span>{{ tab.label }}</span>
      </button>
    </div>

    <keep-alive>
      <component :is="activeComponent" />
    </keep-alive>
  </div>
</template>

<script>
import PickerPanel from './color/PickerPanel.vue'
import GradientPanel from './color/GradientPanel.vue'
import PalettePanel from './color/PalettePanel.vue'
import ContrastPanel from './color/ContrastPanel.vue'
import ImagePanel from './color/ImagePanel.vue'
import './color/styles.css'

export default {
  name: 'ColorToolbox',
  components: { PickerPanel, GradientPanel, PalettePanel, ContrastPanel, ImagePanel },
  data() {
    return {
      active: 'picker',
      tabs: [
        { key: 'picker',   label: '取色板', comp: 'PickerPanel',   emoji: '🎯' },
        { key: 'gradient', label: '渐变',   comp: 'GradientPanel', emoji: '🌈' },
        { key: 'palette',  label: '调色板', comp: 'PalettePanel',  emoji: '🎨' },
        { key: 'contrast', label: '对比度', comp: 'ContrastPanel', emoji: '👁' },
        { key: 'image',    label: '图片取色', comp: 'ImagePanel',  emoji: '🖼' },
      ],
    }
  },
  computed: {
    activeComponent() {
      const t = this.tabs.find(x => x.key === this.active)
      return t ? t.comp : 'PickerPanel'
    },
  },
}
</script>
