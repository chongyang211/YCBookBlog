<template>
  <div class="nt-panel">
    <div class="nt-toolbar">
      <span class="nt-toolbar-label">搜索</span>
      <input v-model="kw" class="nt-input" style="flex: 1; min-width: 220px;" placeholder="输入扩展名或 MIME，如 png / image / json" />
    </div>

    <div class="nt-mime-grid">
      <div v-for="m in filtered" :key="m.ext + m.type" class="nt-mime-item">
        <span class="nt-mime-ext">.{{ m.ext }}</span>
        <span class="nt-mime-type">{{ m.type }}</span>
      </div>
    </div>

    <p v-if="!filtered.length" style="text-align: center; color: #888; padding: 30px;">未找到匹配项</p>
  </div>
</template>

<script>
import { MIME_TYPES } from './utils'

export default {
  name: 'MimePanel',
  data() { return { kw: '', all: MIME_TYPES } },
  computed: {
    filtered() {
      const k = this.kw.trim().toLowerCase().replace(/^\./, '')
      if (!k) return this.all
      return this.all.filter(m => m.ext.includes(k) || m.type.toLowerCase().includes(k))
    },
  },
}
</script>
