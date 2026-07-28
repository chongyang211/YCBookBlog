<template>
  <div class="dt-wrapper">
    <div class="dt-header">
      <h2 class="dt-title">文档转化 · 浏览器端</h2>
      <p class="dt-desc">Markdown ↔ HTML · PDF 合并/拆分 · 文档模板，所有计算在本地完成，文件不上传服务器</p>
    </div>

    <div class="dt-tabs">
      <button
        v-for="tab in tabs"
        :key="tab.key"
        class="dt-tab"
        :class="{ active: active === tab.key }"
        @click="active = tab.key"
      >
        <span class="dt-tab-icon" v-html="tab.icon"></span>
        <span>{{ tab.label }}</span>
      </button>
    </div>

    <keep-alive>
      <component :is="activeComponent" />
    </keep-alive>
  </div>
</template>

<script>
import MdToHtmlPanel from './doc/MdToHtmlPanel.vue'
import HtmlToMdPanel from './doc/HtmlToMdPanel.vue'
import PdfPanel from './doc/PdfPanel.vue'
import TemplatePanel from './doc/TemplatePanel.vue'
import './doc/styles.css'

export default {
  name: 'DocToolbox',
  components: { MdToHtmlPanel, HtmlToMdPanel, PdfPanel, TemplatePanel },
  data() {
    return {
      active: 'md2html',
      tabs: [
        { key: 'md2html', label: 'MD → HTML', comp: 'MdToHtmlPanel', icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><path d="M4 4h16v16H4z"/><path d="M8 8v8M8 12l3 3 3-3M16 16V8"/></svg>' },
        { key: 'html2md', label: 'HTML → MD', comp: 'HtmlToMdPanel', icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><polyline points="16 18 22 12 16 6"/><polyline points="8 6 2 12 8 18"/></svg>' },
        { key: 'pdf',     label: 'PDF 工具', comp: 'PdfPanel',      icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14 2 14 8 20 8"/></svg>' },
        { key: 'tpl',     label: '文档模板', comp: 'TemplatePanel', icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><rect x="3" y="3" width="18" height="18" rx="2"/><line x1="3" y1="9" x2="21" y2="9"/><line x1="9" y1="21" x2="9" y2="9"/></svg>' },
      ],
    }
  },
  computed: {
    activeComponent() {
      const t = this.tabs.find(x => x.key === this.active)
      return t ? t.comp : 'MdToHtmlPanel'
    },
  },
}
</script>
