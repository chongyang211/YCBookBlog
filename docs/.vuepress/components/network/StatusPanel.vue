<template>
  <div class="nt-panel">
    <div class="nt-toolbar">
      <span class="nt-toolbar-label">搜索</span>
      <input v-model="kw" class="nt-input" style="flex: 1; min-width: 220px;" placeholder="输入状态码或关键字，如 404 / not found" />
      <span class="nt-toolbar-label">分类</span>
      <select v-model="filter" class="nt-select">
        <option value="all">全部</option>
        <option value="1">1xx 信息</option>
        <option value="2">2xx 成功</option>
        <option value="3">3xx 重定向</option>
        <option value="4">4xx 客户端</option>
        <option value="5">5xx 服务器</option>
      </select>
    </div>

    <div class="nt-status-grid">
      <div
        v-for="s in filtered"
        :key="s.code"
        class="nt-status-card"
        :class="'nt-status-' + Math.floor(s.code / 100) + 'xx'"
      >
        <span class="nt-status-code">{{ s.code }}</span>
        <span class="nt-status-name">{{ s.name }}</span>
        <div class="nt-status-desc">{{ s.desc }}</div>
      </div>
    </div>

    <p v-if="!filtered.length" style="text-align: center; color: #888; padding: 30px;">未找到匹配项</p>
  </div>
</template>

<script>
import { HTTP_STATUS } from './utils'

export default {
  name: 'StatusPanel',
  data() { return { kw: '', filter: 'all', all: HTTP_STATUS } },
  computed: {
    filtered() {
      const k = this.kw.trim().toLowerCase()
      return this.all.filter(s => {
        if (this.filter !== 'all' && Math.floor(s.code / 100) !== Number(this.filter)) return false
        if (!k) return true
        return String(s.code).includes(k) || s.name.toLowerCase().includes(k) || s.desc.toLowerCase().includes(k)
      })
    },
  },
}
</script>
