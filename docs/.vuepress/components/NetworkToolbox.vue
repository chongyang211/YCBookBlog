<template>
  <div class="nt-wrapper">
    <div class="nt-header">
      <h2 class="nt-title">网络工具箱 · 浏览器端</h2>
      <p class="nt-desc">URL 解析 / UA 识别 / HTTP 状态码 / MIME / CIDR 子网计算，纯前端运算</p>
    </div>

    <div class="nt-tabs">
      <button
        v-for="tab in tabs"
        :key="tab.key"
        class="nt-tab"
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
import UrlPanel from './network/UrlPanel.vue'
import UaPanel from './network/UaPanel.vue'
import StatusPanel from './network/StatusPanel.vue'
import MimePanel from './network/MimePanel.vue'
import CidrPanel from './network/CidrPanel.vue'
import './network/styles.css'

export default {
  name: 'NetworkToolbox',
  components: { UrlPanel, UaPanel, StatusPanel, MimePanel, CidrPanel },
  data() {
    return {
      active: 'url',
      tabs: [
        { key: 'url',    label: 'URL 解析',  comp: 'UrlPanel',    emoji: '🔗' },
        { key: 'ua',     label: 'UA 解析',   comp: 'UaPanel',     emoji: '🌐' },
        { key: 'status', label: 'HTTP 状态', comp: 'StatusPanel', emoji: '📡' },
        { key: 'mime',   label: 'MIME',      comp: 'MimePanel',   emoji: '📋' },
        { key: 'cidr',   label: 'CIDR',      comp: 'CidrPanel',   emoji: '🧮' },
      ],
    }
  },
  computed: {
    activeComponent() {
      const t = this.tabs.find(x => x.key === this.active)
      return t ? t.comp : 'UrlPanel'
    },
  },
}
</script>
