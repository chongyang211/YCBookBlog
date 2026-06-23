<template>
  <div class="mt-wrapper">
    <!-- Header -->
    <div class="mt-header">
      <h2 class="mt-title">代码压缩 / 美化 · 浏览器端</h2>
      <p class="mt-desc">JS / CSS / HTML / JSON / SQL / XML 压缩与格式化，本地运算，代码不上传服务器</p>
    </div>

    <!-- Tabs -->
    <div class="mt-tabs">
      <button
        v-for="tab in tabs"
        :key="tab.key"
        class="mt-tab"
        :class="{ active: active === tab.key }"
        @click="active = tab.key"
      >
        <span class="mt-tab-icon">{{ tab.emoji }}</span>
        <span>{{ tab.label }}</span>
      </button>
    </div>

    <keep-alive>
      <component :is="activeComponent" />
    </keep-alive>
  </div>
</template>

<script>
import JsPanel from './minify/JsPanel.vue'
import CssPanel from './minify/CssPanel.vue'
import HtmlPanel from './minify/HtmlPanel.vue'
import JsonPanel from './minify/JsonPanel.vue'
import SqlPanel from './minify/SqlPanel.vue'
import LinesPanel from './minify/LinesPanel.vue'
import './minify/styles.css'

export default {
  name: 'MinifyToolbox',
  components: { JsPanel, CssPanel, HtmlPanel, JsonPanel, SqlPanel, LinesPanel },
  data() {
    return {
      active: 'js',
      tabs: [
        { key: 'js',    label: 'JS',     comp: 'JsPanel',    emoji: '🟨' },
        { key: 'css',   label: 'CSS',    comp: 'CssPanel',   emoji: '🎨' },
        { key: 'html',  label: 'HTML',   comp: 'HtmlPanel',  emoji: '🌐' },
        { key: 'json',  label: 'JSON',   comp: 'JsonPanel',  emoji: '{ }' },
        { key: 'sql',   label: 'SQL',    comp: 'SqlPanel',   emoji: '🗄' },
        { key: 'lines', label: '行处理', comp: 'LinesPanel', emoji: '📋' },
      ],
    }
  },
  computed: {
    activeComponent() {
      const t = this.tabs.find(x => x.key === this.active)
      return t ? t.comp : 'JsPanel'
    },
  },
}
</script>
