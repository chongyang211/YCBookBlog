<template>
  <div class="jt-wrapper">
    <!-- Header -->
    <div class="jt-header">
      <h2 class="jt-title">JSON 工具箱 · 浏览器端</h2>
      <p class="jt-desc">格式化 · 校验 · 树视图 · 转换 · 对比 · 统计，所有计算在本地完成，数据不上传服务器</p>
    </div>

    <!-- Tabs -->
    <div class="jt-tabs">
      <button
        v-for="tab in tabs"
        :key="tab.key"
        class="jt-tab"
        :class="{ active: active === tab.key }"
        @click="active = tab.key"
      >
        <span class="jt-tab-icon" v-html="tab.icon"></span>
        <span>{{ tab.label }}</span>
      </button>
    </div>

    <!-- 各 Panel：keep-alive 保留每个 tab 内部 state -->
    <keep-alive>
      <component :is="activeComponent" />
    </keep-alive>
  </div>
</template>

<script>
import FormatPanel from './json/FormatPanel.vue'
import ValidatePanel from './json/ValidatePanel.vue'
import TreePanel from './json/TreePanel.vue'
import ConvertPanel from './json/ConvertPanel.vue'
import DiffPanel from './json/DiffPanel.vue'
import StatsPanel from './json/StatsPanel.vue'
import './json/styles.css'

export default {
  name: 'JsonToolbox',
  components: { FormatPanel, ValidatePanel, TreePanel, ConvertPanel, DiffPanel, StatsPanel },
  data() {
    return {
      active: 'format',
      tabs: [
        { key: 'format',   label: '格式化', comp: 'FormatPanel',   icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><line x1="3" y1="6" x2="21" y2="6"/><line x1="3" y1="12" x2="15" y2="12"/><line x1="3" y1="18" x2="18" y2="18"/></svg>' },
        { key: 'validate', label: '校验',   comp: 'ValidatePanel', icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><polyline points="20 6 9 17 4 12"/></svg>' },
        { key: 'tree',     label: '树视图', comp: 'TreePanel',     icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><circle cx="6" cy="6" r="2"/><circle cx="18" cy="6" r="2"/><circle cx="18" cy="18" r="2"/><path d="M6 8v6a2 2 0 0 0 2 2h8M8 6h8"/></svg>' },
        { key: 'convert',  label: '转换',   comp: 'ConvertPanel',  icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><polyline points="17 1 21 5 17 9"/><path d="M3 11V9a4 4 0 0 1 4-4h14"/><polyline points="7 23 3 19 7 15"/><path d="M21 13v2a4 4 0 0 1-4 4H3"/></svg>' },
        { key: 'diff',     label: '对比',   comp: 'DiffPanel',     icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><path d="M9 3H4v18h5M15 3h5v18h-5M12 6v12"/></svg>' },
        { key: 'stats',    label: '统计',   comp: 'StatsPanel',    icon: '<svg viewBox="0 0 24 24" width="16" height="16" fill="none" stroke="currentColor" stroke-width="2"><line x1="18" y1="20" x2="18" y2="10"/><line x1="12" y1="20" x2="12" y2="4"/><line x1="6" y1="20" x2="6" y2="14"/></svg>' },
      ],
    }
  },
  computed: {
    activeComponent() {
      const t = this.tabs.find(x => x.key === this.active)
      return t ? t.comp : 'FormatPanel'
    },
  },
}
</script>
